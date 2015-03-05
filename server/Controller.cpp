#include "pch.h"
#include "Controller.h"
#include "Program.h"
#include "MyLogger.h"
#include "StorageEngine.h"
#include <kdutils.h>

#include <zmq.h>
#include <unistd.h>

#include "LinuxTimer.h"

using namespace std;
using namespace fmt;

// Controller
Controller::Controller()
	: zcontext(0), zsocket(0), zsocketREP(0)
{
	zcontext = zmq_ctx_new();
	
	m_startTime = getTicks();
	
	m_storage = new StorageEngine();
	
}
Controller::~Controller()
{
	if (zsocket)
		zmq_close(zsocket);
	if (zsocketREP)
		zmq_close(zsocketREP);
	if (zcontext)
		zmq_ctx_destroy(zsocketREP);
}

bool Controller::init()
{
	zsocket = zmq_socket(zcontext, ZMQ_PUB);
	int rc = zmq_bind(zsocket, "tcp://*:9999");
	if (rc != 0)
	{
		m_logger->logError(str(Format("Error during creating PUB socket {}") << zmq_strerror(errno)));
		return false;
	}
	
	zsocketREP = zmq_socket(zcontext, ZMQ_REP);
	rc = zmq_bind(zsocketREP, "tcp://*:10000");
	if (rc != 0)
	{
		m_logger->logError(str(Format("Error during creating REP socket {}") << zmq_strerror(errno)));
		return false;
	}
	
	m_logger->logInfo("Creating UDP server");
	m_server.setListener(this);
	m_server.setPort(9999);
	if (!m_server.init())
	{
		m_logger->logError(str(Format("Unable to configure UDP server: {}") << m_server.getLastError()));
		return false;
	}
	if (!m_server.bind())
	{
		m_logger->logError(str(Format("Unable to bind UDP server: {}") << m_server.getLastError()));
		return false;
	}
	
	m_checkTimer.setInterval(0, 100 * 1000);
	
	reload();
	
	return true;
}
void Controller::reload()
{
	m_sessKey = rand() % (99999 - 10000) + 10000;
	
	m_logger->logInfo("Loading persistent storage");
	m_storage->setPath("data");
	m_storage->load();
	
	m_initialized = false;
	
	m_inputsNames.clear();
	m_outputsNames.clear();
	m_tempsNames.clear();
	m_inputNameToId.clear();
	m_outputNameToId.clear();
	m_tempNameToId.clear();
	m_persistentOutputs.clear();
	
	for (auto& dev : m_devices)
	{
		m_logger->logInfo(str(Format("Deleting device [{}]") << dev->getName()));
		delete dev;
	}
	m_devices.clear();
}

void Controller::publish(string msg)
{
	msg = str(Format("#{}:{}") << msg << m_sessKey);
	int r = zmq_send(zsocket, msg.data(), msg.size(), 0);
	m_logger->logInfo(Format("PUB[{}] {}") << r << msg);
}
string Controller::processREQ(string msg)
{
	vector<string> p = explode(msg, ":");
	
	if (p.size() < 3)
		return "";
		
	int sessKey = atoi(p[p.size() - 1].c_str());
	
	if (sessKey != 0 && sessKey != m_sessKey)
	{
		m_logger->logInfo("Invalid sessKey");
		return "";
	}
	
	string type = p[0];
	if (type == "CTRL")
	{
		string cmd = p[1];
		if (cmd == "INIT-OK")
		{
			m_initialized = true;
		}
		else if (cmd == "RESET")
		{
			reload();
		}
		else if (cmd == "REGISTER-ETHERNET-DEVICE")
		{
			if (p.size() < 7)
				return "";
				
			int id = atoi(p[2].c_str());
			string ip = p[3];
			int port = atoi(p[4].c_str());
			string name = p[5];
			
			int dev = registerEthernetDevice(id, ip, port, name);
			return str(Format("{}") << dev);
		}
	}
	else if (type == "MESSAGE")
	{
		string cmd = p[1];
		if (cmd == "BROADCAST")
		{
			if (p.size() < 3)
				return "";
				
			string msg = p[2];
			
			publish(str(Format("MESSAGE:BROADCAST:{}") << msg));
			return "OK";
		}
	}
	else if (type == "OUTPUT")
	{
		string cmd = p[1];
		if (cmd == "SET")
		{
			if (p.size() < 5)
				return "";
				
			string name = p[2];
			int value = atoi(p[3].c_str());
			
			setOutput(name, value);
			return "";
		}
		else if (cmd == "TOGGLE")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			
			toggleOutput(name);
			return "";
		}
		else if (cmd == "GET")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			
			int val = getOutput(name);
			return str(Format("{}") << val);
		}
		else if (cmd == "SET-NAME")
		{
			if (p.size() < 5)
				return "";
				
			string id = p[2];
			string name = p[3];
			m_outputsNames[id] = name;
			m_outputNameToId[name] = id;
			
			return "";
		}
		else if (cmd == "SET-PERSISTENT")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			setOutputAsPersistent(name);
			
			return "";
		}
	}
	else if (type == "INPUT")
	{
		string cmd = p[1];
		if (cmd == "GET")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			
			int val = getInput(name);
			return str(Format("{}") << val);
		}
		else if (cmd == "SET-NAME")
		{
			if (p.size() < 5)
				return "";
				
			// int num = atoi(p[2].c_str());
			// string name = p[3];
			string id = p[2];
			string name = p[3];
			m_inputsNames[id] = name;
			m_inputNameToId[name] = id;
			
			// m_inputsNames[num] = name;
			
			return "";
		}
	}
	else if (type == "TEMP")
	{
		string cmd = p[1];
		if (cmd == "GET")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			
			float val = getTemp(name);
			return str(Format("{}") << val);
		}
		else if (cmd == "IS-VALID")
		{
			if (p.size() < 4)
				return "";
				
			string name = p[2];
			
			int val = isTempValid(name);
			return str(Format("{}") << val);
		}
		else if (cmd == "SET-NAME")
		{
			if (p.size() < 5)
				return "";
				
			string id = p[2];
			string name = p[3];
			m_tempsNames[id] = name;
			m_tempNameToId[name] = id;
			
			return "";
		}
	}
	else if (type == "COUNTER")
	{
		string cmd = p[1];
		if (cmd == "SET-NAME")
		{
			if (p.size() < 5)
				return "";
				
			string id = p[2];
			string name = p[3];
			m_countersNames[id] = name;
			m_counterNameToId[name] = id;
			
			return "";
		}
	}
	return "";
}

void Controller::run()
{
	zmq_pollitem_t items[3];
	items[0].socket = NULL;
	items[0].fd = m_server.getFd();
	items[0].events = ZMQ_POLLIN;
	items[1].socket = zsocketREP;
	items[1].events = ZMQ_POLLIN;
	items[2].socket = NULL;
	items[2].fd = m_checkTimer.getFd();
	items[2].events = ZMQ_POLLIN;
	
	for (;;)
	{
		zmq_poll(items, 3, -1);
		
		if (items[0].revents & ZMQ_POLLIN)
		{
			m_server.process();
		}
		if (items[1].revents & ZMQ_POLLIN)
		{
			char data[1024];
			int r = zmq_recv(zsocketREP, data, sizeof(data), ZMQ_DONTWAIT);
			if (r != -1)
			{
				string s(data, r);
				
				m_logger->logInfo(Format("ZMQ {}") << s);
				string rep = processREQ(s);
				
				m_logger->logInfo(Format("REP {}") << rep);
				zmq_send(zsocketREP, rep.data(), rep.size(), 0);
			}
		}
		if (items[2].revents & ZMQ_POLLIN)
		{
			if (m_checkTimer.wait())
			{
				for (size_t i = 0; i < m_devices.size(); i++)
				{
					EthernetDevice *dev = m_devices[i];
					dev->process();
				}
				
				if (!m_initialized)
				{
					static uint32_t lastInitTime = 0;
					if (getTicks() - lastInitTime >= 500)
					{
						publish("CTRL:INIT");
						lastInitTime = getTicks();
					}
				}
			}
		}
	}
}
void Controller::savePersistentState(const string& id)
{
	bool state = m_outputValues[id];
	m_storage->setInt("output-" + id, state);
	m_storage->save();
}

// Registering devices and proviers
int Controller::registerEthernetDevice(uint32_t id, const string& ip, uint16_t port, const string& name)
{
	EthernetDevice *dev = new EthernetDevice(&m_server, this, id, ip, port, name);
	int idx = m_devices.size();
	m_devices.push_back(dev);
	m_logger->logInfo(Format("Registered ethernet device #{} with IP {}") << idx << ip);
	return idx;
}

// Inputs
bool Controller::getInput(const string& name)
{
	string id = getInputID(name);
	if (m_inputValues.find(id) == m_inputValues.end())
		m_inputValues[id] = 0;
	return m_inputValues[id];
	
	// EthernetDevice *dev;
	// IInputProvider *p;
	// int num;
	// if (!findProvider(name, m_inputNameToId, dev, p, num))
	// {
	// m_logger->logWarning(str(Format("Unable to find input provider for {}") << name));
	// return false;
	// }
	// return p->getInputState(num);
}

// Outputs
void Controller::setOutput(const string& name, bool state)
{
	string id = getOutputID(name);
	if (getOutput(id) != state)
	{
		setOutputProvider(name, state);

		publish(str(Format("OUTPUT:CHANGED:{}:{}:{}") << id << getOutputName(id) << state));
		
		if (state)
			m_logger->logInfo(Format("[output] Output {} enabled") << getOutputName(id));
		else
			m_logger->logInfo(Format("[output] Output {} disabled") << getOutputName(id));
			
		m_outputValues[id] = state;
		savePersistentState(id);
	}
}
bool Controller::getOutput(const string& name)
{
	string id = getOutputID(name);
	if (m_outputValues.find(id) == m_outputValues.end())
		m_outputValues[id] = 0;
	return m_outputValues[id];
	// IOutputProvider *p;
	// EthernetDevice *dev;
	// int num;
	// if (!findProvider(name, m_outputNameToId, dev, p, num))
	// {
	// m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
	// return false;
	// }
	// return p->getOutputState(num);
}
void Controller::toggleOutput(const string& name)
{
	bool state = getOutput(name);
	setOutput(name, !state);
}
void Controller::setOutputAsPersistent(const string& name)
{
	string id = name;
	auto v = m_outputNameToId.find(id);
	if (v != m_outputNameToId.end())
		id = v->second;
		
	m_persistentOutputs[id] = 1;
}

// Temperatures
bool Controller::isTempValid(const string& name)
{
	ITempProvider *p;
	EthernetDevice *dev;
	int num;
	if (!findProvider(name, m_tempNameToId, dev, p, num))
	{
		m_logger->logWarning(str(Format("Unable to find temperature provider for {}") << num));
		return false;
	}
	return p->isTempValid(num);
}
float Controller::getTemp(const string& name)
{
	ITempProvider *p;
	EthernetDevice *dev;
	int num;
	if (!findProvider(name, m_tempNameToId, dev, p, num))
	{
		m_logger->logWarning(str(Format("Unable to find temperature provider for {}") << num));
		return 0;
	}
	return p->getTemp(num);
}

template<typename T>
bool Controller::findProvider(const string& name, const map<string, string>& idMap, EthernetDevice*& dev, T*& provider, int& num)
{
	string id = name;
	auto v = idMap.find(id);
	if (v != idMap.end())
		id = v->second;
		
	size_t del = id.find_first_of("-");
	if (del == string::npos)
		return false;
	string devName = id.substr(0, del);
	string numStr = id.substr(del + 1);
	num = atoi(numStr.c_str());
	dev = findDevice(devName);
	
	if (!dev || !dev->hasProvider(T::getTypeStatic()))
		return false;
		
	provider = dynamic_cast<T*>(dev->getProvider(T::getTypeStatic()));
	
	if (provider && num < provider->getAmount())
		return true;
		
	return false;
}
EthernetDevice* Controller::findDevice(const string& name)
{
	for (int i = 0; i < m_devices.size(); i++)
		if (m_devices[i]->getName() == name)
			return m_devices[i];
	return 0;
}

// IEthernetDataListener
void Controller::onEthernetDataReceived(const string& ip, const void* buffer, int len)
{
	for (size_t i = 0; i < m_devices.size(); i++)
	{
		EthernetDevice *dev = m_devices[i];
		if (dev->getIP() == ip)
		{
			dev->processData(buffer, len);
			return;
		}
	}
}

// IInputProviderListener
void Controller::onInputInitState(IInputProvider* provider, const string& id, int state)
{
	m_logger->logInfo(str(Format("Input {} has got new state {}") << getInputName(id) << state));
	
	publish(str(Format("INPUT:INIT-STATE:{}:{}:{}") << id << getInputName(id) << state));
	
	m_inputValues[id] = state;
}
void Controller::onInputChanged(IInputProvider* provider, const string& id, int state)
{
	if (m_inputValues[id] != state)
	{
		if (state)
			m_logger->logInfo(str(Format("Input {} changed 0->1") << getInputName(id)));
		else
			m_logger->logInfo(str(Format("Input {} changed 1->0") << getInputName(id)));
			
		publish(str(Format("INPUT:CHANGED:{}:{}:{}") << id << getInputName(id) << state));
	}
	m_inputValues[id] = state;
}

// IIRProviderListener
void Controller::onIRCodeReceived(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:NEW-CODE:0x{0:08x}") << code));
}
void Controller::onIRButtonPressed(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:PRESSED:0x{0:08x}") << code));
}
void Controller::onIRButtonReleased(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:RELEASED:0x{0:08x}") << code));
}

// IOutputProviderListener
void Controller::onInitialStatesRequest(IOutputProvider* provider)
{
	m_logger->logInfo(str(Format("Initial states requested")));
	
	for (auto& it : m_outputValues)
	{
		const string& outputId = it.first;
		int state = m_outputValues[outputId];
		m_logger->logInfo(str(Format("Restoring output {} state to {}") << outputId << state));
		setOutputProvider(outputId, state);
	}
}

// ICounterProviderListener
void Controller::onCounterChanged(ICounterProvider* provider, const string& id, uint32_t value)
{
	EthernetDevice *dev = provider->getDevice();
	m_logger->logInfo(str(Format("Counter {} changed to {}") << getCounterName(id) << value));
	
	publish(str(Format("COUNTER:CHANGED:{}:{}:{}") << id << getCounterName(id) << value));
}

void Controller::setOutputProvider(const string& name, bool state)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	int num;
	
	if (!findProvider(name, m_outputNameToId, dev, p, num))
	{
		m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
		return;
	}
	
	p->setOutputState(num, state);
}

string Controller::getInputName(const string& id)
{
	return getNameOrID(id, m_inputsNames);
}
string Controller::getOutputName(const string& id)
{
	return getNameOrID(id, m_outputsNames);
}
string Controller::getCounterName(const string& id)
{
	return getNameOrID(id, m_countersNames);
}
string Controller::getInputID(const string& name)
{
	return getNameOrID(name, m_inputNameToId);
}
string Controller::getOutputID(const string& name)
{
	return getNameOrID(name, m_outputNameToId);
}
string Controller::getCounterID(const string& name)
{
	return getNameOrID(name, m_counterNameToId);
}
string Controller::getNameOrID(const string& id, const map<string, string>& idMap)
{
	auto v = idMap.find(id);
	if (v != idMap.end())
		return v->second;
	return id;
}

