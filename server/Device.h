#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Providers.h"
// #include "UdpServer.h"

class Device
{
public:
	Device(uint32_t m_id);
	virtual ~Device() { }

	void addProvider(IProvider* provider) { m_providers.push_back(provider); }
	bool hasProvider(uint16_t type);
	IProvider* getProvider(uint16_t type);

	uint32_t getID () const { return m_id; }

	void processData(ByteBuffer& buffer);
	virtual void process();
	void checkConnection();

	void prepareBuffer(ByteBuffer& buffer);
	virtual void sendData(ByteBuffer& data) = 0;

protected:
	uint32_t m_id;
	uint32_t m_lastPacketTime, m_registrationDataSendTime;
	bool m_connected;

	uint16_t m_lastRecvPacketId, m_sendPacketId;
	uint16_t m_sessKey;

	vector<IProvider*> m_providers;

	void markDisconnected();
	void markConnected();
	void registerDevice();

	virtual void logInfo(const string& msg);
};

#endif
