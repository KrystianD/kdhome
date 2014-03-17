#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include <string>
#include <vector>
using namespace std;

#include "Device.h"
#include "Providers.h"
// #include "UdpServer.h"

#include <libusb.h>

class UsbDevice : public Device
{
public:
	UsbDevice(uint32_t id, uint16_t vendor, uint16_t product);
	~UsbDevice() { }

	// const string& getIP () const { return m_ip; }
	void process();

	void sendData(ByteBuffer& data);

private:
	uint16_t m_vendor, m_product;

	uint8_t m_buf[1500];
	libusb_transfer* m_transfer;
	libusb_device_handle* m_dev;
	// string m_ip;
	// UdpServer *m_server;

	void logInfo(const string& msg);
	static void callback_s(libusb_transfer* transfer);
	void callback(libusb_transfer* transfer);
};

#endif
