#include "pch.h"
#include "WatchDog.h"

WatchDog::WatchDog(uint16_t wdAddr, std::string* wdName, uint16_t wdCurrentValue, bool wdStatus)
{
	address = wdAddr;

	if (wdName == NULL)
	{
		name = new std::string("unnamed");
	}
	else
	{
		name = wdName;
	}

	currentValue = wdCurrentValue;
	status = wdStatus;
}

WatchDog::~WatchDog()
{
	free (name);
}

void WatchDog::setAddr(uint16_t wdAddr)
{
	address = wdAddr;
}

void WatchDog::setName(std::string* wdName)
{
	if (wdName == NULL)
	{
		name = new std::string("unnamed");
	}
	else
	{
		name = wdName;
	}
}

void WatchDog::setCurrentValue(uint8_t wdCurrentValue)
{
	currentValue = wdCurrentValue;
}

void WatchDog::setStatus(bool wdStatus)
{
	status = wdStatus;
}