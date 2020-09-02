#pragma once
#include <stdint.h>

#include "memoryManager.h"

class DMG_Timer
{
public : 
	DMG_Timer(memoryManager& mMng);
	~DMG_Timer();

	void run(int nbCycles);
	void reset();

private : 
	memoryManager& memMng;

	uint16_t timerCounter;
	uint16_t prevTimerCounter;
	int delayBeforeOverflowMgmt;

	const uint16_t MASK_TIMER_4194Hz = (1 << 10);
	const uint16_t MASK_TIMER_16384Hz = (1 << 8);
	const uint16_t MASK_TIMER_65536Hz = (1 << 6);
	const uint16_t MASK_TIMER_262144Hz =( 1 << 4);
};
