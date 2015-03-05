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
	
	m_inputs.clear();
	m_outputs.clear();
	m_temps.clear();
	m_counters.clear();
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
			for (auto& it : m_persistentOutputs)
			{
				const string& id = it.first;
				int state = m_storage->getInt("output-" + id, 0);
				setOutput(id, state);
			}
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
		else if (cmd == "LOG")
		{
			if (p.size() < 3)
				return "";
				
			string msg = p[2];
			
			m_userLogger->logInfo(msg);
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
			findValue(id, m_outputs).name = name;
			
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
				
			string id = p[2];
			string name = p[3];
			findValue(id, m_inputs).name = name;
			
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
			findValue(id, m_temps).name = name;
			
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
			findValue(id, m_counters).name = name;
			
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
	bool state = getOutput(id);
	m_storage->setInt("output-" + id, state);
	m_storage->save();
}

// Registering devices and providers
int Controller::registerEthernetDevice(uint32_t id, const string& ip, uint16_t port, const string& name)
{
	EthernetDevice *dev = new EthernetDevice(&m_server, this, id, ip, port, name);
	int idx = m_devices.size();
	m_devices.push_back(dev);
	m_logger->logInfo(Format("Registered ethernet device #{} with IP {}:{}") << idx << ip << port);
	return idx;
}

// Inputs
bool Controller::getInput(const string& name)
{
	TInputProviderValue& info = findValue(name, m_inputs);
	if (info.initialized)
		return info.value;
	else
		return false;
}

// Outputs
void Controller::setOutput(const string& name, bool state)
{
	TOutputProviderValue& info = findValue(name, m_outputs);
	
	if (info.value != state || !info.initialized)
	{
		setOutputInProvider(info.id, state);
		
		publish(str(Format("OUTPUT:CHANGED:{}:{}:{}") << info.id << info.name << state));
		
		if (state)
			m_logger->logInfo(Format("[output] Output {} enabled") << info.name);
		else
			m_logger->logInfo(Format("[output] Output {} disabled") << info.name);
			
		info.setValue(state);
		savePersistentState(info.id);
	}
}
bool Controller::getOutput(const string& name)
{
	TOutputProviderValue& info = findValue(name, m_outputs);
	if (info.initialized)
		return info.value;
	else
		return false;
}
void Controller::toggleOutput(const string& name)
{
	bool state = getOutput(name);
	setOutput(name, !state);
}
void Controller::setOutputAsPersistent(const string& name)
{
	TOutputProviderValue& info = findValue(name, m_outputs);
	m_persistentOutputs[info.id] = 1;
}

// Temperatures
bool Controller::isTempValid(const string& name)
{
	ITempProvider *p;
	EthernetDevice *dev;
	int num;
	TTempProviderValue& info = findValue(name, m_temps);
	if (!findProvider(info.id, dev, p, num))
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
	TTempProviderValue& info = findValue(name, m_temps);
	if (!findProvider(info.id, dev, p, num))
	{
		m_logger->logWarning(str(Format("Unable to find temperature provider for {}") << num));
		return 0;
	}
	return p->getTemp(num);
}

template<typename T>
bool Controller::findProvider(const string& id, EthernetDevice*& dev, T*& provider, int& num)
{
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
	TInputProviderValue& info = findValue(id, m_inputs);
	
	m_logger->logInfo(str(Format("Input {} has got new state {}") << info.name << state));
	
	publish(str(Format("INPUT:INIT-STATE:{}:{}:{}") << info.id << info.name << state));
	
	info.setValue(state);
}
void Controller::onInputChanged(IInputProvider* provider, const string& id, int state)
{
	TInputProviderValue& info = findValue(id, m_inputs);
	
	if (info.value != state)
	{
		if (state)
			m_logger->logInfo(str(Format("Input {} changed 0->1") << info.name));
		else
			m_logger->logInfo(str(Format("Input {} changed 1->0") << info.name));
			
		publish(str(Format("INPUT:CHANGED:{}:{}:{}") << info.id << info.name << state));
	}
	info.setValue(state);
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
	
	for (TOutputProviderValue& info : m_outputs)
	{
		if (info.initialized)
		{
			m_logger->logInfo(str(Format("Restoring output {} state to {}") << info.id << info.value));
			setOutputInProvider(info.id, info.value);
		}
	}
}

// ICounterProviderListener
void Controller::onCounterChanged(ICounterProvider* provider, const string& id, uint32_t value)
{
	TCounterProviderValue& info = findValue(id, m_counters);
	
	m_logger->logInfo(str(Format("Counter {} changed to {}") << info.name << value));
	
	publish(str(Format("COUNTER:CHANGED:{}:{}:{}") << info.id << info.name << value));
}

void Controller::setOutputInProvider(const string& id, bool state)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	int num;
	
	if (!findProvider(id, dev, p, num))
	{
		m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
		return;
	}
	
	p->setOutputState(num, state);
}
