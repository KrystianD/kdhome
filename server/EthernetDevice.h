#ifndef __ETHERNET_DEVICE_H__
#define __ETHERNET_DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Device.h"
#include "Providers.h"
#include "UdpServer.h"

class EthernetDevice : public Device
{
public:
	EthernetDevice(uint32_t id, UdpServer* server, const string& ip);
	~EthernetDevice() { }

	const string& getIP () const { return m_ip; }

	void sendData(ByteBuffer& data);

private:
	string m_ip;
	UdpServer *m_server;

	void logInfo(const string& msg);
};

#endif
