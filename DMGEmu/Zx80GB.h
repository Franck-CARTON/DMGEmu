#pragma once
#include <string>
#include "memoryManager.h"
#include <iostream>

class Zx80GB
{
public:
	Zx80GB(memoryManager &memMng);
	~Zx80GB();

	// Read next Opcode from memory
	// Debug to set opcodeRead with the opcode as a string
	int runZx80(bool debug=false);

//private : // Not private while debugging

	// Method to handle every sub function of CB opcode
	int readOpcode(bool debug);
	int handlePrefixCB(bool debug);

	uint16_t stackPointer;
	uint16_t programCounter;

	// Register declaration :
	__int8 regA;
	__int8 regB;
	__int8 regC;
	__int8 regD;
	__int8 regE;
	__int8 regL;
	__int8 regH;

	// Bursted F register (flag status)
	bool ZeroFlag;
	bool SubFlag;
	bool HalfCarryFlag;
	bool CarryFlag;

	// INterruption mgmt
	bool IME;

	// CPU Status
	bool haltPending;
	bool isStopped;

	// For debugger :
	// #TODO => Find a better way. translation should be done in another module
	std::string opcodeRead;

	// Memory manager
	memoryManager &memMng;

	// Handle IRQ from devices
	void handleIRQ();

	// Handle timer register and IRQ
	int clock16384Hz;
	void handleTimer(int inc);

	// Opcode Mgmt : WARNING - Check if should be inline ?
	void opAdd(__int8 &regDest, __int8 regVal);
	void opAdd16(uint16_t &regDest, uint16_t regVal);
	void opAdc(__int8 regVal);
	void opInc8(__int8 &reg);
	void opDec8(__int8 &reg);
	void opOr(__int8 reg);
	void opAnd(__int8 reg);
	void opXor(__int8 reg);
	void opRet();
	void opRst(__int8 offset);
	void opRes(__int8 bitNumber, _int8  &reg);
	void opBit(__int8 bitNumber, __int8 reg);
	void opSet(__int8 bitNumber, _int8  &reg);
	void opCall(uint16_t addr);
	void opPush(_int8 h, _int8 l);
	void opPop(_int8 &h, _int8 &l);
	void opSla(__int8 &reg);
	void opRlc(__int8 &reg);
	void opRl(__int8 &eg);
	void opSra(__int8 &reg);
	void opSrl(__int8 &reg);
	void opRrc(__int8 &reg);
	void opRr(__int8 &reg);
	void opSwap(__int8 &reg);
	void opCp(__int8 regVal);


	inline uint16_t getHLReg()
	{
		uint16_t val = ((unsigned char)regH << 8) | (unsigned char)regL;
		return val;
	}

	inline void setHLReg(uint16_t val)
	{
		regH = (val >> 8) & 0x00FF;
		regL = val & 0x00FF;
	}

	inline uint16_t getBCReg()
	{
		uint16_t val = ((unsigned char)regB << 8) | (unsigned char)regC;
		return val;
	}

	inline void setBCReg(uint16_t val)
	{
		regB = (val >> 8) & 0x00FF;
		regC = val & 0x00FF;
	}

	// Get register D & E as a 16bits register
	inline uint16_t getDEReg()
	{
		uint16_t val = ((unsigned char)regD << 8) | (unsigned char)regE;
		return val;
	}

	// Set register D & E as a 16bits register
	inline void setDEReg(uint16_t val)
	{
		regD = (val >> 8) & 0x00FF;
		regE = val & 0x00FF;
	}

	// Retrieve Zero flag from F register
	inline bool getZeroFlag()
	{
		return ZeroFlag;
	}

	// set Zero flag from F register
	inline void setZeroFlag()
	{
		ZeroFlag = true;
	}

	// Reset Zero flag from F register
	inline void resetZeroFlag()
	{
		ZeroFlag = false;
	}


	inline bool getSubFlag()
	{
		return SubFlag;
	}

	inline void setSubFlag()
	{
		SubFlag = true;
	}

	inline void resetSubFlag()
	{
		SubFlag = false;
	}


	inline bool getHalfCarryFlag()
	{
		return HalfCarryFlag;
	}

	inline void setHalfCarryFlag()
	{
		HalfCarryFlag = true;
	}

	inline void resetHalfCarryFlag()
	{
		HalfCarryFlag = false;
	}


	inline bool getCarryFlag()
	{
		return CarryFlag;
	}

	inline void setCarryFlag()
	{
		CarryFlag = true;
	}

	inline void resetCarryFlag()
	{
		CarryFlag = false;
	}

	/*
	inline void resetBitNOfReg(_int8 bitMask, _int8 *reg)
	{
		programCounter++;
		_int8 res = *reg & bitMask;
	}
	*/

	inline int sub(__int8 regVal)
	{
		programCounter++;
		__int16 valA = (__int16)(regA) & 0x00ff;
		__int16 valB = (__int16)(regVal) & 0x00ff;
		__int16 result = valA - valB;
		__int8 testHalfCarry = (regA & 0x0f) - (regVal & 0x0f);

		regA = (__int8)(result & 0x00ff);

		if (regA == 0)
			setZeroFlag();
		else
			resetZeroFlag();

		setSubFlag();

		if (testHalfCarry < 0x00)
			setHalfCarryFlag();
		else
			resetHalfCarryFlag();

		if (result < 0x0000)
			setCarryFlag();
		else
			resetCarryFlag();

		return 4;
	}

	inline int sbc(__int8 regVal)
	{
		programCounter++;
		__int16 valA = (__int16)(regA) & 0x00ff;
		__int16 valB = (__int16)(regVal) & 0x00ff;
		bool carry = getCarryFlag();
		__int16 result = valA - valB - carry;
		__int8 testHalfCarry = (regA & 0x0f) - (regVal & 0x0f) - (__int8)carry;

		regA = (__int8)(result & 0x00ff);

		if (regA == 0)
			setZeroFlag();
		else
			resetZeroFlag();

		setSubFlag();

		if (testHalfCarry < 0x00)
			setHalfCarryFlag();
		else
			resetHalfCarryFlag();

		if (result < 0x0000)
			setCarryFlag();
		else
			resetCarryFlag();

		return 4;
	}

	inline __int8 getFlagRegister()
	{
		__int8 result = 0;
		if (ZeroFlag)
			result += 0x80;
		if (SubFlag)
			result += 0x40;
		if (HalfCarryFlag)
			result += 0x20;
		if (CarryFlag)
			result += 0x10;
		return result;
	}

	inline void setFlagRegister(__int8 flagReg)
	{
		ZeroFlag = ((flagReg & 0x80) != 0);
		SubFlag = ((flagReg & 0x40) != 0);
		HalfCarryFlag = ((flagReg & 0x20) != 0);
		CarryFlag = ((flagReg & 0x10) != 0);
	}

};
