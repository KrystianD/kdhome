#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include <stdint.h>
#include "common.h"
#include "ByteBuffer.h"

class Device;
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
	~IProvider();

	virtual uint16_t getType() = 0;
	virtual void init() = 0;
	virtual void deinit() = 0;
	virtual void processData(ByteBuffer& buffer) = 0;
	virtual void process() = 0;
	virtual Device* getDevice() = 0;

	StorageEngine* getStorage() { return m_storage; }

protected:
	StorageEngine *m_storage;
};

// class IInputProvider
// {
// public:
	// struct TInput
	// {
		// uint8_t flags;
		// bool state;
		// uint8_t debounceTime;
		// uint16_t patternTimeout;
	// };

	// virtual int getInputsAmount () = 0;
	// virtual int getInputState (int num) = 0;
	// virtual void setInputDebounceTime (int num, int time) = 0;
	// virtual void setInputPatternTimeout (int num, int timeout) = 0;
	// virtual void setInputChangeListener (IInputChangeListener* listener) = 0;
// };



#endif
