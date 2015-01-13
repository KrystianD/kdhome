#ifndef __IINPUT_PROVIDER_H__
#define __IINPUT_PROVIDER_H__

#include <kdhome.h>

class ICounterProvider;

class ICounterProviderListener
{
public:
	virtual void onCounterChanged(ICounterProvider* provider, const string& id, uint32_t value) = 0;
};

class ICounterProvider : public IProvider
{
public:
	static uint16_t getTypeStatic() { return PROVIDER_TYPE_COUNTER; }
	uint16_t getType() { return PROVIDER_TYPE_COUNTER; }
	virtual int getAmount() = 0;
	virtual uint32_t getCounterValue(int num) = 0;
	virtual void setListener(IInputProviderListener* listener) = 0;
	virtual string getInputID(int num) = 0;
};

#endif
