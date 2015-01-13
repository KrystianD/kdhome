#ifndef __COUNTER_PROVIDER_H__
#define __COUNTER_PROVIDER_H__

#include "../EthernetDevice.h"
#include "ICounterProvider.h"

class CounterProvider : public ICounterProvider
{
public:
	CounterProvider(EthernetDevice* device, int amount);
	virtual ~CounterProvider()
	{
		logger->logInfo("Deleting CounterProvider");
	}
	
	// IProvider
	void init();
	void deinit() { }
	void processData(const void* buffer, int len);
	void process();
	EthernetDevice* getDevice()
	{
		return m_device;
	}
	
	// ICounterProvider
	int getAmount()
	{
		return m_counters.size();
	}
	uint32_t getCounterValue(int num)
	{
		return m_counters[num];
	}
	void setListener(ICounterProviderListener* listener)
	{
		m_listener = listener;
	}

	string getInputID(int num);
	
private:
	EthernetDevice *m_device;
	ICounterProviderListener *m_listener;
	
	bool m_hasFirstData;
	vector<uint32_t> m_counters;
	
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
