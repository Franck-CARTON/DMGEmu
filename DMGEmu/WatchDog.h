#pragma once
#include <stdint.h>
#include <string>

class WatchDog
{
public :

	WatchDog(uint16_t wdAddr, std::string *wdName, uint16_t wdCurrentValue, bool wdStatus);
	~WatchDog();

	 uint16_t getAddr() const { return address; }
	 const std::string* getName() const { return name; }
	 uint16_t getCurrentValue()const { return currentValue; }
	 bool getStatus()const { return status; }

	void setAddr(uint16_t wdAddress);
	void setName(std::string* wdName);
	void setCurrentValue(uint8_t wdCurrentValue);
	void setStatus(bool wdStatus);

private :
	uint16_t address;
	std::string *name;
	uint8_t currentValue;
	bool status;
};

