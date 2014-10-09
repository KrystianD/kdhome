#include "EthernetDevice.h"

#include "common.h"
#include "kutils.h"

EthernetDevice::EthernetDevice(UdpServer* server, uint32_t id, const string& ip)
	: m_server(server), m_id(id), m_ip(ip), m_lastPacketTime(0), m_connected(false), m_lastRecvPacketId(0), m_sendPacketId(0),
	  m_sessKey(0), m_registrationDataSendTime(0)
{
}
EthernetDevice::~EthernetDevice()
{
	for (auto& prov : m_providers)
		delete prov;
}

bool EthernetDevice::hasProvider(uint16_t type)
{
	for (size_t i = 0; i < m_providers.size(); i++)
		if (m_providers[i]->getType() == type)
			return true;
	return false;
}
IProvider* EthernetDevice::getProvider(uint16_t type)
{
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		if (provider->getType() == type)
		{
			return provider;
		}
	}
	return 0;
}

string EthernetDevice::getName() const
{
	return str(Format("EthDev {}") << getIP());
}

void EthernetDevice::processData(ByteBuffer& buffer)
{
	m_lastPacketTime = getTicks();
	checkConnection();
	
	uint16_t packetId, sessKey, type;
	buffer.fetch(packetId);
	buffer.fetch(sessKey);
	buffer.fetch(type);
	
	logInfo(str(Format("Packet #{} received - type: {} sessKey: {}") << packetId << type << sessKey));
	
	if (m_connected)
	{
		if (m_sessKey == 0 || m_sessKey != sessKey)
		{
			markDisconnected();
			registerDevice();
			return;
		}
		
		if (packetId <= m_lastRecvPacketId && packetId - m_lastRecvPacketId > 10) // prevent disconnecting on wrap
		{
			markDisconnected();
			registerDevice();
			return;
		}
		
		m_lastRecvPacketId = packetId;
		
		for (size_t i = 0; i < m_providers.size(); i++)
		{
			IProvider *provider = m_providers[i];
			if (provider->getType() == type)
			{
				provider->processData(buffer);
				break;
			}
		}
	}
	else // if not connected
	{
		if (m_sessKey == sessKey)
		{
			markConnected();
		}
		else
		{
			if (getTicks() - m_registrationDataSendTime > REGISTRATION_DATA_DELAY)
			{
				registerDevice();
			}
		}
	}
}
void EthernetDevice::process()
{
	checkConnection();
}
void EthernetDevice::checkConnection()
{
	uint32_t ticks = getTicks();
	if (m_connected)
	{
		if (ticks - m_lastPacketTime >= PACKETLOSS_DISCONNECT_DELAY)
		{
			markDisconnected();
		}
	}
	if (m_connected)
	{
		for (size_t i = 0; i < m_providers.size(); i++)
		{
			IProvider *provider = m_providers[i];
			provider->process();
		}
	}
}

void EthernetDevice::prepareBuffer(ByteBuffer& buffer)
{
	buffer.append(m_sendPacketId);
	
	m_sendPacketId++;
}
void EthernetDevice::preparePacket(TSrvHeader* packet)
{
	packet->packetId = m_sendPacketId;
	
	m_sendPacketId++;
}
void EthernetDevice::sendData(ByteBuffer& buffer)
{
	m_server->sendData(getIP(), buffer);
	logInfo(str(Format("Packet sent")));
}
void EthernetDevice::sendData(const void* data, int len)
{
	m_server->sendData(getIP(), data, len);
	logInfo(str(Format("Packet sent")));
}

void EthernetDevice::markDisconnected()
{
	m_connected = false;
	m_sessKey = 0;
	logInfo("Disconnected");
	
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		provider->deinit();
	}
}
void EthernetDevice::markConnected()
{
	m_connected = true;
	logInfo("Connected");
	m_lastRecvPacketId = 0;
	
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		provider->init();
	}
}
void EthernetDevice::registerDevice()
{
	m_sessKey = rand() % (0xffff - 1) + 1;
	m_lastRecvPacketId = 0;
	
	ByteBuffer buf;
	prepareBuffer(buf);
	
	uint16_t type = 0x0000;
	uint8_t cmd = 0x01;
	buf.append(type);
	buf.append(cmd);
	buf.append(m_sessKey);
	logInfo(str(Format("Sending registration data... - sessKey: {}") << m_sessKey));
	sendData(buf);
	m_registrationDataSendTime = getTicks();
}

void EthernetDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[{}] {}") << getName() << msg);
}
