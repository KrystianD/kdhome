#include "UsbDevice.h"

#include <libusb.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "kutils.h"

UsbDevice::UsbDevice(uint32_t id, uint16_t vendor, uint16_t product)
	: Device(id), m_vendor(vendor), m_product(product), m_errors(0), m_dev(0), m_hasData(false)
{
	logInfo(str(Format("Created")));

	m_transfer = libusb_alloc_transfer(0); // free
	m_readTransfer = libusb_alloc_transfer(0); // free
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
					m_buf, sizeof(m_buf), UsbDevice::callback_intr_s, this, 0);

			int res = libusb_submit_transfer(m_transfer);
			printf("res %d\n", res);
		}
	}

	if (m_dev)
	{
		if (m_hasData)
		{
			uint8_t data[1500];
			int len = 1500;
			if (!sendCommand(0, 0, 0, false, data, &len))
			{
			}

			for(int i=0;i<len;i++)
			{
				// printf("%02x, ", data[i]);
			}
			ByteBuffer buffer((char*)data, len);
			processData(buffer);


			m_hasData = false;
			// printf("d %d\n", len);
		}
	}
}

void UsbDevice::sendData(ByteBuffer& buffer)
{
	// m_server->sendData(getIP(), buffer);

	logInfo(str(Format("send {}") << buffer.size()));
	int len = buffer.size();
	if (!sendCommand(1, 0, 0, true, const_cast<uint8_t*>((const uint8_t*)buffer.ptr()), &len))
	{
	}

	logInfo(str(Format("Packet sent {}") << len));
}

void UsbDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[UsbDev #{}] {}") << getID() << msg);
}
void UsbDevice::callback_intr(libusb_transfer* transfer)
{
	printf("inte  %d\n", transfer->status);
	switch (transfer->status)
	{
	case LIBUSB_TRANSFER_NO_DEVICE:
		m_dev = 0;
		return;
	}

	libusb_submit_transfer(transfer);

	m_hasData = true;
	// libusb_fill_control_setup(
	// m_buf,
	// LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN,
	// 0, 0, 0, sizeof(m_buf) - 8);
	// libusb_fill_control_transfer(m_readTransfer, m_dev, m_buf, UsbDevice::callback_control_s, this, 0);
	// libusb_submit_transfer(m_readTransfer);

}
void UsbDevice::callback_control(libusb_transfer* transfer)
{
	printf("ctrl  %d\n", transfer->status);

	// libusb_control_setup *setup = libusb_control_transfer_get_setup(transfer);
	// printf("ctrl  %d\n", setup->wLength);
	const char *data = (char*)libusb_control_transfer_get_data(transfer);

	for(int i=0;i<transfer->actual_length;i++)
	{
		printf("%02x, ", data[i]);
	}
	ByteBuffer buffer(data, transfer->actual_length);
	processData(buffer);
}



bool UsbDevice::sendCommand (uint8_t cmd, uint16_t value, uint16_t index, bool out, uint8_t* buffer, int* len)
{
	// printf ("dose\n");
	int tries = 0;
retry_send:
	int toRead = len ? *len : 0;
	// if (!checkConnection ())
	// return false;
	// uint32_t t1 = getTicks ();
	// m_usbMutex.lock ();
	// uint32_t t2 = getTicks ();
	// printf ("-[%s] %lu\n", m_name.c_str(), t2-t1);
	// printf ("se locked\n");
	// if (m_state != USBST_CONNECTED)
	{
		// printf ("se unlocked 1\n");
		// m_usbMutex.unlock ();
		// return false;
	}
	// m_usbMutex.unlock ();
	// m_ctrlTransferMutex.lock ();
	// printf ("W %d \n", 3);
	int res = libusb_control_transfer (
			m_dev,
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE
			| (out ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN),
			cmd,
			value, index, buffer, toRead, 100);
	// m_ctrlTransferMutex.unlock ();
	// printf ("se unlocked 2\n");
	// printf ("Q %d \n", res);
	if (res >= 0)
	{
		if (len)
			*len = res;
		return true;
	}
	else if (res == LIBUSB_ERROR_PIPE || res == LIBUSB_ERROR_NO_DEVICE)
	{
		printf ("erp nodev\n");
		// disconnect ();
		usleep (1000 * 1000);
		return false;
	}
	else if (res == LIBUSB_ERROR_TIMEOUT)
	{
		tries++;
		usleep (200 * 1000);
		if (tries == 5)
		{
			printf ("tmout\n");
			// disconnect ();
			return false;
		}
		goto retry_send;
	}
	else
	{
		tries++;
		usleep (200 * 1000);
		if (tries == 5)
		{
			m_errors++;
			// if (m_errors == 10) { disconnect (); return false; }
			// logger->logWarning (boost::format ("[USB,%1%] Invalid res %2%") % m_name % res);
			return false;
		}
		goto retry_send;

	}
}
