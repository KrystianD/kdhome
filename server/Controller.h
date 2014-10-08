#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "UdpServer.h"
#include "3rd/mongoose.h"
#include <kutils.h>

#include <stdint.h>
#include <string>
using namespace std;

#include "Providers.h"
#include "EthernetDevice.h"
#include "providers/IOutputProvider.h"
#include "providers/EthernetOutputProvider.h"
#include "providers/EthernetInputProvider.h"
#include "providers/EthernetIRProvider.h"
#include "providers/EthernetTempProvider.h"

#include <zmq.h>

class MyLogger;
class lua_State;
class StorageEngine;

class TDelayedCode
{
public:
	std::string id, code;
	bool repeating;
	uint32_t executionTime;
	float timeout;
};
class TActivityTimer
{
public:
	TActivityTimer();
	
	uint32_t getIdleTime() const;
	void setAsIdle();
	void reset();
	
private:
	uint32_t m_lastTime, m_resetValidTime;
};
template<typename T>
struct TProviderEntry
{
	T *provider;
	int startNumber;
};

class Controller : public IInputProviderListener, public IEthernetDataListener, public IIRProviderListener
{
public:
	Controller();
	~Controller();
	
	void setLogger(MyLogger* logger)
	{
		m_logger = logger;
	}
	void init();
	void execute();
	void savePersistentState(int outputId);
	
	void updateNames();
	
	template<typename T>
	bool findProvider(int num, EthernetDevice*& dev, T*& provider);
	
	// Expander
	// void freezeExpander() { m_expander->freeze(); }
	// void unfreezeExpander() { m_expander->unfreeze(); }
	
	int getTime()
	{
		return getTicks() - m_startTime;
	}
	
	// Devices
	int registerEthernetDevice(uint32_t id, const std::string& ip);
	void addOutputProvider(int dev, int outputsCount);
	void addInputProvider(int dev, int inputsCount);
	void addIRProvider(int dev);
	void addTempProvider(int dev, int sensorsCount);
	
	// Inputs
	bool getInput(int num);
	
	// Outputs
	void setOutput(int num, bool on);
	bool getOutput(int num);
	void toggleOutput(int num);
	void setOutputAsPersistent(int num);
	
	// Temperature
	bool isTempValid(int num);
	float getTemp(int num);
	
	// Script commands
	void setTimeout(const std::string& id, float timeout, const std::string& code)
	{
		setInterval(id, timeout, code, false);
	}
	void setInterval(const std::string& id, float timeout, const std::string& code)
	{
		setInterval(id, timeout, code, true);
	}
	void setInterval(const std::string& id, float timeout, const std::string& code, bool repeating);
	void removeTimeout(const std::string& id)
	{
		removeInterval(id);
	}
	void removeInterval(const std::string& id);
	bool hasTimeout(const std::string& id)
	{
		return hasInterval(id);
	}
	bool hasInterval(const std::string& id);
	
	// Handlers
	bool onExternalCommand(const std::vector<std::string>& parts, std::string& res);
	void onIRCode(uint32_t code);
	void onIRButtonPressed(uint32_t code);
	void onIRButtonReleased(uint32_t code);
	
	void execLuaFunc(int num);
	
	lua_State* getLua()
	{
		return m_lua;
	}
	StorageEngine* getStorage()
	{
		return m_storage;
	}
	
	// mongoose
	mg_server* getHttpServer()
	{
		return m_httpserver;
	}
	mg_connection* getHttpConnection()
	{
		return m_currConn;
	}
	int processHttpRequest(mg_connection* conn);
	
	// IEthernetDataListener
	void onEthernetDataReceived(const string& ip, ByteBuffer& buffer);
	
	// IInputProviderListener
	void onInputChanged(IInputProvider* provider, int num, int state);
	
	// IIRProviderListener
	void onIRCodeReceived(IIRProvider* provider, uint32_t code);
	void onIRButtonPressed(IIRProvider* provider, uint32_t code);
	void onIRButtonReleased(IIRProvider* provider, uint32_t code);
	
private:
	uint32_t m_lastTicks;
	uint32_t m_startTime;
	
	UdpServer m_server;
	mg_server *m_httpserver;
	mg_connection *m_currConn;
	// TActivityTimer m_userActivity, m_movementActivity, m_totalActivity;
	MyLogger *m_logger;
	StorageEngine *m_storage;
	
	vector<EthernetDevice*> m_devices;
	
	map<int, int> m_persistentOutputs;
	
	// Lua
	lua_State *m_lua;
	pthread_mutex_t m_luaMutex;
	pthread_cond_t m_luaCondWait;
	pthread_t m_luaThreadId;
	int m_luaThreads;
	string m_luaProtectionStr;
	
	std::vector<TDelayedCode> m_delayedCode;
	
	// zmq
	void *zcontext, *zsocket, *zsocketREP;
	bool m_initialized;
	
	void publish(const string& msg);
	
	std::string getLuaError();
	std::string getLuaErrorNOPROTECT();
	void _protectLua(const char* file, int line);
	void _unprotectLua(const char* file, int line);
	void throwLuaErrorNOPROTECT(const string& msg);
	
	std::string getInputName(int num);
	std::string getOutputName(int num);
};

// class InputStatusChangedEvent : public UsbEvent
// {
// public:
// InputStatusChangedEvent(int num, bool newState) : m_num(num), m_newState(newState) { }

// int getType() const { return EVENT_PROGRAM_INPUTSTATUSCHANGED; }

// // int getStatus() const { return m_status; }
// int getNum() const { return m_num; }
// bool getNewState() const { return m_newState; }

// std::string toString() const { return boost::str(boost::format("Input status changed(input: %1%, %2%->%3%)") % getNum() % !getNewState() % getNewState()); }

// private:
// int m_status, m_num;
// bool m_newState;
// };

#endif
