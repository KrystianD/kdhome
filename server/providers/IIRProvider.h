#ifndef __IIR_PROVIDER_H__
#define __IIR_PROVIDER_H__

class IIRProvider;

class IIRProviderListener
{
public:
	virtual void onIRCodeReceived(IIRProvider* provider, uint32_t code) = 0;
	virtual void onIRButtonPressed(IIRProvider* provider, uint32_t code) = 0;
	virtual void onIRButtonReleased(IIRProvider* provider, uint32_t code) = 0;
};

class IIRProvider : public IProvider
{
public:
	static uint16_t getTypeStatic() { return 0x0003; }
	uint16_t getType() { return 0x0003; }
	virtual void setListener(IIRProviderListener* listener) = 0;
};

#endif
