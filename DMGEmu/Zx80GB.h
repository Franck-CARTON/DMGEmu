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
	uint8_t regA;
	uint8_t regB;
	uint8_t regC;
	uint8_t regD;
	uint8_t regE;
	uint8_t regL;
	uint8_t regH;

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
	void opAdd(uint8_t&regDest, uint8_t regVal);
	void opAdd16(uint16_t &regDest, uint16_t regVal);
	void opAdc(uint8_t regVal);
	void opInc8(uint8_t&reg);
	void opDec8(uint8_t&reg);
	void opOr(uint8_t reg);
	void opAnd(uint8_t reg);
	void opXor(uint8_t reg);
	void opRet();
	void opRst(uint8_t offset);
	void opRes(uint8_t bitNumber, uint8_t&reg);
	void opBit(uint8_t bitNumber, uint8_t reg);
	void opSet(uint8_t bitNumber, uint8_t&reg);
	void opCall(uint16_t addr);
	void opPush(uint8_t h, uint8_t l);
	void opPop(uint8_t&h, uint8_t&l);
	void opSla(uint8_t&reg);
	void opRlc(uint8_t&reg);
	void opRl(uint8_t&eg);
	void opSra(uint8_t&reg);
	void opSrl(uint8_t&reg);
	void opRrc(uint8_t&reg);
	void opRr(uint8_t&reg);
	void opSwap(uint8_t&reg);
	void opCp(uint8_t regVal);
	void opSub(uint8_t regVal);


	inline uint16_t getHLReg()
	{
		uint16_t val = regH << 8 | regL;
		return val;
	}

	inline void setHLReg(uint16_t val)
	{
		regH = val >> 8;
		regL = val;
	}

	inline uint16_t getBCReg()
	{
		uint16_t val = regB << 8 | regC;
		return val;
	}

	inline void setBCReg(uint16_t val)
	{
		regB = val >> 8;
		regC = val;
	}

	// Get register D & E as a 16bits register
	inline uint16_t getDEReg()
	{
		uint16_t val = regD << 8 | regE;
		return val;
	}

	// Set register D & E as a 16bits register
	inline void setDEReg(uint16_t val)
	{
		regD = val >> 8;
		regE = val;
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
	/*
	inline int sub(uint8_t regVal)
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
	*/
	inline int sbc(uint8_t regVal)
	{
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

	inline void setFlagRegister(uint8_t flagReg)
	{
		ZeroFlag = ((flagReg & 0x80) != 0);
		SubFlag = ((flagReg & 0x40) != 0);
		HalfCarryFlag = ((flagReg & 0x20) != 0);
		CarryFlag = ((flagReg & 0x10) != 0);
	}

};
