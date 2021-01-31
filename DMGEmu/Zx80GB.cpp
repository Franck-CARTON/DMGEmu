#include "pch.h"
#include "Zx80GB.h"

#include <iostream>
#include <sstream>


Zx80GB::Zx80GB(memoryManager& memRef) : memMng(memRef)
{
	stackPointer = 0;
	programCounter = 0;

	// DMG Zx80 flag register :
	regA = 0;
	regB = 0;
	regC = 0;
	regD = 0;
	regE = 0;
	regL = 0;
	regH = 0;

	// Bursted Flag register :
	ZeroFlag = false;
	SubFlag = false;
	HalfCarryFlag = false;
	CarryFlag = false;

	// Cpu status :
	haltPending = false;
	isStopped = false;

	// Init interruption flag & register :
	IME = false;
	memMng.writeByte(0xFFFF, 0, true);
	memMng.writeByte(0xFF0F, 0, true);
}

Zx80GB::~Zx80GB()
{
}

int Zx80GB::runZx80(bool debug)
{
	int durationInCycles = 1;
	opcodeRead.erase();
	handleIRQ();

	// Behave according to CPU Status :
	if (isStopped)
	{
		// Check keyboard :
		uint8_t input = memMng.readByte(0xff00, true);
		if ((input & 0x0f) != 0x00)
		{
			isStopped = false;
		}
	}

	if (haltPending)
	{
		durationInCycles += 4;
	}

	if (!isStopped && !haltPending)
	{
		durationInCycles = readOpcode(debug);
	}

	return durationInCycles;
}

// #TODO : Should return the number of elapsed tick instead of incrementing a internal counter with no other use
int Zx80GB::readOpcode(bool debug)
{
	int durationInCycles = 0;
	int prefixHandlingDuration = 0;

	unsigned char opcode = memMng.readByte(programCounter);
	switch (opcode)
	{
	case 0x00: // NOP
		programCounter++;
		if (debug)
			opcodeRead += std::string("NOP");
		durationInCycles = 4;
		break;
	case 0x40: // LD B,B
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD B, B");
		durationInCycles = 4;
		break;
	case 0x49: // LD C,C
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD C, C");
		durationInCycles = 4;
		break;
	case 0x52: // LD D,D
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD D, D");
		durationInCycles = 4;
		break;
	case 0x5B: // LD E,E
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD E, E");
		durationInCycles = 4;
		break;
	case 0x64: // LD H,H
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD H, H");
		durationInCycles = 4;
		break;
	case 0x6D: // LD L,L
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD L, L");
		durationInCycles = 4;
		break;
	case 0x7F: // LD A,A
		programCounter++;
		if (debug)
			opcodeRead += std::string("LD A, A");
		durationInCycles = 4;
		break;
	case 0x01: // LD BC,d16
		programCounter++;
		// Mot de poid faible en tête
		regC = memMng.readByte(programCounter++);
		regB = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)getBCReg();
			opcodeRead += "LD BC, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	case 0x02: // LD (BC), A
	{
		programCounter++;
		uint16_t addr = getBCReg();
		memMng.writeByte(addr, regA);
		if (debug)
			opcodeRead += std::string("LD (BC), A");
		durationInCycles = 8;
		break;
	}
	case 0x03: // INC BC
	{
		programCounter++;
		uint16_t value = getBCReg();
		value++;
		setBCReg(value);
		if (debug)
			opcodeRead += "INC BC";
		durationInCycles = 8;
		break;
	}
	case 0x04: // INC B
		programCounter++;
		opInc8(regB);
		if (debug)
			opcodeRead += std::string("INC B");
		durationInCycles = 4;
		break;
	case 0x05 : // DEC B
		programCounter++;
		opDec8(regB);
		if (debug)
			opcodeRead += std::string("DEC B");
		durationInCycles = 4;
		break;
	case 0x06 : // LD B, d8
		programCounter++;
		regB = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regB;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD B, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x07: // RLCA
	{
		programCounter++;
		uint8_t oldA = regA;
		regA = regA << 1;

		// Handle Carry flag
		if ((oldA & 0x80) == 0x80)
		{
			regA = regA + 1;
			setCarryFlag();
		}
		else
			resetCarryFlag();

		resetZeroFlag();
		resetSubFlag();
		resetHalfCarryFlag();

		if (debug)
			opcodeRead += std::string("RLCA");

		durationInCycles = 4;
		break;
	}
	case 0x08 : // LD (a16),SP
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = addrH << 8 | addrL;
		memMng.writeByte(addr, (uint8_t)stackPointer);
		memMng.writeByte(addr+1, (uint8_t)(stackPointer >> 8));
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "LD (";
			opcodeRead += stream.str();
			opcodeRead += "), SP";
		}
		durationInCycles = 20;
		break;
	}
	case 0x09: // AD HL, BC
	{
		programCounter++;
		uint16_t hl = getHLReg();
		uint16_t de = getBCReg();
		opAdd16(hl, de);
		setHLReg(hl);
		if (debug)
			opcodeRead += "AD HL, BC";
		durationInCycles = 8;
		break;
	}
	case 0x0A: // LD A, (BC)
	{
		programCounter++;
		uint16_t addr = getBCReg();
		regA = memMng.readByte(addr);
		if (debug)
			opcodeRead += std::string("LD A, (BC)");
		durationInCycles = 8;
		break;
	}
	case 0x0B: // DEC BC
	{
		programCounter++;
		uint16_t bc = getBCReg();
		bc--;
		setBCReg(bc);
		if (debug)
			opcodeRead += std::string("DEC BC");
		durationInCycles = 8;
		break;
	}
	case 0x0C: // INC C
		programCounter++;
		opInc8(regC);
		if (debug)
			opcodeRead += std::string("INC C");
		durationInCycles = 4;
		break;
	case 0x0D: // DEC C
		programCounter++;
		opDec8(regC);
		if (debug)
			opcodeRead += std::string("DEC C");
		durationInCycles = 4;
		break;
	case 0x0E: // LD C, d8
		programCounter++;
		regC = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regC;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD C, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x0F : // RRCA
	{
		programCounter++;
		_int8 oldVal = regA;
		regA = (regA >> 1) & 0x7F;

		if ((oldVal & 0x01) != 0)
		{
			setCarryFlag();
			regA = (regA) | 0x80;
		}
		else
			resetCarryFlag();

		resetZeroFlag();
		resetSubFlag();
		resetHalfCarryFlag();

		if (debug)
			opcodeRead += "RRCA";

		durationInCycles = 4;
		break;
	}
	case 0x10: // STOP
	{
		isStopped = true;
		programCounter++;
		programCounter++;
		if (debug)
			opcodeRead += "STOP";

		durationInCycles = 4;
		break;
	}
	case 0x11: // LD DE,d16
		programCounter++;
		regE = memMng.readByte(programCounter++);
		regD = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)getDEReg();
			opcodeRead += "LD DE, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	case 0x12: // LD (DE), A
	{
		programCounter++;
		uint16_t addr = ((regD << 8) | regE );
		memMng.writeByte(addr, regA);
		if (debug)
			opcodeRead += "LD (DE), A";
		durationInCycles = 8;
		break;
	}
	case 0x13: // INC DE
	{
		programCounter++;
		__int16 value = getDEReg();
		value++;
		setDEReg(value);
		if (debug)
			opcodeRead += "INC DE";
		durationInCycles = 8;
		break;
	}
	case 0x14: // INC D
		programCounter++;
		opInc8(regD);
		if (debug)
			opcodeRead += "INC D";
		durationInCycles = 4;
		break;
	case 0x15: // DEC D
		programCounter++;
		opDec8(regD);
		if (debug)
			opcodeRead += "DEC D";
		durationInCycles = 4;
		break;
	case 0x16: // LD D, d8
		programCounter++;
		regD = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regD;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD D, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x17: // RLA
	{
		programCounter++;
		_int8 oldA = regA; // TODO Recheck validity (_int8 or uint8_t ?)
		regA = regA << 1;

		if (getCarryFlag())
			regA++;

		// Handle Carry flag
		if ((oldA & 0x80) == 0x80)
			setCarryFlag();
		else
			resetCarryFlag();

		resetZeroFlag();
		resetSubFlag();
		resetHalfCarryFlag();

		if (debug)
			opcodeRead += "RLA";

		durationInCycles = 4;
		break;
	}
	case 0x18: // JR d8
	{
		programCounter++;
		int8_t val = (int8_t)memMng.readByte(programCounter++);
		programCounter += (uint16_t)val;
		if (debug)
		{
			uint8_t val2Disp = (uint8_t)val;
			std::stringstream stream;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "JR ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	}

	case 0x19: // AD HL, DE
	{
		programCounter++;
		uint16_t hl = getHLReg();
		uint16_t de = getDEReg();
		opAdd16(hl, de);
		setHLReg(hl);
		if (debug)
			opcodeRead += "AD HL, DE";
		durationInCycles = 8;
		break;
	}
	case 0x1A: // LD A, (DE)
	{
		programCounter++;
		uint16_t addr = getDEReg();
		regA = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD A, (DE)";
		durationInCycles = 8;
		break;
	}
	case 0x1B: // DEC DE
	{
		programCounter++;
		uint16_t de = getDEReg();
		de--;
		setDEReg(de);
		if (debug)
			opcodeRead += std::string("DEC DE");
		durationInCycles = 8;
		break;
	}
	case 0x1C: // INC E
		programCounter++;
		opInc8(regE);
		if (debug)
			opcodeRead += "INC E";
		durationInCycles = 4;
		break;
	case 0x1D: // DEC E
		programCounter++;
		opDec8(regE);
		if (debug)
			opcodeRead += "DEC E";
		durationInCycles = 4;
		break;
	case 0x1E: // LD E, d8
		programCounter++;
		regE = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regE;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD E, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x1F : // RRA
	{
		programCounter++;
		_int8 oldVal = regA;
		regA = ((regA) >> 1) & 0x7F;

		if (getCarryFlag())
			regA = regA | 0x80;

		resetZeroFlag();
		resetSubFlag();
		resetHalfCarryFlag();

		if ((oldVal & 0x01) != 0)
			setCarryFlag();
		else
			resetCarryFlag();

		if (debug)
			opcodeRead += "RRA";

		durationInCycles = 4;
		break;
	}
	case 0x20: // JR NZ, d8
	{
		programCounter++;
		int8_t val = (int8_t)memMng.readByte(programCounter++);
		if (getZeroFlag())
			durationInCycles = 8;
		else
		{
			programCounter += (__int16)val;
			durationInCycles = 12;
		}
		if (debug)
		{
			unsigned char val2Disp = (unsigned char)val;
			std::stringstream stream;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "JR NZ, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0x21: // LD HL,d16
	{
		programCounter++;
		// Mot de poid faible en tête
		regL = memMng.readByte(programCounter++);
		regH = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)getHLReg();
			opcodeRead += "LD HL, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	}
	case 0x22: // LD (HL+), A
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regA);
		setHLReg(++addr);
		if (debug)
			opcodeRead += "LD (HL+), A";
		durationInCycles = 8;
		break;
	}
	case 0x23: // INC HL
	{
		programCounter++;
		uint16_t value = getHLReg();
		value++;
		setHLReg(value);
		if (debug)
			opcodeRead += "INC HL";
		durationInCycles = 8;
		break;
	}
	case 0x24: // INC H
		programCounter++;
		opInc8(regH);
		if (debug)
			opcodeRead += "INC H";
		durationInCycles = 4;
		break;
	case 0x25: // DEC H
		programCounter++;
		opDec8(regH);
		if (debug)
			opcodeRead += "DEC H";
		durationInCycles = 4;
		break;
	case 0x26: // LD H, d8
		programCounter++;
		regH = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regH;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD H, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x27: // DAA
		programCounter++;
		
		if (!getSubFlag())
		{
			if (getCarryFlag() || regA > 0x99) { regA += 0x60; setCarryFlag(); }
			if (getHalfCarryFlag() || (regA & 0x0f)> 0x09) { regA += 0x6; }
		}
		else
		{
			if (getCarryFlag()) { regA -= 0x60; }
			if (getHalfCarryFlag()) { regA -= 0x6; }
		}

		if (regA == 0) { setZeroFlag(); }
		else { resetZeroFlag(); }

		resetHalfCarryFlag();

		if (debug)
		{
			opcodeRead += "DAA";
		}
		durationInCycles = 4;

		break;
	case 0x28: // JR Z, d8
	{
		programCounter++;
		__int8 val = memMng.readByte(programCounter++);
		if (!getZeroFlag())
			durationInCycles = 8;
		else
		{
			programCounter += (__int16)val;
			durationInCycles = 12;
		}
		if (debug)
		{
			uint8_t val2Disp = (uint8_t)val;
			std::stringstream stream;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "JR Z, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0x29: // AD HL, HL
	{
		programCounter++;
		uint16_t hl = getHLReg();
		uint16_t de = getHLReg();
		opAdd16(hl, de);
		setHLReg(hl);
		if (debug)
			opcodeRead += "AD HL, HL";
		durationInCycles = 8;
		break;
	}
	case 0x2A: // LD A, (HL+)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regA = memMng.readByte(addr);
		setHLReg(++addr);
		if (debug)
			opcodeRead += "LD A, (HL+)";
		durationInCycles = 8;
		break;
	}
	case 0x2B: // DEC HL
	{
		programCounter++;
		uint16_t hl = getHLReg();
		hl--;
		setHLReg(hl);
		if (debug)
			opcodeRead += std::string("DEC HL");
		durationInCycles = 8;
		break;
	}
	case 0x2C: // INC L
		programCounter++;
		opInc8(regL);
		if (debug)
			opcodeRead += "INC L";
		durationInCycles = 4;
		break;
	case 0x2D: // DEC L
		programCounter++;
		opDec8(regL);
		if (debug)
			opcodeRead += "DEC L";
		durationInCycles = 4;
		break;
	case 0x2E: // LD L, d8
		programCounter++;
		regL = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)regL;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD L, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x2F: // CPL
		programCounter++;
		regA = regA ^ 0xFF;
		setSubFlag();
		setHalfCarryFlag();
		if (debug)
			opcodeRead += "CPL";
		durationInCycles = 4;
		break;
	case 0x30: // JR NC, d8
	{
		programCounter++;
		__int8 val = memMng.readByte(programCounter++);
		if (getCarryFlag())
			durationInCycles = 8;
		else
		{
			programCounter += (__int16)val;
			durationInCycles = 12;
		}
		if (debug)
		{
			uint8_t val2Disp = (uint8_t)val;
			std::stringstream stream;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "JR NC, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0x31: // LD SP, d16
	{
		programCounter++;
		uint8_t valL = memMng.readByte(programCounter++);
		uint8_t  valH = memMng.readByte(programCounter++);
		stackPointer = (valH << 8) | valL;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)stackPointer;
			opcodeRead += "LD SP, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	}
	case 0x32: // LD (HL-), A
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regA);
		setHLReg(--addr);
		if (debug)
			opcodeRead += "LD (HL-), A";
		durationInCycles = 8;
		break;
	}
	case 0x33: // INC SP
		programCounter++;
		stackPointer++;
		if (debug)
			opcodeRead += "INC SP";
		durationInCycles = 8;
		break;
	case 0x34: // INC (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value = memMng.readByte(addr);
		opInc8(value);
		memMng.writeByte(addr, value);
		if (debug)
			opcodeRead += "INC (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x35: // DEC (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value= memMng.readByte(addr);
		opDec8(value);
		memMng.writeByte(addr, value);
		if (debug)
			opcodeRead += "DEC (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x36: // LD (HL), d8
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value = memMng.readByte(programCounter++);
		memMng.writeByte(addr, value);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val = (unsigned char)value;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD (HL), ";
			opcodeRead += stream.str();
		}
		durationInCycles = 12;
		break;
	}
	case 0x37 : // SCF
	{
		programCounter++;
		setCarryFlag();
		resetSubFlag();
		resetHalfCarryFlag();
		setCarryFlag();
		if (debug)
			opcodeRead += "SCF";
		durationInCycles = 4;
		break;
	}
	case 0x38: // JR C, d8
	{
		programCounter++;
		__int8 val = memMng.readByte(programCounter++);
		if (!getCarryFlag())
			durationInCycles = 8;
		else
		{
			programCounter += (__int16)val;
			durationInCycles = 12;
		}
		if (debug)
		{
			uint8_t val2Disp = (uint8_t)val;
			std::stringstream stream;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "JR C, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0x39: // AD HL, SP
	{
		programCounter++;
		uint16_t hl = getHLReg();
		opAdd16(hl, stackPointer);
		setHLReg(hl);
		if (debug)
			opcodeRead += "AD HL, SP";
		durationInCycles = 8;
		break;
	}
	case 0x3A: // LD A, (HL-)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regA = memMng.readByte(addr);
		setHLReg(--addr);
		if (debug)
			opcodeRead += "LD A, (HL-)";
		durationInCycles = 8;
		break;
	}
	case 0x3B: // DEC SP
		programCounter++;
		stackPointer--;
		if (debug)
			opcodeRead += "DEC SP";
		durationInCycles = 8;
		break;
	case 0x3C: // INC A
		programCounter++;
		opInc8(regA);
		if (debug)
			opcodeRead += "INC A";
		durationInCycles = 4;
		break;
	case 0x3D: // DEC A
		programCounter++;
		opDec8(regA);
		if (debug)
			opcodeRead += "DEC A";
		durationInCycles = 4;
		break;
	case 0x3E: // LD A, d8
		programCounter++;
		regA = memMng.readByte(programCounter++);
		if (debug)
		{
			std::stringstream stream;
			uint8_t val = (uint8_t)regA;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "LD A, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	case 0x3F: // CCF
		programCounter++;
		if (getCarryFlag())
			resetCarryFlag();
		else
			setCarryFlag();

		resetSubFlag();
		resetHalfCarryFlag();
		durationInCycles = 4;
		if (debug)
			opcodeRead += "CCF";
		break;
	case 0x41 : // LD B,C
		programCounter++;
		regB = regC;
		if (debug)
			opcodeRead += "LD B, C";
		durationInCycles = 4;
		break;
	case 0x42: // LD B,D
		programCounter++;
		regB = regD;
		if (debug)
			opcodeRead += "LD B, D";
		durationInCycles = 4;
		break;
	case 0x43: // LD B,E
		programCounter++;
		regB = regE;
		if (debug)
			opcodeRead += "LD B, E";
		durationInCycles = 4;
		break;
	case 0x44: // LD B,H
		programCounter++;
		regB = regH;
		if (debug)
			opcodeRead += "LD B, H";
		durationInCycles = 4;
		break;
	case 0x45: // LD B,L
		programCounter++;
		regB = regL;
		if (debug)
			opcodeRead += "LD B, L";
		durationInCycles = 4;
		break;
	case 0x46: // LD B,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regB = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD B, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x47: // LD B,A
		programCounter++;
		regB = regA;
		if (debug)
			opcodeRead += "LD B, A";
		durationInCycles = 4;
		break;
	case 0x48: // LD C,B
		programCounter++;
		regC = regB;
		if (debug)
			opcodeRead += "LD C, B";
		durationInCycles = 4;
		break;
	case 0x4A: // LD C,D
		programCounter++;
		regC = regD;
		if (debug)
			opcodeRead += "LD C, D";
		durationInCycles = 4;
		break;
	case 0x4B: // LD C,E
		programCounter++;
		regC = regE;
		if (debug)
			opcodeRead += "LD C, E";
		durationInCycles = 4;
		break;
	case 0x4C: // LD C,H
		programCounter++;
		regC = regH;
		if (debug)
			opcodeRead += "LD C, H";
		durationInCycles = 4;
		break;
	case 0x4D: // LD C,L
		programCounter++;
		regC = regL;
		if (debug)
			opcodeRead += "LD C, L";
		durationInCycles = 4;
		break;
	case 0x4E: // LD C,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regC = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD C, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x4F : // LD C,A
		programCounter++;
		regC = regA;
		if (debug)
			opcodeRead += "LD C, A";
		durationInCycles = 4;
		break;
	case 0x50 : // LD D,B
		programCounter++;
		regD = regB;
		if (debug)
			opcodeRead += "LD D, B";
		durationInCycles = 4;
		break;
	case 0x51 : // LD D,C
		programCounter++;
		regD = regC;
		if (debug)
			opcodeRead += "LD D, C";
		durationInCycles = 4;
		break;
	case 0x53: // LD D,E
		programCounter++;
		regD = regE;
		if (debug)
			opcodeRead += "LD D, E";
		durationInCycles = 4;
		break;
	case 0x54: // LD D,H
		programCounter++;
		regD = regH;
		if (debug)
			opcodeRead += "LD D, H";
		durationInCycles = 4;
		break;
	case 0x55: // LD D,L
		programCounter++;
		regD = regL;
		if (debug)
			opcodeRead += "LD D, L";
		durationInCycles = 4;
		break;
	case 0x56: // LD D,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regD = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD D, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x57: // LD D,A
		programCounter++;
		regD = regA;
		if (debug)
			opcodeRead += "LD D, A";
		durationInCycles = 4;
		break;
	case 0x58: // LD E,B
		programCounter++;
		regE = regB;
		if (debug)
			opcodeRead += "LD E, B";
		durationInCycles = 4;
		break;
	case 0x59: // LD E,C
		programCounter++;
		regE = regC;
		if (debug)
			opcodeRead += "LD E, C";
		durationInCycles = 4;
		break;
	case 0x5A : // LD E,D
		programCounter++;
		regE = regD;
		if (debug)
			opcodeRead += "LD E, D";
		durationInCycles = 4;
		break;
	case 0x5C : // LD E,H
		programCounter++;
		regE = regH;
		if (debug)
			opcodeRead += "LD E, H";
		durationInCycles = 4;
		break;
	case 0x5D : // LD E,L
		programCounter++;
		regE = regL;
		if (debug)
			opcodeRead += "LD E, L";
		durationInCycles = 4;
		break;
	case 0x5E: // LD E,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regE = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD E, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x5F: // LD E,A
		programCounter++;
		regE = regA;
		if (debug)
			opcodeRead += "LD E, A";
		durationInCycles = 4;
		break;
	case 0x60: // LD H,B
		programCounter++;
		regH = regB;
		if (debug)
			opcodeRead += "LD H, B";
		durationInCycles = 4;
		break;
	case 0x61: // LD H,C
		programCounter++;
		regH = regC;
		if (debug)
			opcodeRead += "LD H, C";
		durationInCycles = 4;
		break;
	case 0x62: // LD H,D
		programCounter++;
		regH = regD;
		if (debug)
			opcodeRead += "LD H, D";
		durationInCycles = 4;
		break;
	case 0x63: // LD H,E
		programCounter++;
		regH = regE;
		if (debug)
			opcodeRead += "LD H, E";
		durationInCycles = 4;
		break;
	case 0x65: // LD H,L
		programCounter++;
		regH = regL;
		if (debug)
			opcodeRead += "LD H, L";
		durationInCycles = 4;
		break;
	case 0x66: // LD H,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regH = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD H, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x67: // LD H,A
		programCounter++;
		regH = regA;
		if (debug)
			opcodeRead += "LD H, A";
		durationInCycles = 4;
		break;
	case 0x68: // LD L,B
		programCounter++;
		regL = regB;
		if (debug)
			opcodeRead += "LD L, B";
		durationInCycles = 4;
		break;
	case 0x69: // LD L,C
		programCounter++;
		regL = regC;
		if (debug)
			opcodeRead += "LD L, C";
		durationInCycles = 4;
		break;
	case 0x6A : // LD L,D
		programCounter++;
		regL = regD;
		if (debug)
			opcodeRead += "LD L, D";
		durationInCycles = 4;
		break;
	case 0x6B : // LD L,E
		programCounter++;
		regL = regE;
		if (debug)
			opcodeRead += "LD L, E";
		durationInCycles = 4;
		break;
	case 0x6C : // LD L,H
		programCounter++;
		regL = regH;
		if (debug)
			opcodeRead += "LD L, H";
		durationInCycles = 4;
		break;
	case 0x6E : // LD L,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regL = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD L, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x6F : // LD L,A
		programCounter++;
		regL = regA;
		if (debug)
			opcodeRead += "LD L, A";
		durationInCycles = 4;
		break;
	case 0x70 : // LD (HL), B
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regB);
		if (debug)
			opcodeRead += "LD (HL), B";
		durationInCycles = 8;
		break;
	}
	case 0x71: // LD (HL), C
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regC);
		if (debug)
			opcodeRead += "LD (HL), C";
		durationInCycles = 8;
		break;
	}
	case 0x72: // LD (HL), D
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regD);
		if (debug)
			opcodeRead += "LD (HL), D";
		durationInCycles = 8;
		break;
	}
	case 0x73: // LD (HL), E
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regE);
		if (debug)
			opcodeRead += "LD (HL), E";
		durationInCycles = 8;
		break;
	}
	case 0x74: // LD (HL), H
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regH);
		if (debug)
			opcodeRead += "LD (HL), H";
		durationInCycles = 8;
		break;
	}
	case 0x75: // LD (HL), L
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regL);
		if (debug)
			opcodeRead += "LD (HL), L";
		durationInCycles = 8;
		break;
	}
	case 0x76: // HALT
	{
		programCounter++;
		haltPending = true;
		//std::cout << "Enter Halt - " << programCounter << std::endl;
		//std::cout << "HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT HALT" << std::endl;
		/*haltPending = true;
		if (debug)
			opcodeRead += "HALT";
		durationInCycles = 4;*/
		break;
	}
	case 0x77: // LD (HL), A
	{
		programCounter++;
		uint16_t addr = getHLReg();
		memMng.writeByte(addr, regA);
		if (debug)
			opcodeRead += "LD (HL), A";
		durationInCycles = 8;
		break;
	}
	case 0x78 : // LD A,B
		programCounter++;
		regA = regB;
		if (debug)
			opcodeRead += "LD A, B";
		durationInCycles = 4;
		break;
	case 0x79 : // LD A,C
		programCounter++;
		regA = regC;
		if (debug)
			opcodeRead += "LD A, C";
		durationInCycles = 4;
		break;
	case 0x7A : // LD A,D
		programCounter++;
		regA = regD;
		if (debug)
			opcodeRead += "LD A, D";
		durationInCycles = 4;
		break;
	case 0x7B : // LD A,E
		programCounter++;
		regA = regE;
		if (debug)
			opcodeRead += "LD A, E";
		durationInCycles = 4;
		break;
	case 0x7C : // LD A,H
		programCounter++;
		regA = regH;
		if (debug)
			opcodeRead += "LD A, H";
		durationInCycles = 4;
		break;
	case 0x7D : // LD A,L
		programCounter++;
		regA = regL;
		if (debug)
			opcodeRead += "LD A, L";
		durationInCycles = 4;
		break;
	case 0x7E : // LD A,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		regA = memMng.readByte(addr);
		if (debug)
			opcodeRead += "LD A, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x80 :// ADD A, B
		programCounter++;
		opAdd(regA, regB);
		if (debug)
			opcodeRead += "ADD A, B";
		durationInCycles = 4;
		break;
	case 0x81:// ADD A, C
		programCounter++;
		opAdd(regA, regC);
		if (debug)
			opcodeRead += "ADD A, C";
		durationInCycles = 4;
		break;
	case 0x82:// ADD A, D
		programCounter++;
		opAdd(regA, regD);
		if (debug)
			opcodeRead += "ADD A, D";
		durationInCycles = 4;
		break;
	case 0x83:// ADD A, E
		programCounter++;
		opAdd(regA, regE);
		if (debug)
			opcodeRead += "ADD A, E";
		durationInCycles = 4;
		break;
	case 0x84:// ADD A, H
		programCounter++;
		opAdd(regA, regH);
		if (debug)
			opcodeRead += "ADD A, H";
		durationInCycles = 4;
		break;
	case 0x85:// ADD A, L
		programCounter++;
		opAdd(regA, regL);
		if (debug)
			opcodeRead += "ADD A, L";
		durationInCycles = 4;
		break;
	case 0x86:// ADD A, (HL)
	{
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		programCounter++;
		opAdd(regA, val);
		if (debug)
			opcodeRead += "ADD A, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x87:// ADD A, A
		programCounter++;
		opAdd(regA, regA);
		if (debug)
			opcodeRead += "ADD A, A";
		durationInCycles = 4;
		break;
	case 0x88:// ADC A, B
		programCounter++;
		opAdc(regB);
		if (debug)
			opcodeRead += "ADC A, B";
		durationInCycles = 4;
		break;
	case 0x89:// ADC A, C
		programCounter++;
		opAdc(regC);
		if (debug)
			opcodeRead += "ADC A, C";
		durationInCycles = 4;
		break;
	case 0x8A:// ADC A, D
		programCounter++;
		opAdc(regD);
		if (debug)
			opcodeRead += "ADC A, D";
		durationInCycles = 4;
		break;
	case 0x8B:// ADC A, E
		programCounter++;
		opAdc(regE);
		if (debug)
			opcodeRead += "ADC A, E";
		durationInCycles = 4;
		break;
	case 0x8C:// ADC A, H
		programCounter++;
		opAdc(regH);
		if (debug)
			opcodeRead += "ADC A, H";
		durationInCycles = 4;
		break;
	case 0x8D:// ADC A, L
		programCounter++;
		opAdc(regL);
		if (debug)
			opcodeRead += "ADC A, L";
		durationInCycles = 4;
		break;
	case 0x8E:// ADC A, (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opAdc(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "ADC A, (HL)";
		durationInCycles = 8;
		break;
	}
	case 0x8F:// ADC A, A
		programCounter++;
		opAdc(regA);
		if (debug)
			opcodeRead += "ADC A, A";
		durationInCycles = 4;
		break;
	case 0x90:// SUB A, B
		programCounter++;
		opSub(regB);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, B";
		break;
	case 0x91:// SUB A, C
		programCounter++;
		opSub(regC);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, C";
		break;
	case 0x92:// SUB A, D
		programCounter++;
		opSub(regD);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, D";
		break;
	case 0x93:// SUB A, E
		programCounter++;
		opSub(regE);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, E";
		break;
	case 0x94:// SUB A, H
		programCounter++;
		opSub(regH);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, H";
		break;
	case 0x95:// SUB A, L
		programCounter++;
		opSub(regL);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, L";
		break;
	case 0x96:// SUB A, (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opSub(val);
		durationInCycles = 8;
		if (debug)
			opcodeRead += "SUB A, (HL)";
		break;
	}
	case 0x97:// SUB A, A
		programCounter++;
		opSub(regA);
		durationInCycles = 4;
		if (debug)
			opcodeRead += "SUB A, A";
		break;
	case 0x98:// SBC A, B
		programCounter++;
		durationInCycles = sbc(regB);
		if (debug)
			opcodeRead += "SBC A, B";
		break;
	case 0x99:// SBC A, C
		programCounter++;
		durationInCycles = sbc(regC);
		if (debug)
			opcodeRead += "SBC A, C";
		break;
	case 0x9A:// SBC A, D
		programCounter++;
		durationInCycles = sbc(regD);
		if (debug)
			opcodeRead += "SBC A, D";
		break;
	case 0x9B:// SBC A, E
		programCounter++;
		durationInCycles = sbc(regE);
		if (debug)
			opcodeRead += "SBC A, E";
		break;
	case 0x9C:// SBC A, H
		programCounter++;
		durationInCycles = sbc(regH);
		if (debug)
			opcodeRead += "SBC A, H";
		break;
	case 0x9D:// SBC A, L
		programCounter++;
		durationInCycles = sbc(regL);
		if (debug)
			opcodeRead += "SBC A, L";
		break;
	case 0x9E:// SBC A, (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value = memMng.readByte(addr);
		durationInCycles = sbc(value);
		durationInCycles += 4;
		memMng.writeByte(addr, value);
		if (debug)
			opcodeRead += "SBC A, (HL)";
		break;
	}
	case 0x9F:// SBC A, A
		programCounter++;
		durationInCycles = sbc(regA);
		if (debug)
			opcodeRead += "SBC A, A";
		break;
	case 0xA0 : // AND B
		programCounter++;
		opAnd(regB);
		if (debug)
			opcodeRead += "AND B";
		durationInCycles = 4;
		break;
	case 0xA1: // AND C
		programCounter++;
		opAnd(regC);
		if (debug)
			opcodeRead += "AND C";
		durationInCycles = 4;
		break;
	case 0xA2: // AND D
		programCounter++;
		opAnd(regD);
		if (debug)
			opcodeRead += "AND D";
		durationInCycles = 4;
		break;
	case 0xA3: // AND E
		programCounter++;
		opAnd(regE);
		if (debug)
			opcodeRead += "AND E";
		durationInCycles = 4;
		break;
	case 0xA4: // AND H
		programCounter++;
		opAnd(regH);
		if (debug)
			opcodeRead += "AND H";
		durationInCycles = 4;
		break;
	case 0xA5: // AND L
		programCounter++;
		opAnd(regL);
		if (debug)
			opcodeRead += "AND L";
		durationInCycles = 4;
		break;
	case 0xA6: // AND (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opAnd(val);
		if (debug)
			opcodeRead += "AND (HL)";
		durationInCycles = 8;
		break;
	}
	case 0xA7: // AND A
		programCounter++;
		opAnd(regA);
		if (debug)
			opcodeRead += "AND A";
		durationInCycles = 4;
		break;
	case 0xA8: // XOR B
		programCounter++;
		opXor(regB);
		if (debug)
			opcodeRead += "XOR B";
		durationInCycles = 4;
		break;
	case 0xA9: // XOR C
		programCounter++;
		opXor(regC);
		if (debug)
			opcodeRead += "XOR C";
		durationInCycles = 4;
		break;
	case 0xAA: // XOR D
		programCounter++;
		opXor(regD);
		if (debug)
			opcodeRead += "XOR D";
		durationInCycles = 4;
		break;
	case 0xAB: // XOR E
		programCounter++;
		opXor(regE);
		if (debug)
			opcodeRead += "XOR E";
		durationInCycles = 4;
		break;
	case 0xAC: // XOR H
		programCounter++;
		opXor(regH);
		if (debug)
			opcodeRead += "XOR H";
		durationInCycles = 4;
		break;
	case 0xAD: // XOR L
		programCounter++;
		opXor(regL);
		if (debug)
			opcodeRead += "XOR L";
		durationInCycles = 4;
		break;
	case 0xAE: // XOR (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opXor(val);
		if (debug)
			opcodeRead += "XOR (HL)";
		durationInCycles = 8;
		break;
	}
	case 0xAF: // XOR A
		opcodeRead += "XOR A";
		programCounter++;
		opXor(regA);
		durationInCycles = 4;
		break;
	case 0xB0: // OR B
		programCounter++;
		opOr(regB);
		if (debug)
			opcodeRead += "OR B";
		durationInCycles = 4;
		break;
	case 0xB1: // OR C
		programCounter++;
		opOr(regC);
		if (debug)
			opcodeRead += "OR C";
		durationInCycles = 4;
		break;
	case 0xB2: // OR D
		programCounter++;
		opOr(regD);
		if (debug)
			opcodeRead += "OR D";
		durationInCycles = 4;
		break;
	case 0xB3: // OR E
		programCounter++;
		opOr(regE);
		if (debug)
			opcodeRead += "OR E";
		durationInCycles = 4;
		break;
	case 0xB4: // OR H
		programCounter++;
		opOr(regH);
		if (debug)
			opcodeRead += "OR H";
		durationInCycles = 4;
		break;
	case 0xB5: // OR L
		programCounter++;
		opOr(regL);
		if (debug)
			opcodeRead += "OR L";
		durationInCycles = 4;
		break;
	case 0xB6: // OR (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opOr(val);
		if (debug)
			opcodeRead += "OR (HL)";
		durationInCycles = 8;
		break;
	}
	case 0xB7: // OR A
		programCounter++;
		opOr(regA);
		if (debug)
			opcodeRead += "OR A";
		durationInCycles = 4;
		break;
	case 0xB8: // CP B
		programCounter++;
		opCp(regB);
		if (debug)
			opcodeRead += "CP B";
		durationInCycles = 4;
		break;
	case 0xB9: // CP C
		programCounter++;
		opCp(regC);
		if (debug)
			opcodeRead += "CP C";
		durationInCycles = 4;
		break;
	case 0xBA: // CP D
		programCounter++;
		opCp(regD);
		if (debug)
			opcodeRead += "CP D";
		durationInCycles = 4;
		break;
	case 0xBB: // CP E
		programCounter++;
		opCp(regE);
		if (debug)
			opcodeRead += "CP E";
		durationInCycles = 4;
		break;
	case 0xBC: // CP H
		programCounter++;
		opCp(regH);
		if (debug)
			opcodeRead += "CP H";
		durationInCycles = 4;
		break;
	case 0xBD: // CP L
		programCounter++;
		opCp(regL);
		if (debug)
			opcodeRead += "CP L";
		durationInCycles = 4;
		break;
	case 0xBE: // CP (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value = memMng.readByte(addr);
		opCp(value);
		if (debug)
			opcodeRead += "CP (HL)";
		durationInCycles = 8;
		break;
	}
	case 0xBF: // CP A
		programCounter++;
		opCp(regA);
		if (debug)
			opcodeRead += "CP A";
		durationInCycles = 4;
		break;
	case 0XC0: // RET NZ
		programCounter++;
		if (!getZeroFlag())
		{
			opRet();
			durationInCycles = 20;
		}
		else
			durationInCycles = 8;
		if (debug)
			opcodeRead += "RET NZ";
		break;
	case 0xC1: // POP BC
		programCounter++;
		opPop(regB, regC);
		if (debug)
			opcodeRead += "POP BC";
		durationInCycles = 12;
		break;
	case 0xC2: // JP NZ a16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;

		if (!getZeroFlag())
		{
			programCounter = addr;
			durationInCycles = 16;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "ADD NZ ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xC3: // JP a16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		programCounter = addr;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "JP ";
			opcodeRead += stream.str();
		}
		durationInCycles = 16;
		break;
	}
	case 0xC4 : // CALL NZ, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (!getZeroFlag())
		{ 
			opCall(addr);
			durationInCycles = 24;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "CALL NZ, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xC5: // PUSH BC
		programCounter++;
		opPush(regB, regC);
		if (debug)
			opcodeRead += "PUSH BC";
		durationInCycles = 16;
		break;
	case 0xC6: // ADD A, d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		opAdd(regA, val);
		if (debug)
		{
			std::stringstream stream;
			uint8_t val2Disp = (uint8_t)val;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "ADD A, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	}
	case 0xC7: // RST 0x00
		programCounter++;
		opRst(0x00);
		if (debug)
			opcodeRead += "RST 0x00";
		durationInCycles = 16;
		break;
	case 0XC8: // RET Z
		programCounter++;
		if (debug)
			opcodeRead += "RET Z";
		if (getZeroFlag())
		{
			opRet();
			durationInCycles = 20;
		}
		else
			durationInCycles = 8;
		break;
	case 0xC9 : // RET
		programCounter++;
		if (debug)
			opcodeRead += "RET";
		opRet();
		durationInCycles = 16;
		break;
	case 0xCA: // JP Z, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (getZeroFlag())
		{
			programCounter = addr;
			durationInCycles = 16;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "JP Z ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xCB: // CB Prefix
		programCounter++;
		prefixHandlingDuration = handlePrefixCB(debug);  // TODO Check return value in a better way ???
		if (prefixHandlingDuration > 0)
		{
			durationInCycles = 4 + prefixHandlingDuration;
		}
		break;
	case 0xCC: // CALL Z, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (getZeroFlag())
		{
			opCall(addr);
			durationInCycles = 24;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "CALL Z ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xCD : // CALL a16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		opCall(addr);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "CALL ";
			opcodeRead += stream.str();
		}
		durationInCycles = 24;
		break;
	}
	case 0xCE: // ADC A, d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		opAdc(val);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "ADC A, ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	}
	case 0xCF: // RST 0x08
		programCounter++;
		opRst(0x08);
		if (debug)
			opcodeRead += "RST 0x08";
		durationInCycles = 16;
		break;
	case 0XD0: // RET NC
		programCounter++;
		if (debug)
			opcodeRead += "RET NC";
		if (!getCarryFlag())
		{
			opRet();
			durationInCycles = 20;
		}
		else
			durationInCycles = 8;
		break;
	case 0xD1: // POP DE
		programCounter++;
		opPop(regD, regE);
		if (debug)
			opcodeRead += "POP DE";
		durationInCycles = 12;
		break;
	case 0xD2: // JP NC a16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;

		if (!getCarryFlag())
		{
			programCounter = addr;
			durationInCycles = 16;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "JP NC, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xD4: // CALL NC, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (!getCarryFlag())
		{
			opCall(addr);
			durationInCycles = 24;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "CALL NC, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xD5: // PUSH DE
		programCounter++;
		opPush(regD, regE);
		if (debug)
			opcodeRead += "PUSH DE";
		durationInCycles = 16;
		break;
	case 0xD6: // SUB A, d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		opSub(val);
		durationInCycles = 8;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "SUB A, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xD7: // RST 0x10
		programCounter++;
		opRst(0x10);
		if (debug)
			opcodeRead += "RST 0x10";
		durationInCycles = 16;
		break;
	case 0XD8: // RET C
		programCounter++;
		if (debug)
			opcodeRead += "RET C";
		if (getCarryFlag())
		{
			opRet();
			durationInCycles = 20;
		}
		else
			durationInCycles = 8;
		break;
	case 0xD9: // RETI
		programCounter++;
		opRet();
		IME = true;
		if (debug)
			opcodeRead += "RETI";
		durationInCycles = 16;
		break;
	case 0xDA: // JP C, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (getCarryFlag())
		{
			programCounter = addr;
			durationInCycles = 16;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "JP C, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xDC: // CALL C, d16
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		if (getCarryFlag())
		{
			opCall(addr);
			durationInCycles = 24;
		}
		else
			durationInCycles = 12;

		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "CALL C, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xDE: // SBC A, d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		durationInCycles = sbc(val);
		durationInCycles += 4;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)val;
			opcodeRead += "SBC A, ";
			opcodeRead += stream.str();
		}
		break;
	}
	case 0xDF: // RST 0x18
		programCounter++;
		opRst(0x18);
		if (debug)
			opcodeRead += "RST 0x18";
		durationInCycles = 16;
		break;
	case 0xE0: // LDH (a8),A
	{
		programCounter++;
		uint8_t value = memMng.readByte(programCounter++);
		uint16_t addr = 0xff00 + value;
		memMng.writeByte(addr, regA);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)value;
			opcodeRead += "LD (0xff00 + ";
			opcodeRead += stream.str();
			opcodeRead += "), A";
		}
		durationInCycles = 12;
		break;
	}
	case 0xE1: // POP HL
		programCounter++;
		opPop(regH, regL);
		if (debug)
			opcodeRead += "POP HL";
		durationInCycles = 12;
		break;
	case 0xE2: // LD (C), A
	{
		programCounter++;
		uint16_t addr = 0xff00 + regC;
		memMng.writeByte(addr, regA);
		if (debug)
			opcodeRead += std::string("LD (C), A");
		durationInCycles = 8;
		break;
	}
	case 0xE5: // PUSH HL
		programCounter++;
		opPush(regH, regL);
		if (debug)
			opcodeRead += "PUSH HL";
		durationInCycles = 16;
		break;
	case 0xE6: // AND d8
	{
		programCounter++;
		uint8_t value = memMng.readByte(programCounter++);
		opAnd(value);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)value;
			opcodeRead += "AND ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	}
	case 0xE7: // RST 0x20
		programCounter++;
		opRst(0x20);
		if (debug)
			opcodeRead += "RST 0x20";
		durationInCycles = 16;
		break;
	case 0XE8: // ADD SP, r8
	{
		programCounter++;
		int16_t hl = (int16_t)stackPointer;
		int8_t value = (int8_t)memMng.readByte(programCounter++);
		hl += value;
		resetZeroFlag();
		resetSubFlag();

		// Test Carry
		uint16_t carry = (uint16_t)(stackPointer & 0x00ff) + ((uint16_t)value & 0x00ff);
		if (carry > 0x00ff)
			setCarryFlag();
		else
			resetCarryFlag();

		// Test halfCarry
		uint8_t halfCarry = (uint8_t)(stackPointer & 0x000f) + (uint8_t)(value & 0x0f);
		if (halfCarry > 0x0f)
			setHalfCarryFlag();
		else
			resetHalfCarryFlag();

		stackPointer = hl;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)value;
			opcodeRead += "ADD SP + ";
			opcodeRead += stream.str();
		}
		durationInCycles = 16;
		break;
	}
	case 0xE9: // JP (HL)
		programCounter++;
		programCounter = getHLReg();
		if (debug)
			opcodeRead += "JP (HL)";
		durationInCycles = 4;
		break;
	case 0XEA: // LD (a16), A
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		memMng.writeByte(addr, regA);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "LD (";
			opcodeRead += stream.str();
			opcodeRead += "), A";
		}
		durationInCycles = 16;
		break;
	}
	case 0xEE: // XOR A, d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		opXor(val);
		durationInCycles = 8;
		break;
	}
	case 0xEF: // RST 0x28
		programCounter++;
		opRst(0x28);
		if (debug)
			opcodeRead += "RST 0x28";
		durationInCycles = 16;
		break;
	case 0xF0: // LDH A, (a8)
	{
		programCounter++;
		uint8_t value2Load = memMng.readByte(programCounter++);
		uint16_t addr = 0xff00 + value2Load;
		regA = memMng.readByte(addr);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)value2Load;
			opcodeRead += "LD A, (0xff00 + ";
			opcodeRead += stream.str();
			opcodeRead += ")";
		}
		durationInCycles = 12;
		break;
	}
	case 0xF1: // POP AF
	{
		programCounter++;
		uint8_t flagReg = getFlagRegister();
		opPop(regA, flagReg);
		setFlagRegister(flagReg);
		if (debug)
			opcodeRead += "POP AF";
		durationInCycles = 12;
		break;
	}
	case 0xF2: // LD A, (C)
	{
		programCounter++;
		uint16_t addr = 0xff00 + regC;
		regA = memMng.readByte(addr);
		if (debug)
			opcodeRead += std::string("LD A, (C)");
		durationInCycles = 8;
		break;
	}
	case 0xF3: // DI
		programCounter++;
		IME = false;
		if (debug)
			opcodeRead += "DI";
		durationInCycles = 4;
		break;
	case 0xF5: // PUSH AF
		programCounter++;
		opPush(regA, getFlagRegister());
		if (debug)
			opcodeRead += "PUSH AF";
		durationInCycles = 16;
		break;
	case 0xF6: // OR d8
	{
		programCounter++;
		uint8_t value = memMng.readByte(programCounter++);
		opOr(value);
		durationInCycles = 8;
		break;
	}
	case 0xF7: // RST 0x30
		programCounter++;
		opRst(0x30);
		if (debug)
			opcodeRead += "RST 0x30";
		durationInCycles = 16;
		break;
	
	case 0XF8: // LD HL, SP+r8
	{
		programCounter++;
		int16_t hl = (int16_t)stackPointer;
		int8_t value = (int8_t)memMng.readByte(programCounter++);
		hl += value;
		resetZeroFlag();
		resetSubFlag();

		// Test Carry
		uint16_t carry = (uint16_t)(stackPointer & 0x00ff) + ((uint16_t)value & 0x00ff);
		if (carry > 0x00ff)
			setCarryFlag();
		else
			resetCarryFlag();

		// Test halfCarry
		uint8_t halfCarry = (uint8_t)(stackPointer & 0x000f) + (uint8_t)(value & 0x0f);
		if (halfCarry > 0x0f)
			setHalfCarryFlag();
		else
			resetHalfCarryFlag();

		setHLReg((uint16_t)hl);
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)value;
			opcodeRead += "LD HL, SP + (";
			opcodeRead += stream.str();
			opcodeRead += ")";
		}
		durationInCycles = 16;
		break;
	}

	case 0xF9: // LD SP, HL
	{
		programCounter++;
		stackPointer = getHLReg();
		if (debug)
			opcodeRead += "LD SP, HL";
		durationInCycles = 8;
		break;
	}
	case 0XFA: // LD A, (a16)
	{
		programCounter++;
		uint8_t addrL = memMng.readByte(programCounter++);
		uint8_t addrH = memMng.readByte(programCounter++);
		uint16_t addr = (addrH << 8) | addrL;
		uint8_t value = memMng.readByte(addr);
		regA = value;
		if (debug)
		{
			std::stringstream stream;
			stream << std::hex << (uint16_t)addr;
			opcodeRead += "LD A, (";
			opcodeRead += stream.str();
			opcodeRead += ")";
		}
		durationInCycles = 16;
		break;
	}
	case 0xFB: // EI
		programCounter++;
		IME = true;
		if (debug)
			opcodeRead += "EI";
		durationInCycles = 4;
		break;
	case 0xFE: // CP d8
	{
		programCounter++;
		uint8_t val = memMng.readByte(programCounter++);
		opCp(val);
		if (debug)
		{
			std::stringstream stream;
			unsigned char val2Disp = (unsigned char)val;
			stream << std::hex << (uint16_t)val2Disp;
			opcodeRead += "CP ";
			opcodeRead += stream.str();
		}
		durationInCycles = 8;
		break;
	}
	case 0xFF: // RST 0x38
		programCounter++;
		opRst(0x38);
		if (debug)
			opcodeRead += "RST 0x38";
		durationInCycles = 16;
		break;
	default :
		std::cout << "Unsupported Opcode : " << std::hex << (uint16_t)opcode << " at " << programCounter << std::endl;
		return -1;
	}

	if (debug)
	{
		unsigned char a = regA;
		unsigned char f = getFlagRegister();
		unsigned char b = regB;
		unsigned char c = regC;
		unsigned char d = regD;
		unsigned char e = regE;
		unsigned char h = regH;
		unsigned char l = regL;

		std::cout << "opcode read = " << opcodeRead << "\t" <<
			" B :" << std::hex << (uint16_t)b <<
			" C :" << std::hex << (uint16_t)c <<
			" D :" << std::hex << (uint16_t)d <<
			" E :" << std::hex << (uint16_t)e <<
			" H :" << std::hex << (uint16_t)h <<
			" L :" << std::hex << (uint16_t)l <<
			" A :" << std::hex << (uint16_t)a <<
			" F :" << std::hex << (uint16_t)f << 
			" SP : " << std::hex << (uint16_t)stackPointer 
			<< " IME : " << IME << std::endl;
	}

	return  durationInCycles;
}
