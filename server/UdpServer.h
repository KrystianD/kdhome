#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include <string>
using namespace std;

#include "ByteBuffer.h"

class IEthernetDataListener
{
public:
	virtual void onEthernetDataReceived(const string& ip, ByteBuffer& buffer) = 0;
};

class UdpServer
{
public:
	UdpServer();
	~UdpServer();

	bool init();
	bool process();
	void setListener(IEthernetDataListener* listener) { m_listener = listener; }

	void sendData(const string& ip, ByteBuffer& buffer);

private:
	int m_sockfd;

	IEthernetDataListener *m_listener;

	string m_lastErrorStr;
};

#endif
