#ifndef __ETHERNET_IR_PROVIDER_H__
#define __ETHERNET_IR_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IIRProvider.h"

#include <map>

using namespace std;

class EthernetIRProvider : public IIRProvider
{
public:
	EthernetIRProvider(EthernetDevice* device);
	virtual ~EthernetIRProvider()
	{
		logger->logInfo("Deleting IRProvider");
	}
	
	// IProvider
	void init() { }
	void deinit() { }
	void processData(ByteBuffer& buffer);
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
	uint32_t m_lastCode;
	
	void update();
	void prepareCommand(ByteBuffer& buffer, uint8_t command);
	void sendData(ByteBuffer& buffer)
	{
		m_device->sendData(buffer);
	}
};

#endif
