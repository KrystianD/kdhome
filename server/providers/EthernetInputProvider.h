#ifndef __ETHERNET_INPUT_PROVIDER_H__
#define __ETHERNET_INPUT_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IInputProvider.h"

class EthernetInputProvider : public IInputProvider
{
public:
	EthernetInputProvider(EthernetDevice* device, int amount);
	virtual ~EthernetInputProvider()
	{
		logger->logInfo("Deleting InputProvider");
	}
	
	// IProvider
	// uint16_t getType() { return 0x0002; }
	void init();
	void deinit() { }
	void processData(const void* buffer, int len);
	void process();
	EthernetDevice* getDevice()
	{
		return m_device;
	}
	
	// IInputProvider
	int getAmount()
	{
		return m_inputs.size();
	}
	int getInputState(int num)
	{
		return m_inputs[num].state == 1;
	}
	void setListener(IInputProviderListener* listener)
	{
		m_listener = listener;
	}

	string getInputID(int num);
	
private:
	struct TInput
	{
		uint8_t low, high;
		int state;
		
		TInput() : low(0), high(0), state(0) { }
	};
	
	EthernetDevice *m_device;
	IInputProviderListener *m_listener;
	
	bool m_hasFirstData;
	vector<TInput> m_inputs;
	
	uint32_t m_lastUpdateTime;
	
	void update();
	void preparePacket(TSrvHeader* packet, uint8_t command);
	void sendData(const void* data, int len)
	{
		m_device->sendData(data, len);
	}

	void logInfo(const string& msg);
};

#endif
