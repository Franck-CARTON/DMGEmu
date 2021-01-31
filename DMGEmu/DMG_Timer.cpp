#include "pch.h"
#include "DMG_Timer.h"
#include <iostream>

DMG_Timer::DMG_Timer(memoryManager& mMng) : memMng(mMng)
{
	reset();
}


DMG_Timer::~DMG_Timer()
{
}

void DMG_Timer::reset()
{
	timerCounter = 0;
	prevTimerCounter = 0;
	delayBeforeOverflowMgmt = 0;
}


void DMG_Timer::run(int nbCycles)
{
	// read DIV :
	uint8_t DIV = memMng.readByte(0xff04, true);
	// Read TIMA : 
	uint8_t TIMA = memMng.readByte(0xff05, true);
	// read TMA :
	uint8_t TMA = memMng.readByte(0xff06, true);
	// Read TAC : 
	uint8_t TAC = memMng.readByte(0xff07, true);
	// Read IF (Interruption flag) : 
	uint8_t IF = memMng.readByte(0xff0f, true);

	// Decode TAC internal status =>
	bool timerEnabled = TAC & 0x04;
	uint8_t selectedClock = TAC & 0x03;

	// Check if DIV has been ovrewrited :
	uint8_t prevDIV = (uint8_t)(timerCounter >> 8);
	if (prevDIV != DIV)
	{
		DIV = 0;
		timerCounter = 0;
		memMng.writeByte(0xff04, DIV, true);
	}

	for (int i = 0; i < nbCycles; i++)
	{
		if (timerEnabled)
		{
			// Check internal timer and update register:
			uint8_t selectedClock = TAC & 0x03;
			int mask2Use = 0;

			switch (selectedClock)
			{
			case 0:
				mask2Use = MASK_TIMER_4194Hz;
				break;
			case 1:
				mask2Use = MASK_TIMER_262144Hz;
				break;
			case 2:
				mask2Use = MASK_TIMER_65536Hz;
				break;
			case 3:
				mask2Use = MASK_TIMER_16384Hz;
				break;
			default:
				mask2Use = 0;
			}

			// Update TIMA =>
			if ((prevTimerCounter & mask2Use) != (timerCounter & mask2Use))
			{
				if (TIMA == 0xff)
				{
					// TIMA Overflow mgmt is deayed for 4 clock :
					delayBeforeOverflowMgmt = 4;
				}
				else
				{
					TIMA++;
				}
				memMng.writeByte(0xff05, TIMA, true);
			}
		}

		// Handle Delayed overflow mgmt :
		if (delayBeforeOverflowMgmt >0)
		{
			delayBeforeOverflowMgmt--;
			if (delayBeforeOverflowMgmt == 0)
			{
				// TIMA Overflow => Reset TIMA with TMA Value
				TIMA = TMA;
				memMng.writeByte(0xff05, TIMA, true);

				// TIMA Overflow => Set Timer Interruption Request :
				IF = IF & 0x04;
				memMng.writeByte(0xff0f, IF, true);
			}
		}
		prevTimerCounter = timerCounter;
		timerCounter++;
	}

	// DIV register is in fact the Most Significant Byte from a 16 register => Set DIV to timerCounter's MSB
	DIV = (uint8_t)(timerCounter >> 8);
	memMng.writeByte(0xff04, DIV, true);
}

