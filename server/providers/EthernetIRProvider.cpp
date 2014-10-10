#include "EthernetIRProvider.h"

#include "kdutils.h"

#include "../common.h"

EthernetIRProvider::EthernetIRProvider(EthernetDevice* device)
	: m_device(device), m_listener(0)
{
}

void EthernetIRProvider::processData(ByteBuffer& buffer)
{
	uint8_t cmd;
	if (!buffer.fetch(cmd)) return;

	switch (cmd)
	{
	case IR_NOTF_NEWCODE:
		{
			uint32_t code;
			if (!buffer.fetch(code)) return;
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
	for (map<uint32_t,uint32_t>::iterator it = m_codes.begin(); it != m_codes.end(); )
	{
		if (getTicks () - it->second >= 500)
		{
			// it = m_codes.erase(it);
			m_codes.erase(it);
			if (m_listener) m_listener->onIRButtonReleased(this, it->first);
			it = m_codes.begin();
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
