#ifndef __IEVENT_H__
#define __IEVENT_H__

class IEvent
{
public:
	// virtual int getType () const = 0;

	virtual std::string toString() const { return ""; }
};

#endif
