#ifndef __ETHERNET_DEVICE_H__
#define __ETHERNET_DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Providers.h"
#include <UdpSocket.h>
#include "kdhome.h"

class IInputProviderListener;
class IIRProviderListener;

class EthernetDevice
{
public:
	EthernetDevice(UdpSocket* server, Controller* controller, uint32_t m_id, const string& ip, uint16_t port, const string& name);
	~EthernetDevice();

	void addProvider(IProvider* provider) { m_providers.push_back(provider); }
	bool hasProvider(uint16_t type);
	IProvider* getProvider(uint16_t type);

	const string& getIP () const { return m_ip; }
	uint16_t getPort() const { return m_port; }
	uint32_t getID () const { return m_id; }
	string getName () const;

	void processData(const void* buffer, int len);
	void process();
	void checkConnection();

	void preparePacket(TSrvHeader* packet);
	void sendData(const void* data, int len);

private:
	string m_ip, m_name;
	uint16_t m_port;
	uint32_t m_id;
	UdpSocket *m_server;
	uint32_t m_lastPacketTime, m_registrationDataSendTime;
	bool m_connected;

	uint16_t m_lastRecvPacketId, m_sendPacketId;
	uint16_t m_sessKey;

	vector<IProvider*> m_providers;

	Controller *m_controller;

	void markDisconnected();
	void markConnected();
	void registerDevice();

	void logInfo(const string& msg);
};

#endif
