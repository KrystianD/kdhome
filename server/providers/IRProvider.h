#ifndef __ETHERNET_IR_PROVIDER_H__
#define __ETHERNET_IR_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IIRProvider.h"

#include <map>

using namespace std;

class IRProvider : public IIRProvider
{
public:
	IRProvider(EthernetDevice* device);
	virtual ~IRProvider()
	{
		logger->logInfo("Deleting IRProvider");
	}
	
	// IProvider
	void init() { }
	void deinit() { }
	void processData(const void* buffer, int len);
	void process();
	EthernetDevice* getDevice()
	{
		return m_device;
	}
	
	// IIRProvider
	void setListener(IIRProviderListener* listener)
	{
		m_listener = listener;
	}
	
private:
	EthernetDevice *m_device;
	IIRProviderListener *m_listener;
	
	map<uint32_t, uint32_t> m_codes;
	uint32_t m_lastCode, m_lastCodeTime;
	
	void update();
	// void prepareCommand(ByteBuffer& buffer, uint8_t command);
	// void sendData(ByteBuffer& buffer)
	// {
		// m_device->sendData(buffer);
	// }
};

#endif
