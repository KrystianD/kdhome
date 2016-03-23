#include "EthernetDevice.h"

#include "common.h"
#include "kdutils.h"

#include "Controller.h"
#include "providers/InputProvider.h"
#include "providers/OutputProvider.h"
#include "providers/IRProvider.h"
#include "providers/TempProvider.h"
#include "providers/CounterProvider.h"

EthernetDevice::EthernetDevice(UdpSocket* server, Controller* controller, uint32_t id, const string& ip, uint16_t port, const string& name)
	: m_server(server), m_controller(controller), m_id(id), m_ip(ip), m_port(port), m_name(name), m_lastPacketTime(0), m_connected(false), m_lastRecvPacketId(0), m_sendPacketId(0),
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
	return m_name;
}

void EthernetDevice::processData(const void* buffer, int len)
{
	m_lastPacketTime = getTicks();
	checkConnection();
	
	TProvHeader *header = (TProvHeader*)buffer;
	
	logInfo(str(Format("Packet #{} received - type: {} cmd: {} sessKey: {}") << header->packetId << header->type << (int)header->cmd << header->sessKey));
	
	if (m_connected)
	{
		if (m_sessKey == 0 || m_sessKey != header->sessKey)
		{
			markDisconnected();
			registerDevice();
			return;
		}
		
		if (header->packetId <= m_lastRecvPacketId && header->packetId - m_lastRecvPacketId > 10) // prevent disconnecting on wrap
		{
			markDisconnected();
			registerDevice();
			return;
		}
	}
	else // if not connected
	{
		if (m_sessKey == header->sessKey)
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
	
	if (m_connected)
	{
		m_lastRecvPacketId = header->packetId;
		
		if (header->type == 0)
		{
			logInfo(str(Format("cmd: {}") << (int)header->cmd));
		}
		else
		{
			if (header->cmd == 0x00)
			{
				switch(header->type)
				{
				case PROVIDER_TYPE_INPUT:
				{
					TProvInputRegisterPacket *p = (TProvInputRegisterPacket*)buffer;
					logInfo(str(Format("Adding input provider to device #{} with {} inputs") << 0 << (int)p->cnt));
					InputProvider *prov = new InputProvider(this, p->cnt);
					prov->init();
					prov->setListener(dynamic_cast<IInputProviderListener*>(m_controller));
					addProvider(prov);
					break;
				}
				case PROVIDER_TYPE_OUTPUT:
				{
					TProvOutputRegisterPacket *p = (TProvOutputRegisterPacket*)buffer;
					logInfo(str(Format("Adding output provider to device #{} with {} outputs") << 0 << (int)p->cnt));
					OutputProvider *prov = new OutputProvider(this, p->cnt);
					prov->init();
					prov->setListener(dynamic_cast<IOutputProviderListener*>(m_controller));
					addProvider(prov);
					break;
				}
				case PROVIDER_TYPE_IR:
				{
					TProvIRRegisterPacket *p = (TProvIRRegisterPacket*)buffer;
					logInfo(str(Format("Adding IR provider to device #{}") << 0));
					IRProvider *prov = new IRProvider(this);
					prov->init();
					prov->setListener(dynamic_cast<IIRProviderListener*>(m_controller));
					addProvider(prov);
					break;
				}
				case PROVIDER_TYPE_TEMP:
				{
					TProvTempRegisterPacket *p = (TProvTempRegisterPacket*)buffer;
					logInfo(str(Format("Adding temp provider to device #{} with {} sensors") << 0 << (int)p->cnt));
					TempProvider *prov = new TempProvider(this, p->cnt);
					prov->init();
					addProvider(prov);
					break;
				}
				case PROVIDER_TYPE_COUNTER:
				{
					TProvCounterRegisterPacket *p = (TProvCounterRegisterPacket*)buffer;
					logInfo(str(Format("Adding counter provider to device #{} with {} counters") << 0 << (int)p->cnt));
					CounterProvider *prov = new CounterProvider(this, p->cnt);
					prov->init();
					prov->setListener(dynamic_cast<ICounterProviderListener*>(m_controller));
					addProvider(prov);
					break;
				}
				default:
					logInfo("Unknown provider type");
					break;
				}
				// logInfo(str(Format("Registering provider of type: {}") << (int)header->type));
			}
			else
			{
				for (size_t i = 0; i < m_providers.size(); i++)
				{
					IProvider *provider = m_providers[i];
					if (provider->getType() == header->type)
					{
						provider->processData(buffer, len);
						break;
					}
				}
			}
		}
	}
}
void EthernetDevice::process()
{
	checkConnection();
	if (m_connected)
	{
		for (size_t i = 0; i < m_providers.size(); i++)
		{
			IProvider *provider = m_providers[i];
			provider->process();
		}
	}
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
}

void EthernetDevice::preparePacket(TSrvHeader* packet)
{
	packet->packetId = m_sendPacketId;
	
	m_sendPacketId++;
}
void EthernetDevice::sendData(const void* data, int len)
{
	m_server->send(getIP(), getPort(), data, len);
	logInfo(str(Format("Packet sent to {}:{}") << getIP() << getPort()));
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
		delete provider;
	}
	m_providers.clear();
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
	
	TSrvCmdRegister packet;
	preparePacket((TSrvHeader*)&packet);
	
	packet.header.type = PROVIDER_TYPE_CONTROL;
	packet.header.cmd = CONTROL_CMD_REGISTER;
	packet.sessKey = m_sessKey;
	logInfo(str(Format("Sending registration data... - sessKey: {}") << m_sessKey));
	sendData(&packet, sizeof(packet));
	m_registrationDataSendTime = getTicks();
}

void EthernetDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[{}] {}") << getName() << msg);
}
