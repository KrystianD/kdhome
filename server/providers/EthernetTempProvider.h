#ifndef __ETHERNET_TEMP_PROVIDER_H__
#define __ETHERNET_TEMP_PROVIDER_H__

#include "../Device.h"
#include "ITempProvider.h"

class EthernetTempProvider : public ITempProvider
{
public:
	EthernetTempProvider(Device* device, int amount);
	virtual ~EthernetTempProvider() { }

	// IProvider
	// uint16_t getType() { return 0x0002; }
	void init();
	void deinit() { }
	void processData(ByteBuffer& buffer);
	void process();
	Device* getDevice() { return m_device; }

	// ITempProvider
	int getAmount() { return m_sensors.size(); }
	bool isTempValid(int num) { return m_sensors[num].error ? false : true; }
	float getTemp(int num) { return m_sensors[num].value; }

private:
	struct TSensor
	{
		float value;
		bool error;

		TSensor() : value(0), error(false) { }
	};

	Device *m_device;

	vector<TSensor> m_sensors;

	uint32_t m_lastDataTime;

	void logInfo(const string& msg);
};

#endif
