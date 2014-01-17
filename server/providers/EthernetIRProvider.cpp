#include "EthernetIRProvider.h"

#include "kutils.h"

#include "../common.h"

EthernetIRProvider::EthernetIRProvider(EthernetDevice* device)
	: m_device(device), m_listener(0)
{
}

void EthernetIRProvider::processData(ByteBuffer& buffer)
{
	uint8_t cmd;
	buffer.fetch(cmd);

	switch (cmd)
	{
	case 0x00:
		{
			uint32_t code;
			buffer.fetch(code);
			logger->logInfo(Format("[ir] New code received 0x{:08x}") << code);
			if (code == 0xffffffff)
				code = m_lastCode;
			m_lastCode = code;
			if (m_listener) m_listener->onIRCodeReceived(this, code);

			if (m_codes.find(code) == m_codes.end())
				if (m_listener) m_listener->onIRButtonPressed(this, code);

			m_codes[code] = getTicks();
		}
		break;
	}
}
void EthernetIRProvider::process()
{
	for (auto it = m_codes.begin(); it != m_codes.end(); )
	{
		if (getTicks () - it->second >= 500)
		{
			it = m_codes.erase(it);
			if (m_listener) m_listener->onIRButtonReleased(this, it->first);
		}
		else
		{
			it++;
		}
	}
}

void EthernetIRProvider::update()
{
}
void EthernetIRProvider::prepareCommand(ByteBuffer& buffer, uint8_t command)
{
	m_device->prepareBuffer(buffer);
	buffer.append(getType());
	buffer.append(command);
}
