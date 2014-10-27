#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "UdpServer.h"
#include <kdutils.h>
#include <LinuxTimer.h>

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
	bool init();
	void reload();
	void run();
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
	
	// Handlers
	bool onExternalCommand(const std::vector<std::string>& parts, std::string& res);
	void onIRCode(uint32_t code);
	void onIRButtonPressed(uint32_t code);
	void onIRButtonReleased(uint32_t code);
	
	void execLuaFunc(int num);
	
	StorageEngine* getStorage()
	{
		return m_storage;
	}
	
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
	
	LinuxTimer m_checkTimer;
	UdpServer m_server;
	MyLogger *m_logger;
	StorageEngine *m_storage;
	
	vector<EthernetDevice*> m_devices;

	map<int,string> m_inputsNames, m_outputsNames;
	
	map<int, int> m_persistentOutputs;
	
	// zmq
	uint32_t m_sessKey;
	void *zcontext, *zsocket, *zsocketREP;
	bool m_initialized;
	
	void publish(string msg);
	string processREQ(string msg);
	
	string getInputName(int num);
	string getOutputName(int num);
};

#endif
