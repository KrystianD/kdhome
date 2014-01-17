#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

// #include <UsbEvent.h>
// #include "LedUsbDevice.h"
// #include "SensorUsbDevice.h"
// #include "ExpanderUsbDevice.h"
// #include "Color.h"
#include "UdpServer.h"
#include "IEvent.h"
#include "3rd/mongoose.h"

#include <stdint.h>
#include <string>
using namespace std;

#include "Providers.h"
#include "EthernetDevice.h"
#include "providers/IOutputProvider.h"
#include "providers/EthernetOutputProvider.h"
#include "providers/EthernetInputProvider.h"
#include "providers/EthernetIRProvider.h"

class MyLogger;
class lua_State;

// #include "stdafx.h"


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
// struct TBlinkEntry
// {
	// ColorF color1, color2;
	// int duration;

	// TBlinkEntry() { }
	// TBlinkEntry(const ColorF& c1, const ColorF& c2, int duration) : color1(c1), color2(c2), duration(duration) { }
// };
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

	// LedUsbDevice *m_led;
	// SensorUsbDevice *m_sensor;
	// ExpanderUsbDevice *m_expander;

	void setLogger(MyLogger* logger) { m_logger = logger; }
	void init();
	void execute();

	void updateNames();

	// Providers
	void checkInputs();
	void enableInputsChecking() { m_inputCheckingEnabled = true; }

	// bool findOutputProvider(int num, EthernetDevice*& dev, IOutputProvider*& provider);
	template<typename T>
	bool findProvider(int num, EthernetDevice*& dev, T*& provider);

	// Activity
	// TActivityTimer& getUserActivity() { return m_userActivity; }
	// TActivityTimer& getMovementActivity() { return m_movementActivity; }
	// TActivityTimer& getTotalActivity() { return m_totalActivity; }

	// Expander
	// void freezeExpander() { m_expander->freeze(); }
	// void unfreezeExpander() { m_expander->unfreeze(); }

	// Devices
	int registerEthernetDevice(uint32_t id, const std::string& ip);
	void addOutputProvider(int dev, int outputsCount);
	void addInputProvider(int dev, int inputsCount);
	void addIRProvider(int dev);

	// Inputs
	bool getInput(int num);

	// Outputs
	void setOutput(int num, bool on);
	bool getOutput(int num);
	void toggleOutput(int num);

	// Sensor
	// int getLightValue() { return m_sensor->getLightValue(); }
	// uint8_t getFlags() { return m_sensor->getFlags(); }
	// void setSensitivity(int sens) { m_sensor->setSensitivity(sens); }

	// Temperature
	// double getTemperature(int num);

	// Script commands
	void setStateCheckTimeout(float timeout);
	void setTimeout(const std::string& id, float timeout, const std::string& code) { setInterval(id, timeout, code, false); }
	void setInterval(const std::string& id, float timeout, const std::string& code) { setInterval(id, timeout, code, true); }
	void setInterval(const std::string& id, float timeout, const std::string& code, bool repeating);
	void removeTimeout(const std::string& id) { removeInterval(id); }
	void removeInterval(const std::string& id);
	bool hasTimeout(const std::string& id) { return hasInterval(id); }
	bool hasInterval(const std::string& id);

	// Handlers
	bool onExternalCommand(const std::vector<std::string>& parts, std::string& res);
	void onEvent(IEvent* event);
	void onIRCode(uint32_t code);
	void onIRButtonPressed(uint32_t code);
	void onIRButtonReleased(uint32_t code);

	void execLuaFunc(int num);

	lua_State* getLua() { return m_lua; }
	mg_server* getHttpServer() { return m_httpserver; }

	// mongoose
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

	UdpServer m_server;
	mg_server *m_httpserver;
	// TActivityTimer m_userActivity, m_movementActivity, m_totalActivity;
	MyLogger *m_logger;

	// Providers
	bool m_inputCheckingEnabled;
	uint32_t m_inputStates;

	vector<EthernetDevice*> m_devices;

	// Keyboard
	std::map<int,bool> m_keys;

	// Lua
	lua_State *m_lua;
	pthread_mutex_t m_luaMutex;
	pthread_cond_t m_luaCondWait;
	pthread_t m_luaThreadId;
	int m_luaThreads;
	string m_luaProtectionStr;

	std::vector<TDelayedCode> m_delayedCode;
	int m_stateNextCheckTime, m_stateCheckTimeout;

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
