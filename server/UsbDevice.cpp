#include "UsbDevice.h"

#include <libusb.h>

#include "common.h"
#include "kutils.h"

UsbDevice::UsbDevice(uint32_t id, uint16_t vendor, uint16_t product)
	: Device(id), m_vendor(vendor), m_product(product)
{
	logInfo(str(Format("Created")));

	m_dev = 0;

	m_transfer = libusb_alloc_transfer(0); // free
}

void UsbDevice::process()
{
	Device::process();

	if (!m_dev)
	{
		m_dev = libusb_open_device_with_vid_pid(0, m_vendor, m_product);
		if (m_dev)
		{
			logInfo("Connected");

			libusb_fill_interrupt_transfer(m_transfer, m_dev, LIBUSB_ENDPOINT_IN | 1,
					m_buf, sizeof(m_buf), UsbDevice::callback_s, this, 0);

			int res = libusb_submit_transfer(m_transfer);
			printf("res %d\n", res);
		}
	}

}

void UsbDevice::sendData(ByteBuffer& buffer)
{
	// m_server->sendData(getIP(), buffer);
	// logInfo(str(Format("Packet sent")));
}

void UsbDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[UsbDev #{}] {}") << getID() << msg);
}
void UsbDevice::callback_s(libusb_transfer* transfer)
{
	((UsbDevice*)transfer->user_data)->callback(transfer);
}
void UsbDevice::callback(libusb_transfer* transfer)
{
	printf("inte  %d\n", transfer->status);
	switch (transfer->status)
	{
	case LIBUSB_TRANSFER_NO_DEVICE:
		m_dev = 0;
		return;
	}




	libusb_submit_transfer(m_transfer);
}
