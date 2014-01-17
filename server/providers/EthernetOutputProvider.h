#ifndef __ETHERNET_OUTPUT_PROVIDER_H__
#define __ETHERNET_OUTPUT_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IOutputProvider.h"

class EthernetOutputProvider : public IOutputProvider
{
public:
	EthernetOutputProvider(EthernetDevice* device, int amount);
	virtual ~EthernetOutputProvider() { }

	// IProvider
	void init() { }
	void deinit() { }
	void processData(ByteBuffer& buffer);
	void process();
	EthernetDevice* getDevice() { return m_device; }

	// IOutputProvider
	int getAmount() { return m_outputs.size(); }
	bool getOutputState(int num) { return m_outputs[num]; }
	void setOutputState(int num, bool on);
	void toggleOutputState(int num);

private:
	EthernetDevice *m_device;

	vector<char> m_outputs;

	uint32_t m_lastUpdateTime;

	void update();
	void prepareCommand(ByteBuffer& buffer, uint8_t command);
	void sendData(ByteBuffer& buffer) { m_device->sendData(buffer); }
};

#endif
