#include "pch.h"
#include "Controller.h"
#include "Program.h"
// #include "events.h"
#include "MyLogger.h"
#include "kutils.h"

#include "lua_funcs.h"
#include "lua_funcs_gen.h"

#include "3rd/mongoose.h"

#include "ICURL.h"

//#define protectLua()  printf("want protect %s:%d\n", __FILE__, __LINE__); _protectLua(); printf("protect %s:%d\n", __FILE__, __LINE__);
//#define unprotectLua()  _unprotectLua(); printf("unprotect %s:%d\n", __FILE__, __LINE__); 
#define protectLua()  _protectLua(__FILE__, __LINE__); 
#define unprotectLua()  _unprotectLua(__FILE__, __LINE__);

using namespace std;
using namespace fmt;

static int handle_hello(struct mg_connection *conn)
{
	Controller *ctrl = (Controller*)conn->server_param;
	return ctrl->processHttpRequest(conn);
}

// Controller
Controller::Controller()
 : m_lua(0), m_luaThreads(0)
{
	pthread_mutex_init(&m_luaMutex, 0);
	pthread_cond_init(&m_luaCondWait, 0);

	m_httpserver = mg_create_server(this);
	mg_set_option(m_httpserver, "document_root", "");
	mg_set_option(m_httpserver, "hide_files_patterns", "*");
	mg_set_option(m_httpserver, "enable_directory_listing", "no");
	mg_set_option(m_httpserver, "listening_port", "8080");
	mg_add_uri_handler(m_httpserver, "/", &handle_hello);

	m_startTime = getTicks();
}
Controller::~Controller()
{
	mg_destroy_server(&m_httpserver);
	pthread_mutex_destroy(&m_luaMutex);
}

void Controller::init()
{
	protectLua();
	if (m_lua)
		lua_close(m_lua);
	m_lua = lua_open();
	luaL_openlibs(m_lua);
	luabind::open(m_lua);

	tolua_lua_funcs_open(m_lua);

	int res = luaL_dofile(m_lua, "scripts/script1.lua");
	unprotectLua();
	if (res)
	{
		m_logger->logWarning(Format("Script error: {}") << getLuaError());
		protectLua();
		lua_close(m_lua);
		m_lua = 0;
		unprotectLua();
	}
	
	m_delayedCode.clear();

	if (m_lua)
	{
		protectLua();
		try
		{
			if (m_lua) luabind::call_function<void>(m_lua, "onInit");
		}
		catch (luabind::error& e)
		{
			m_logger->logWarning(Format("[script] [onInit] {}") << getLuaErrorNOPROTECT());
		}
		unprotectLua();
	}

	m_server.setListener(this);
	m_server.init();
}

void Controller::execute()
{
	int res;

	m_server.process();
	mg_poll_server(m_httpserver, 0);

	uint32_t ticks = getTicks();
	for (int i = m_delayedCode.size() - 1; i >= 0; i--)
	{
		TDelayedCode &dcode = m_delayedCode[i];
		if (dcode.executionTime <= ticks)
		{
			if (dcode.id.find("nolog") == string::npos)
				m_logger->logInfo(Format("[script] Executing delayed code for id '{}'") << dcode.id);
			string code = dcode.code;
			if (!dcode.repeating)
				m_delayedCode.erase(m_delayedCode.begin() + i);
			else
				dcode.executionTime = getTicks() + dcode.timeout;
			protectLua();
			if (m_lua)
			{
				res = luaL_dostring(m_lua, code.c_str());
				if (res)
					m_logger->logWarning(Format("[script] {}") << getLuaErrorNOPROTECT());
			}
			unprotectLua();
		}
	}

	uint32_t diff = ticks - m_lastTicks;
	m_lastTicks = ticks;
	
	for (size_t i = 0; i < m_devices.size(); i++)
	{
		EthernetDevice *dev = m_devices[i];
		dev->process();
	}
}

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
		throwLuaErrorNOPROTECT(str(Format("Bad device index: {}") << dev));
		lua_error(m_lua);
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetOutputProvider *out = new EthernetOutputProvider(device, outputsCount);
	device->addProvider(out);
	m_logger->logInfo(Format("Added output provider for device #{}") << dev);
}
void Controller::addInputProvider(int dev, int inputsCount)
{
	if (dev >= m_devices.size())
	{
		throwLuaErrorNOPROTECT(str(Format("Bad device index: {}") << dev));
		lua_error(m_lua);
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetInputProvider *inp = new EthernetInputProvider(device, inputsCount);
	inp->setListener(this);
	device->addProvider(inp);
	m_logger->logInfo(Format("Added input provider for device #{}") << dev);
}
void Controller::addIRProvider(int dev)
{
	if (dev >= m_devices.size())
	{
		throwLuaErrorNOPROTECT(str(Format("Bad device index: {}") << dev));
		lua_error(m_lua);
		return;
	}
	EthernetDevice *device = m_devices[dev];
	EthernetIRProvider *ir = new EthernetIRProvider(device);
	ir->setListener(this);
	device->addProvider(ir);
	m_logger->logInfo(Format("Added IR provider for device #{}") << dev);
}

bool Controller::getInput(int num)
{
	IInputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		throwLuaErrorNOPROTECT(str(Format("Unable to find input provider for {}") << num));
		return false;
	}
	return p->getInputState(num - dev->getID());
}

void Controller::setOutput(int num, bool on)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		throwLuaErrorNOPROTECT(str(Format("Unable to find output provider for {}") << num));
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
}
bool Controller::getOutput(int num)
{
	IOutputProvider *p;
	EthernetDevice *dev;
	if (!findProvider(num, dev, p))
	{
		throwLuaErrorNOPROTECT(str(Format("Unable to find output provider for {}") << num));
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
		throwLuaErrorNOPROTECT(str(Format("Unable to find output provider for {}") << num));
		return;
	}
	p->toggleOutputState(num - dev->getID());

	if (p->getOutputState(num - dev->getID()))
		m_logger->logInfo(Format("[expander] Output {} enabled(toggled)") << getOutputName(num));
	else
		m_logger->logInfo(Format("[expander] Output {} disabled(toggled)") << getOutputName(num));
}

// Temperature
// double Controller::getTemperature(int num)
// {
	// const TProviderEntry<ITemperatureProvider> *p = findTemperatureProvider(num);
	// if (p)
	// {
		// return p->provider->getTemperature(num - p->startNumber);
	// }
	// return 0;
// }

// Script commands
void Controller::setInterval(const std::string& id, float timeout, const std::string& code, bool repeating)
{
	for (size_t i = 0; i < m_delayedCode.size(); i++)
	{
		if (m_delayedCode[i].id == id)
		{
			m_delayedCode[i].executionTime = getTicks() + timeout;
			m_delayedCode[i].repeating = repeating;
			m_delayedCode[i].timeout = timeout;
			m_delayedCode[i].code = code;
			m_logger->logInfo(Format("[script] Replacing current delayed code for id '{}'") << id);
			return;
		}
	}
	TDelayedCode dcode;
	dcode.id = id;
	dcode.executionTime = getTicks() + timeout;
	dcode.repeating = repeating;
	dcode.timeout = timeout;
	dcode.code = code;
	m_delayedCode.push_back(dcode);
	m_logger->logInfo(Format("[script] New delayed code for id '{}' exec at {}") << id << dcode.executionTime);
}
void Controller::removeInterval(const std::string& id)
{
	for (size_t i = 0; i < m_delayedCode.size(); i++)
	{
		if (m_delayedCode[i].id == id)
		{
			m_delayedCode.erase(m_delayedCode.begin() + i);
			m_logger->logInfo(Format("[script] Removed delayed code for id '{}'") << id);
			return;
		}
	}
}
bool Controller::hasInterval(const std::string& id)
{
	for (size_t i = 0; i < m_delayedCode.size(); i++)
		if (m_delayedCode[i].id == id)
			return true;
	return false;
}

// Handlers
bool Controller::onExternalCommand(const std::vector<std::string>& parts, std::string& res)
{
	if (parts.size() < 1)
		return false;
	string cmd = parts[0];
	if (cmd == "reload")
	{
		// program->ungrabGlobals();
		init();
		m_logger->logInfo("[script] Reloaded");
		return false;
	}

	string argsStr = joinStrings(" ", parts, 1);
	// cout << "argsStr " << argsStr << endl;
	
	vector<string> args = parseArgs(argsStr, -1);

	if (m_lua)
	{
		protectLua();
		lua_getfield(m_lua, LUA_GLOBALSINDEX, "onExternalCommand");
		lua_pushstring(m_lua, cmd.c_str());
		lua_newtable(m_lua);
		for (int i = 0; i < args.size(); i++)
		{
			// cout << "arg " << i << endl;
			lua_pushnumber(m_lua, i + 1);
			lua_pushstring(m_lua, args[i].c_str());
			lua_settable(m_lua, -3);
		}
		int cres = lua_pcall(m_lua, 2, 1, 0);
		if (cres == 0)
		{
			// cout << lua_type(m_lua, -1);
			if (lua_type(m_lua, -1) == LUA_TSTRING)
			{
				string retStr = lua_tostring(m_lua, -1);
				if (retStr != "")
				{
					res = retStr;
					unprotectLua();
					return true;
				}
			}
			else if (lua_type(m_lua, -1) != LUA_TNIL)
			{
				m_logger->logWarning("[script] Invalid return type");
			}
			lua_pop(m_lua, 1);
			unprotectLua();
		}
		else
		{
			unprotectLua();
			m_logger->logWarning(Format("[script] {}") << getLuaError());
			return false;
		}
	}

	return false;
}

void Controller::execLuaFunc(int num)
{
	protectLua();
	lua_rawgeti(m_lua, LUA_REGISTRYINDEX, num);
	int ret = lua_pcall(m_lua, 0, 0, 0);
	if (ret != 0)
		m_logger->logWarning(Format("[script] [execLuaFunc] (ret: {}) {}") << ret << getLuaErrorNOPROTECT());
	unprotectLua();
}

std::string Controller::getInputName(int num)
{
	protectLua();
	try
	{
		if (m_lua)
		{
			string ret = luabind::call_function<string>(m_lua, "getInputName", num);
			unprotectLua();
			return ret;
		}
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [getInputName] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
	return str(Format("{}") << num);
}
std::string Controller::getOutputName(int num)
{
	protectLua();
	try
	{
		if (m_lua)
		{
			string ret = luabind::call_function<string>(m_lua, "getOutputName", num);
			unprotectLua();
			return ret;
		}
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [getOutputName] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
	return str(Format("{}") << num);
}

string Controller::getLuaError()
{
	protectLua();
	string luaErr = lua_tostring(m_lua, -1);
	unprotectLua();
	return luaErr;
}
string Controller::getLuaErrorNOPROTECT()
{
	string luaErr = lua_tostring(m_lua, -1);
	return luaErr;
}

// #define LUALOCK_LOGS
void Controller::_protectLua(const char* file, int line)
{
#ifdef LUALOCK_LOGS
	printf("[%s:%d] protect lock\n", file, line);
#endif
	pthread_mutex_lock(&m_luaMutex);
	while (m_luaThreads > 0 && m_luaThreadId != pthread_self())
	{
#ifdef LUALOCK_LOGS
		printf("[%s:%d] protect cond wait\n", file, line);
#endif
		pthread_cond_wait(&m_luaCondWait, &m_luaMutex);
#ifdef LUALOCK_LOGS
		printf("[%s:%d] protect after cond wait\n", file, line);
#endif
	}
#ifdef LUALOCK_LOGS
	printf("[%s:%d] protect GO\n", file, line);
#endif
	m_luaThreads++;
	m_luaThreadId = pthread_self();
	pthread_mutex_unlock(&m_luaMutex);
#ifdef LUALOCK_LOGS
	printf("[%s:%d] protect HAVE lua %d\n", file, line, m_luaThreads);
#endif
}
void Controller::_unprotectLua(const char* file, int line)
{
#ifdef LUALOCK_LOGS
	printf("[%s:%d] unprotect lock\n", file, line);
#endif
	pthread_mutex_lock(&m_luaMutex);
	m_luaThreads--;
#ifdef LUALOCK_LOGS
	printf("[%s:%d] unprotect signal\n", file, line);
#endif
	pthread_cond_signal(&m_luaCondWait);
#ifdef LUALOCK_LOGS
	printf("[%s:%d] unprotect unlock %d\n", file, line, m_luaThreads);
#endif
	pthread_mutex_unlock(&m_luaMutex);
}
void Controller::throwLuaErrorNOPROTECT(const string& msg)
{
	lua_Debug d;
	lua_getstack(m_lua, 1, &d);
	lua_getinfo(m_lua, "Sln", &d);
	// std::string err = lua_tostring(m_lua, -1);
	lua_pop(m_lua, 1);

	lua_pushfstring(m_lua, "%s:%d %s", d.short_src, d.currentline, msg.c_str());
	lua_error(m_lua);
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
	protectLua();
	try
	{
		m_currConn = conn;
		if (m_lua) luabind::call_function<void>(m_lua, "onHttpRequest", (string)conn->uri);
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [onHttpRequest] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
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
	protectLua();
	try
	{
		if (m_lua) luabind::call_function<void>(m_lua, "onInputStateChanged", provider->getDevice()->getID() + num, state);
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [onInputChanged] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
}

// IIRProviderListener
void Controller::onIRCodeReceived(IIRProvider* provider, uint32_t code)
{
	protectLua();
	try
	{
		if (m_lua) luabind::call_function<void>(m_lua, "onIRCodeReceived", code);
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [onIRCodeReceived] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
}
void Controller::onIRButtonPressed(IIRProvider* provider, uint32_t code)
{
	protectLua();
	try
	{
		if (m_lua) luabind::call_function<void>(m_lua, "onIRButtonPressed", code);
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [onIRButtonPressed] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
}
void Controller::onIRButtonReleased(IIRProvider* provider, uint32_t code)
{
	protectLua();
	try
	{
		if (m_lua) luabind::call_function<void>(m_lua, "onIRButtonReleased", code);
	}
	catch (luabind::error& e)
	{
		m_logger->logWarning(Format("[script] [onIRButtonReleased] {}") << getLuaErrorNOPROTECT());
	}
	unprotectLua();
}
