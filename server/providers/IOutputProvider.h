#ifndef __IOUTPUT_PROVIDER_H__
#define __IOUTPUT_PROVIDER_H__

#include <kdhome.h>

class IOutputProvider : public IProvider
{
public:
	static uint16_t getTypeStatic() { return PROVIDER_TYPE_OUTPUT; }
	uint16_t getType() { return IOutputProvider::getTypeStatic(); }
	virtual int getAmount () = 0;
	virtual bool getOutputState (int num) = 0;
	virtual void setOutputState (int num, bool on) = 0;
	virtual void toggleOutputState (int num) = 0;
	virtual string getOutputID(int num) = 0;
};

#endif
