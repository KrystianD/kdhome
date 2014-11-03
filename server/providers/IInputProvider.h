#ifndef __IINPUT_PROVIDER_H__
#define __IINPUT_PROVIDER_H__

class IInputProvider;

class IInputProviderListener
{
public:
	virtual void onInputChanged(IInputProvider* provider, const string& id, int state) = 0;
};

class IInputProvider : public IProvider
{
public:
	static uint16_t getTypeStatic() { return 0x0002; }
	uint16_t getType() { return 0x0002; }
	virtual int getAmount() = 0;
	virtual int getInputState(int num) = 0;
	virtual void setListener(IInputProviderListener* listener) = 0;
	virtual string getInputID(int num) = 0;
};

#endif
