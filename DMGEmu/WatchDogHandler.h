#pragma once
#include "memoryManager.h"
#include "WatchDog.h"

#include <list>

class WatchDogHandler
{
public:
	WatchDogHandler(memoryManager& mMng);
	void addWatchDog(uint16_t wdAddr, std::string* wdName);
	bool removeWatchDog(uint16_t wdAddr);
	bool removeWatchDog(std::string& wdName);
	const WatchDog* checkWatchDogsChange();

private :
	memoryManager &currentMMng;
	std::list<WatchDog*> watchdogs;
};

