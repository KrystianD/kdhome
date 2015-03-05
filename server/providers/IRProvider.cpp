#include "IRProvider.h"

#include "kdutils.h"

#include "../common.h"

IRProvider::IRProvider(EthernetDevice* device)
	: m_device(device), m_listener(0)
{
}

void IRProvider::processData(const void* buffer, int len)
{
	uint8_t cmd;
	
	TProvHeader *header = (TProvHeader*)buffer;
	
	switch (header->cmd)
	{
	case IR_NOTF_NEWCODE:
	{
		TProvIRCodePacket *p = (TProvIRCodePacket*)buffer;
		uint32_t code = p->code;
		logger->logInfo(Format("[ir] New code received 0x{:08x}") << code);
		uint32_t diff = getTicks() - m_lastCodeTime;
		logger->logInfo(Format("{0}") << diff);
		if (code == 0xffffffff && getTicks() - m_lastCodeTime <= 300)
			code = m_lastCode;
		if (code != 0xffffffff)
		{
			m_lastCode = code;
			m_lastCodeTime = getTicks();
			if (m_listener) m_listener->onIRCodeReceived(this, code);
			
			if (m_codes.find(code) == m_codes.end())
				if (m_listener) m_listener->onIRButtonPressed(this, code);
				
			m_codes[code] = getTicks();
		}
	}
	break;
	}
}
void IRProvider::process()
{
	for (map<uint32_t, uint32_t>::iterator it = m_codes.begin(); it != m_codes.end();)
	{
		if (getTicks() - it->second >= 500)
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

void IRProvider::update()
{
}
// void IRProvider::prepareCommand(ByteBuffer& buffer, uint8_t command)
// {
// m_device->prepareBuffer(buffer);
// buffer.append(getType());
// buffer.append(command);
// }
