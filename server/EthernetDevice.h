#ifndef __ETHERNET_DEVICE_H__
#define __ETHERNET_DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Providers.h"
#include "UdpServer.h"
#include "kdhome.h"

class IInputProviderListener;
class IIRProviderListener;

class EthernetDevice
{
public:
	EthernetDevice(UdpServer* server, uint32_t m_id, const string& ip, const string& name);
	~EthernetDevice();

	void addProvider(IProvider* provider) { m_providers.push_back(provider); }
	bool hasProvider(uint16_t type);
	IProvider* getProvider(uint16_t type);

	const string& getIP () const { return m_ip; }
	uint32_t getID () const { return m_id; }
	string getName () const;

	void processData(ByteBuffer& buffer);
	void process();
	void checkConnection();

	void prepareBuffer(ByteBuffer& buffer);
	void preparePacket(TSrvHeader* packet);
	void sendData(ByteBuffer& data);
	void sendData(const void* data, int len);

	void setInputListener(IInputProviderListener* listener) { m_inputListener = listener; }
	void setIRListener(IIRProviderListener* listener) { m_irListener = listener; }

private:
	string m_ip, m_name;
	uint32_t m_id;
	UdpServer *m_server;
	uint32_t m_lastPacketTime, m_registrationDataSendTime;
	bool m_connected;

	uint16_t m_lastRecvPacketId, m_sendPacketId;
	uint16_t m_sessKey;

	vector<IProvider*> m_providers;

	IInputProviderListener *m_inputListener;
	IIRProviderListener *m_irListener;

	void markDisconnected();
	void markConnected();
	void registerDevice();

	void logInfo(const string& msg);
};

#endif
