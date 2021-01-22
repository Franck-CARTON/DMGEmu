#include "pch.h"
#include "WatchDogHandler.h"
#include <iterator>

WatchDogHandler::WatchDogHandler(memoryManager& mMng) : currentMMng(mMng)
{
}


bool WatchDogHandler::removeWatchDog(uint16_t wdAddr)
{
	std::list<WatchDog*>::iterator it = watchdogs.begin();
	while (it != watchdogs.end())
	{
		if ((*it)->getAddr() == wdAddr)
		{
			free((void*)(*it));
			//watchdogs.remove(*it);
			it = watchdogs.erase(it);
			break;
		}
		it++;
	}
	return true;
}

bool WatchDogHandler::removeWatchDog(std::string& wdName)
{
	std::list<WatchDog*>::iterator it = watchdogs.begin();
	while (it != watchdogs.end())
	{
		if ((*it)->getName()->compare(wdName) == 0)
		{
			free((void*)(*it));
			//watchdogs.remove(*it);
			it = watchdogs.erase(it);
			break;
		}
		it++;
	}
	return true;
}

void WatchDogHandler::addWatchDog(uint16_t wdAddr, std::string* wdName)
{
	uint8_t currentVal = currentMMng.readByte(wdAddr, true);
	watchdogs.push_back(new WatchDog(wdAddr, wdName, currentVal, true));
}
 
const WatchDog* WatchDogHandler::checkWatchDogsChange()
{
	std::list<WatchDog*>::iterator it = watchdogs.begin();
	while (it != watchdogs.end())
	{
		// get current value : 
		uint16_t wdAddr = (*it)->getAddr();
		uint8_t readVal = currentMMng.readByte(wdAddr, true);

		if (readVal != (*it)->getCurrentValue())
		{
			(*it)->setCurrentValue(readVal);
			return (*it);
			break;
		}
		it++;
	}
	return NULL;
}