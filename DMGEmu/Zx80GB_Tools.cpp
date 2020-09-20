#include "pch.h"
#include "Zx80GB.h"
#include <iostream>

void Zx80GB::handleIRQ()
{
	if (IME) // WARNING - Maybe this test should be done outside this method in order to save JP
	{
		uint8_t IRQEnable = memMng.readByte(0xFFFF, true);
		uint8_t IRQFlag = memMng.readByte(0xFF0F, true);

		if (IRQEnable && IRQFlag)
		{
			// Check the 5 Source of IRQ :
			for (int i = 0; i < 5; i++)
			{
				uint8_t mask = 1 << i;
				if (((IRQEnable & mask) != 0) && ((IRQFlag & mask) != 0))
				{
					// Raise interruption
					IME = false; // Cannot be interrupted anymore
					IRQFlag = IRQFlag ^ mask; // Reset IRQ
					memMng.writeByte(0xFF0F, IRQFlag, true);
					opCall(0x40 + (0x8 * i));

					if (haltPending)
					{
						haltPending = false;
						std::cout << "Leave Halt" << std::endl;
					}
					break; // Handle IRQ One after another
				}
			}
		}
	}
}

void Zx80GB::handleTimer(int inc)
{
	clock16384Hz += inc;
	if (clock16384Hz >= 256)
	{
		clock16384Hz -= 256;
		uint8_t timer = memMng.readByte(0xff04, true);
		timer++;
		memMng.writeByte(0xff04, timer, true);
	}
}

void Zx80GB::opInc8(__int8 &reg)
{
	_int8 oldB = reg;
	reg++;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();

	if ((oldB & 0x0f) == 0x0f)
		setHalfCarryFlag();
	else
		resetHalfCarryFlag();
}

void Zx80GB::opDec8(__int8 &reg)
{
	__int8 oldB =reg;
	reg--;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	setSubFlag();

	if ((oldB & 0x0f) == 0x00)
		setHalfCarryFlag();
	else
		resetHalfCarryFlag();
}

// Handle OR opcode call with flag management
void Zx80GB::opOr(__int8 reg)
{
	regA = regA | reg;

	if (regA == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();
	resetCarryFlag();
}

// Handle AND opcode call with flag management
void Zx80GB::opAnd(__int8 reg)
{
	regA = regA & reg;

	if (regA == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	setHalfCarryFlag();
	resetCarryFlag();
}

// Handle XOR opcode call with flag management
void Zx80GB::opXor(__int8 reg)
{
	regA = regA ^ reg;
	if (regA == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();
	resetCarryFlag();
}

// Handle BIT opcode call with flag management
// Set Zero flag with the value of bit <bitNumber> from reg 
void Zx80GB::opBit(__int8 bitNumber, __int8 reg)
{
	__int8 bitMask = 1 << bitNumber;
	__int8 res = reg & bitMask;

	if (res == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	setHalfCarryFlag();
}

void Zx80GB::opRes(__int8 bitNumber, _int8  &reg)
{
	__int8 bitMask = 0xFF - (1 << bitNumber);
	reg = reg & bitMask;
}

void Zx80GB::opSet(__int8 bitNumber, _int8  &reg)
{
	__int8 bitMask = 1 << bitNumber;
	reg = reg | bitMask;
}

// Handle RET opcode call
void Zx80GB::opRet()
{
	unsigned char pcH = (unsigned char) memMng.readByte(stackPointer+1);
	unsigned char pcL = (unsigned char) memMng.readByte(stackPointer);

	programCounter = ((uint16_t)pcH << 8) | (((uint16_t)pcL)&0x00ff) ;
	stackPointer = stackPointer + 2;
}

// Handle RST opcode call
void Zx80GB::opRst(__int8 offset)
{
	__int8 pcH = (programCounter & 0xff00) >> 8;
	__int8 pcL = programCounter & 0x00ff;
	memMng.writeByte(stackPointer - 1, pcH);
	memMng.writeByte(stackPointer - 2, pcL);
	programCounter = 0x00ff & offset;
	stackPointer = stackPointer - 2;
}

// Handle Call opcode call
void Zx80GB::opCall(uint16_t addr)
{
	__int8 pcH = (programCounter >> 8) & 0x00ff;
	__int8 pcL = programCounter & 0x00ff;
	memMng.writeByte(stackPointer - 1, pcH);
	memMng.writeByte(stackPointer - 2, pcL);
	stackPointer -= 2;
	programCounter = addr;
}

void Zx80GB::opPush(_int8 h, _int8 l)
{
	memMng.writeByte(stackPointer - 1, h);
	memMng.writeByte(stackPointer - 2, l);
	stackPointer = stackPointer - 2;
}

void Zx80GB::opPop(_int8 &h, _int8 &l)
{
	l = memMng.readByte(stackPointer);
	h = memMng.readByte(stackPointer + 1);
	stackPointer = stackPointer + 2;
}

// Handle SLA opcode call with flag management
void Zx80GB::opSla(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = reg << 1;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();

	if ((oldVal & 0x80) != 0)
		setCarryFlag();
	else
		resetCarryFlag();
}

// Handle RLC opcode call with flag management
void Zx80GB::opRlc(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = reg << 1;

	if ((oldVal & 0x80) != 0)
	{
		setCarryFlag();
		reg = reg | 0x01;
	}
	else
		resetCarryFlag();

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();
}


// Handle RL opcode call with flag management
void Zx80GB::opRl(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = reg << 1;

	if (getCarryFlag())
		reg = reg | 0x01;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();

	if ((oldVal & 0x80) != 0)
		setCarryFlag();
	else
		resetCarryFlag();
}

// Handle SRA opcode call with flag management
void Zx80GB::opSra(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = (reg >> 1) & 0x7F;
	reg = reg | (oldVal & 0x80);

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();

	if ((oldVal & 0x01) != 0)
		setCarryFlag();
	else
		resetCarryFlag();
}

// Handle SRL opcode call with flag management
void Zx80GB::opSrl(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = (reg >> 1) & 0x7F;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();

	if ((oldVal & 0x01) != 0)
		setCarryFlag();
	else
		resetCarryFlag();
}

// Handle RRC opcode call with flag management
void Zx80GB::opRrc(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = (reg >> 1) & 0x7F;

	if ((oldVal & 0x01) != 0)
	{
		setCarryFlag();
		reg = reg | 0x80;
	}
	else
		resetCarryFlag();

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();
}

// Handle RR opcode call with flag management
void Zx80GB::opRr(__int8 &reg)
{
	_int8 oldVal = reg;
	reg = (reg >> 1) & 0x7F;

	if (getCarryFlag())
		reg = reg | 0x80;

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();

	if ((oldVal & 0x01) != 0)
		setCarryFlag();
	else
		resetCarryFlag();
}

// Handle SWAP opcode call with flag management
void  Zx80GB::opSwap(__int8 &reg)
{
	__int8 low = reg & 0x0f;
	reg = (reg >> 4) & 0x0f;
	reg = reg | (low << 4);

	if (reg == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();
	resetHalfCarryFlag();
	resetCarryFlag();
}

// ADD REG, REG
void Zx80GB::opAdd(__int8 &regDest, __int8 regVal)
{
	uint16_t result = (uint8_t)regDest + (uint8_t)regVal;
	regDest = (_int8)(result & 0x00ff);

	bool halfCarry = ((((regDest & 0x0f) + (regVal & 0x0f)) & 0x10) == 0x10);

	if (regDest == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();

	if (halfCarry)
		setHalfCarryFlag();
	else
		resetHalfCarryFlag();

	if (result > 0x00ff)
		setCarryFlag();
	else
		resetCarryFlag();
}

void Zx80GB::opAdd16(uint16_t &regDest, uint16_t regVal)
{
	uint32_t result = regDest + regVal;
	regDest = result & 0xffff;

	resetSubFlag();

	bool halfCarry = ((((regDest & 0xfff) + (regVal & 0xfff)) & 0x1000) == 0x1000);
	if (halfCarry)
		setHalfCarryFlag();
	else
		resetHalfCarryFlag();

	bool carry = ((result & 0x10000) == 0x10000);
	if (carry)
		setCarryFlag();
	else
		resetCarryFlag();
}

void Zx80GB::opAdc(__int8 regVal)
{
	__int16 valA = (__int16)(regA) & 0x00ff;
	__int16 valB = (__int16)(regVal) & 0x00ff;
	__int16 carry = getCarryFlag();
	__int16 result = valA + valB + carry;
	__int8 testHalfCarry = (regA & 0x0f) + (regVal & 0x0f) + (__int8)carry;

	regA = (_int8)(result & 0x00ff);

	if (regA == 0)
		setZeroFlag();
	else
		resetZeroFlag();

	resetSubFlag();

	if (testHalfCarry > 0x0f)
		setHalfCarryFlag();
	else
		resetHalfCarryFlag();

	if (result > 0x00ff)
		setCarryFlag();
	else
		resetCarryFlag();
}

void Zx80GB::opCp(__int8 regVal)
{
	int16_t valA = (int16_t)(regA) & 0x00ff;
	int16_t valB = (int16_t)(regVal) & 0x00ff;
	int16_t result = valA - valB;
	__int8 testHalfCarry = (regA & 0x0f) - (regVal & 0x0f);

	if (((_int8)(result & 0x00ff)) == 0)
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
}
