#ifndef __ETHERNET_DEVICE_H__
#define __ETHERNET_DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Providers.h"
#include "UdpServer.h"

class EthernetDevice
{
public:
	EthernetDevice(UdpServer* server, uint32_t m_id, const string& ip);
	~EthernetDevice() { }

	void addProvider(IProvider* provider) { m_providers.push_back(provider); }
	bool hasProvider(uint16_t type);
	IProvider* getProvider(uint16_t type);

	const string& getIP () const { return m_ip; }
	uint32_t getID () const { return m_id; }

	void processData(ByteBuffer& buffer);
	void process();
	void checkConnection();

	void prepareBuffer(ByteBuffer& buffer);
	void sendData(ByteBuffer& data);

private:
	string m_ip;
	uint32_t m_id;
	UdpServer *m_server;
	uint32_t m_lastPacketTime, m_registrationDataSendTime;
	bool m_connected;

	uint16_t m_lastRecvPacketId, m_sendPacketId;
	uint16_t m_sessKey;

	vector<IProvider*> m_providers;

	void markDisconnected();
	void markConnected();
	void registerDevice();

	void logInfo(const string& msg);
};

#endif
