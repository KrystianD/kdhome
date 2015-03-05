#ifndef __ETHERNET_TEMP_PROVIDER_H__
#define __ETHERNET_TEMP_PROVIDER_H__

#include "../EthernetDevice.h"
#include "ITempProvider.h"

class TempProvider : public ITempProvider
{
public:
	TempProvider(EthernetDevice* device, int amount);
	virtual ~TempProvider()
	{
		logger->logInfo("Deleting TempProvider");
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
	
	// ITempProvider
	int getAmount()
	{
		return m_sensors.size();
	}
	bool isTempValid(int num)
	{
		return m_sensors[num].error ? false : true;
	}
	float getTemp(int num)
	{
		return m_sensors[num].value;
	}
	
private:
	struct TSensor
	{
		float value;
		bool error;
		
		TSensor() : value(0), error(false) { }
	};
	
	EthernetDevice *m_device;
	
	vector<TSensor> m_sensors;
	
	uint32_t m_lastDataTime;
	
	void logInfo(const string& msg);
};

#endif
