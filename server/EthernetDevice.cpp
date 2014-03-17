#include "EthernetDevice.h"

#include "common.h"
#include "kutils.h"

EthernetDevice::EthernetDevice(uint32_t id, UdpServer* server, const string& ip)
	: Device(id), m_server(server), m_ip(ip)
{
}

void EthernetDevice::sendData(ByteBuffer& buffer)
{
	m_server->sendData(getIP(), buffer);
	logInfo(str(Format("Packet sent")));
}

void EthernetDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[EthDev {}] {}") << getIP() << msg);
}
