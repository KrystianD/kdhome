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
	int m_errors;

	uint8_t m_buf[1500 + 8];
	libusb_transfer *m_transfer, *m_readTransfer;
	libusb_device_handle *m_dev;
	bool m_hasData;
	// string m_ip;
	// UdpServer *m_server;

	void logInfo(const string& msg);
	static void callback_intr_s(libusb_transfer* transfer) { ((UsbDevice*)transfer->user_data)->callback_intr(transfer); }
	static void callback_control_s(libusb_transfer* transfer) { ((UsbDevice*)transfer->user_data)->callback_control(transfer); }
	void callback_intr(libusb_transfer* transfer);
	void callback_control(libusb_transfer* transfer);

	bool sendCommand (uint8_t cmd, uint16_t value, uint16_t index, bool out, uint8_t* buffer, int* len);
};

#endif
