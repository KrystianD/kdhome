#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include <stdint.h>
#include "common.h"
#include "ByteBuffer.h"

class EthernetDevice;
class StorageEngine;

class IInputChangeListener
{
public:
	virtual void checkInputs () = 0;
	virtual void enableInputsChecking () = 0;
};

class ITemperatureProvider
{
public:
	virtual int getTempAmount () = 0;
	virtual double getTemperature (int num) = 0;
};

class IProvider
{
public:
	IProvider();
	virtual ~IProvider();

	virtual uint16_t getType() = 0;
	virtual void init() = 0;
	virtual void deinit() = 0;
	virtual void processData(const void* buffer, int len) = 0;
	virtual void process() = 0;
	virtual EthernetDevice* getDevice() = 0;

	StorageEngine* getStorage() { return m_storage; }

protected:
	StorageEngine *m_storage;
};

#endif
