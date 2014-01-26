#ifndef __ITEMP_PROVIDER_H__
#define __ITEMP_PROVIDER_H__

#include <kdhome.h>

class ITempProvider : public IProvider
{
public:
	static uint16_t getTypeStatic() { return PROVIDER_TYPE_TEMP; }
	uint16_t getType() { return PROVIDER_TYPE_TEMP; }
	virtual int getAmount() = 0;
	virtual bool isTempValid(int num) = 0;
	virtual float getTemp(int num) = 0;
};

#endif
