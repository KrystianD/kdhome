#include "pch.h"
#include "Controller.h"
#include "Program.h"
#include "MyLogger.h"
#include "StorageEngine.h"
#include "kutils.h"

#include <zmq.h>
#include <unistd.h>

#include "3rd/mongoose.h"

#include "ICURL.h"

using namespace std;
using namespace fmt;

static int handle_hello(struct mg_connection *conn)
{
	Controller *ctrl = (Controller*)conn->server_param;
	return ctrl->processHttpRequest(conn);
}

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
		
	delete m_storage;
	mg_destroy_server(&m_httpserver);
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
	
	m_httpserver = mg_create_server(this);
	mg_set_option(m_httpserver, "document_root", "");
	mg_set_option(m_httpserver, "hide_files_patterns", "*");
	mg_set_option(m_httpserver, "enable_directory_listing", "no");
	mg_set_option(m_httpserver, "listening_port", "8080");
	mg_add_uri_handler(m_httpserver, "/", &handle_hello);
	
	m_logger->logInfo("Creating UDP server");
	m_server.setListener(this);
	if (!m_server.init())
	{
		m_logger->logError(str(Format("Unable to configure UDP server: {}") << m_server.getLastError()));
		return false;
	}

	reload();

	return true;
}
void Controller::reload()
{
	m_sessKey = rand() % (99999 - 10000) + 10000;
	
	m_logger->logInfo("Loading persistent storage");
	m_storage->setPath("data");
	m_storage->load();
	
	for (auto& it : m_persistentOutputs)
	{
		int outputId = it.first;
		int state = m_storage->getInt("output", outputId, 1);
		// m_logger->logInfo(Format("out {} {}") << outputId << state);
		setOutput(outputId, state);
	}
	
	m_initialized = false;
	
	m_inputsNames.clear();
	m_outputsNames.clear();
	
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
			if (p.size() < 6)
				return "";
				
			int id = atoi(p[2].c_str());
			string ip = p[3];
			int port = atoi(p[4].c_str());
			
			int dev = registerEthernetDevice(id, ip);
			return str(Format("{}") << dev);
		}
		else if (cmd == "ADD-INPUT-PROVIDER")
		{
			if (p.size() < 5)
				return "";
				
			int id = atoi(p[2].c_str());
			int cnt = atoi(p[3].c_str());
			
			addInputProvider(id, cnt);
		}
		else if (cmd == "ADD-OUTPUT-PROVIDER")
		{
			if (p.size() < 5)
				return "";
				
			int id = atoi(p[2].c_str());
			int cnt = atoi(p[3].c_str());
			
			addOutputProvider(id, cnt);
		}
		else if (cmd == "ADD-IR-PROVIDER")
		{
			if (p.size() < 4)
				return "";
				
			int id = atoi(p[2].c_str());
			
			addIRProvider(id);
		}
		else if (cmd == "ADD-TEMP-PROVIDER")
		{
			if (p.size() < 5)
				return "";
				
			int id = atoi(p[2].c_str());
			int cnt = atoi(p[3].c_str());
			
			addTempProvider(id, cnt);
		}
	}
	else if (type == "OUTPUT")
	{
		string cmd = p[1];
		if (cmd == "SET")
		{
			if (p.size() < 5)
				return "";
				
			int num = atoi(p[2].c_str());
			int value = atoi(p[3].c_str());
			
			setOutput(num, value);
			return "";
		}
		else if (cmd == "TOGGLE")
		{
			if (p.size() < 4)
				return "";
				
			int num = atoi(p[2].c_str());
			
			toggleOutput(num);
			return "";
		}
		else if (cmd == "GET")
		{
			if (p.size() < 4)
				return "";
				
			int num = atoi(p[2].c_str());
			
			int val = getOutput(num);
			return str(Format("{}") << val);
		}
		else if (cmd == "SET-NAME")
		{
			if (p.size() < 4)
				return "";
				
			int num = atoi(p[2].c_str());
			string name = p[3];
			
			m_outputsNames[num] = name;
			
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
				
			int num = atoi(p[2].c_str());
			
			int val = getInput(num);
			return str(Format("{}") << val);
		}
		else if (cmd == "SET-NAME")
		{
			if (p.size() < 4)
				return "";
				
			int num = atoi(p[2].c_str());
			string name = p[3];
			
			m_inputsNames[num] = name;
			
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
				
			int num = atoi(p[2].c_str());
			
			float val = getTemp(num);
			return str(Format("{}") << val);
		}
		else if (cmd == "IS-VALID")
		{
			if (p.size() < 4)
				return "";
				
			int num = atoi(p[2].c_str());
			
			int val = isTempValid(num);
			return str(Format("{}") << val);
		}
	}
	return "";
}

void Controller::execute()
{
	int res;
	
	m_server.process();
	// mg_poll_server(m_httpserver, 0);
	
	// uint32_t ticks = getTicks();
	// for (int i = m_delayedCode.size() - 1; i >= 0; i--)
	// {
		// TDelayedCode &dcode = m_delayedCode[i];
		// if (dcode.executionTime <= ticks)
		// {
			// if (dcode.id.find("nolog") == string::npos)
				// m_logger->logInfo(Format("[script] Executing delayed code for id '{}'") << dcode.id);
			// string code = dcode.code;
			// if (!dcode.repeating)
				// m_delayedCode.erase(m_delayedCode.begin() + i);
			// else
				// dcode.executionTime = getTicks() + dcode.timeout;
		// }
	// }
	
	uint32_t diff = ticks - m_lastTicks;
	m_lastTicks = ticks;
	
	for (size_t i = 0; i < m_devices.size(); i++)
	{
		EthernetDevice *dev = m_devices[i];
		dev->process();
	}
	
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
void Controller::savePersistentState(int outputId)
{
	map<int, int>::iterator it;
	// for (it = m_persistentOutputs.begin(); it != m_persistentOutputs.end(); it++)
	{
		// int outputId = it->first;
		bool state = getOutput(outputId);
		m_storage->setInt("output", outputId, state);
	}
	m_storage->save();
}

// Registering devices and proviers
int Controller::registerEthernetDevice(uint32_t id, const std::string& ip)
{
	EthernetDevice *dev = new EthernetDevice(&m_server, id, ip);
	int idx = m_devices.size();
	m_devices.push_back(dev);
	m_logger->logInfo(Format("Registered ethernet device #{} with IP {}") << idx << ip);
	return idx;
}
void Controller::addOutputProvider(int dev, int outputsCount)
{
	if (dev >= m_devices.size())
	{
		m_logger->logWarning(str(Format("Bad device index: {}") << dev));
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetOutputProvider *out = new EthernetOutputProvider(device, outputsCount);
	// out->getStorage()->setPath(str(Format("data/output_{}") << dev));
	// out->getStorage()->load();
	device->addProvider(out);
	m_logger->logInfo(Format("Added output provider to device #{}") << dev);
}
void Controller::addInputProvider(int dev, int inputsCount)
{
	if (dev >= m_devices.size())
	{
		m_logger->logWarning(str(Format("Bad device index: {}") << dev));
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetInputProvider *inp = new EthernetInputProvider(device, inputsCount);
	// inp->getStorage()->setPath(str(Format("data/input_{}") << dev));
	// inp->getStorage()->load();
	inp->setListener(this);
	device->addProvider(inp);
	m_logger->logInfo(Format("Added input provider to device #{}") << dev);
}
void Controller::addIRProvider(int dev)
{
	if (dev >= m_devices.size())
	{
		m_logger->logWarning(str(Format("Bad device index: {}") << dev));
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetIRProvider *ir = new EthernetIRProvider(device);
	// ir->getStorage()->setPath(str(Format("data/ir_{}") << dev));
	// ir->getStorage()->load();
	ir->setListener(this);
	device->addProvider(ir);
	m_logger->logInfo(Format("Added IR provider to device #{}") << dev);
}
void Controller::addTempProvider(int dev, int sensorsCount)
{
	if (dev >= m_devices.size())
	{
		m_logger->logWarning(str(Format("Bad device index: {}") << dev));
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetTempProvider *temp = new EthernetTempProvider(device, sensorsCount);
	// temp->getStorage()->setPath(str(Format("data/temp_{}") << dev));
	// temp->getStorage()->load();
	device->addProvider(temp);
	m_logger->logInfo(Format("Added temperature provider to device #{}") << dev);
}

// Inputs
bool Controller::getInput(int num)
{
	IInputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find input provider for {}") << num));
		return false;
	}
	return p->getInputState(num - dev->getID());
}

// Outputs
void Controller::setOutput(int num, bool on)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
		return;
	}
	if (p->getOutputState(num - dev->getID()) != on)
	{
		p->setOutputState(num - dev->getID(), on);
		
		if (on)
			m_logger->logInfo(Format("[output] Output {} enabled") << getOutputName(num));
		else
			m_logger->logInfo(Format("[output] Output {} disabled") << getOutputName(num));
	}
	
	savePersistentState(num);
}
bool Controller::getOutput(int num)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
		return false;
	}
	return p->getOutputState(num - dev->getID());
}
void Controller::toggleOutput(int num)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find output provider for {}") << num));
		return;
	}
	p->toggleOutputState(num - dev->getID());
	
	if (p->getOutputState(num - dev->getID()))
		m_logger->logInfo(Format("[expander] Output {} enabled(toggled)") << getOutputName(num));
	else
		m_logger->logInfo(Format("[expander] Output {} disabled(toggled)") << getOutputName(num));
		
	savePersistentState(num);
}
void Controller::setOutputAsPersistent(int num)
{
	m_persistentOutputs[num] = 1;
}

// Temperatures
bool Controller::isTempValid(int num)
{
	ITempProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find temperature provider for {}") << num));
		return false;
	}
	return p->isTempValid(num - dev->getID());
}
float Controller::getTemp(int num)
{
	ITempProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		m_logger->logWarning(str(Format("Unable to find temperature provider for {}") << num));
		return 0;
	}
	return p->getTemp(num - dev->getID());
}

template<typename T>
bool Controller::findProvider(int num, EthernetDevice*& dev, T*& provider)
{
	for (int i = 0; i < m_devices.size(); i++)
	{
		dev = m_devices[i];
		if (dev->hasProvider(T::getTypeStatic()))
		{
			provider = dynamic_cast<T*>(dev->getProvider(T::getTypeStatic()));
			if (provider && num >= dev->getID() && num < dev->getID() + provider->getAmount())
				return true;
		}
	}
	return false;
}

// mongoose
int Controller::processHttpRequest(mg_connection* conn)
{
	// protectLua();
	// try
	// {
	// m_currConn = conn;
	// if (m_lua) luabind::call_function<void>(m_lua, "onHttpRequest", (string)conn->uri);
	// }
	// catch (luabind::error& e)
	// {
	// m_logger->logWarning(Format("[script] [onHttpRequest] {}") << getLuaErrorNOPROTECT());
	// }
	// unprotectLua();
	return 1;
}

// IEthernetDataListener
void Controller::onEthernetDataReceived(const string& ip, ByteBuffer& buffer)
{
	for (size_t i = 0; i < m_devices.size(); i++)
	{
		EthernetDevice *dev = m_devices[i];
		if (dev->getIP() == ip)
		{
			dev->processData(buffer);
			return;
		}
	}
}

// IInputProviderListener
void Controller::onInputChanged(IInputProvider* provider, int num, int state)
{
	if (state)
		m_logger->logInfo(str(Format("Input {} changed 0->1") << getInputName(provider->getDevice()->getID() + num)));
	else
		m_logger->logInfo(str(Format("Input {} changed 1->0") << getInputName(provider->getDevice()->getID() + num)));
		
	publish(str(Format("INPUT:CHANGED:{}:{}") << num << state));
}

// IIRProviderListener
void Controller::onIRCodeReceived(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:NEW-CODE:{}") << code));
}
void Controller::onIRButtonPressed(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:PRESSED:{}") << code));
}
void Controller::onIRButtonReleased(IIRProvider* provider, uint32_t code)
{
	publish(str(Format("IR:RELEASED:{}") << code));
}

string Controller::getInputName(int num)
{
	auto v = m_inputsNames.find(num);
	if (v != m_inputsNames.end())
		return v->second;
	return str(Format("{}") << num);
}
string Controller::getOutputName(int num)
{
	auto v = m_outputsNames.find(num);
	if (v != m_outputsNames.end())
		return v->second;
	return str(Format("{}") << num);
}

