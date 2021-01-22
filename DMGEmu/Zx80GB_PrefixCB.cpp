#include "pch.h"
#include "Zx80GB.h"

#include <iostream>

int Zx80GB::handlePrefixCB(bool debug)
{
	int durationInCycles = 0;
	uint8_t prefix = memMng.readByte(programCounter);
	switch (prefix)
	{
	case 0x00: // RLC B
		programCounter++;
		opRlc(regB);
		if (debug)
			opcodeRead += "RLC B";
		durationInCycles = 4;
		break;
	case 0x01: // RLC C
		programCounter++;
		opRlc(regC);
		if (debug)
			opcodeRead += "RLC C";
		durationInCycles = 4;
		break;
	case 0x02: // RLC D
		programCounter++;
		opRlc(regD);
		if (debug)
			opcodeRead += "RLC D";
		durationInCycles = 4;
		break;
	case 0x03: // RLC E
		programCounter++;
		opRlc(regE);
		if (debug)
			opcodeRead += "RLC E";
		durationInCycles = 4;
		break;
	case 0x04: // RLC H
		programCounter++;
		opRlc(regH);
		if (debug)
			opcodeRead += "RLC H";
		durationInCycles = 4;
		break;
	case 0x05: // RLC L
		programCounter++;
		opRlc(regL);
		if (debug)
			opcodeRead += "RLC L";
		durationInCycles = 4;
		break;
	case 0x06: // RLC (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opRlc(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "RLC (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x07: // RLC A
		programCounter++;
		opRlc(regA);
		if (debug)
			opcodeRead += "RLC A";
		durationInCycles = 4;
		break;
	case 0x08: // RRC B
		programCounter++;
		opRrc(regB);
		if (debug)
			opcodeRead += "RRC B";
		durationInCycles = 4;
		break;
	case 0x09: // RRC C
		programCounter++;
		opRrc(regC);
		if (debug)
			opcodeRead += "RRC C";
		durationInCycles = 4;
		break;
	case 0x0A: // RRC D
		programCounter++;
		opRrc(regD);
		if (debug)
			opcodeRead += "RRC D";
		durationInCycles = 4;
		break;
	case 0x0B: // RRC E
		programCounter++;
		opRrc(regE);
		if (debug)
			opcodeRead += "RRC E";
		durationInCycles = 4;
		break;
	case 0x0C: // RRC H
		programCounter++;
		opRrc(regH);
		if (debug)
			opcodeRead += "RRC H";
		durationInCycles = 4;
		break;
	case 0x0D: // RRC L
		programCounter++;
		opRrc(regL);
		if (debug)
			opcodeRead += "RRC L";
		durationInCycles = 4;
		break;
	case 0x0E: // RRC (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opRrc(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "RRC (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x0F: // RRC A
		programCounter++;
		opRrc(regA);
		if (debug)
			opcodeRead += "RRC A";
		durationInCycles = 4;
		break;
	case 0x10: // RL B
		programCounter++;
		opRl(regB);
		if (debug)
			opcodeRead += "RL B";
		durationInCycles = 4;
		break;
	case 0x11: // RL C
		programCounter++;
		opRl(regC);
		if (debug)
			opcodeRead += "RL C";
		durationInCycles = 4;
		break;
	case 0x12: // RL D
		programCounter++;
		opRl(regD);
		if (debug)
			opcodeRead += "RL D";
		durationInCycles = 4;
		break;
	case 0x13: // RL E
		programCounter++;
		opRl(regE);
		if (debug)
			opcodeRead += "RL E";
		durationInCycles = 4;
		break;
	case 0x14: // RL H
		programCounter++;
		opRl(regH);
		if (debug)
			opcodeRead += "RL H";
		durationInCycles = 4;
		break;
	case 0x15: // RL L
		programCounter++;
		opRl(regL);
		if (debug)
			opcodeRead += "RL L";
		durationInCycles = 4;
		break;
	case 0x16: // RL (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opRl(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "RL (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x17: // RL A
		programCounter++;
		opRl(regA);
		if (debug)
			opcodeRead += "RL A";
		durationInCycles = 4;
		break;
	case 0x18: // RR B
		programCounter++;
		opRr(regB);
		if (debug)
			opcodeRead += "RR B";
		durationInCycles = 4;
		break;
	case 0x19: // RR C
		programCounter++;
		opRr(regC);
		if (debug)
			opcodeRead += "RR C";
		durationInCycles = 4;
		break;
	case 0x1A: // RR D
		programCounter++;
		opRr(regD);
		if (debug)
			opcodeRead += "RR D";
		durationInCycles = 4;
		break;
	case 0x1B: // RR E
		programCounter++;
		opRr(regE);
		if (debug)
			opcodeRead += "RR E";
		durationInCycles = 4;
		break;
	case 0x1C: // RR H
		programCounter++;
		opRr(regH);
		if (debug)
			opcodeRead += "RR H";
		durationInCycles = 4;
		break;
	case 0x1D: // RR L
		programCounter++;
		opRr(regL);
		if (debug)
			opcodeRead += "RR L";
		durationInCycles = 4;
		break;
	case 0x1E: // RR (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opRr(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "RR (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x1F: // RR A
		programCounter++;
		opRr(regA);
		if (debug)
			opcodeRead += "RR A";
		durationInCycles = 4;
		break;
	case 0x20: // SLA B
		programCounter++;
		opSla(regB);
		if (debug)
			opcodeRead += "SLA B";
		durationInCycles = 4;
		break;
	case 0x21: // SLA C
		programCounter++;
		opSla(regC);
		if (debug)
			opcodeRead += "SLA C";
		durationInCycles = 4;
		break;
	case 0x22: // SLA D
		programCounter++;
		opSla(regD);
		if (debug)
			opcodeRead += "SLA D";
		durationInCycles = 4;
		break;
	case 0x23: // SLA E
		programCounter++;
		opSla(regE);
		if (debug)
			opcodeRead += "SLA E";
		durationInCycles = 4;
		break;
	case 0x24: // SLA H
		programCounter++;
		opSla(regH);
		if (debug)
			opcodeRead += "SLA H";
		durationInCycles = 4;
		break;
	case 0x25: // SLA L
		programCounter++;
		opSla(regL);
		if (debug)
			opcodeRead += "SLA L";
		durationInCycles = 4;
		break;
	case 0x26: // SLA (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opSla(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "SLA (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x27: // SLA A
		programCounter++;
		opSla(regA);
		if (debug)
			opcodeRead += "SLA A";
		durationInCycles = 4;
		break;
	case 0x28: // SRA B
		programCounter++;
		opSra(regB);
		if (debug)
			opcodeRead += "SRA B";
		durationInCycles = 4;
		break;
	case 0x29: // SRA C
		programCounter++;
		opSra(regC);
		if (debug)
			opcodeRead += "SRA C";
		durationInCycles = 4;
		break;
	case 0x2A: // SRA D
		programCounter++;
		opSra(regD);
		if (debug)
			opcodeRead += "SRA D";
		durationInCycles = 4;
		break;
	case 0x2B: // SRA E
		programCounter++;
		opSra(regE);
		if (debug)
			opcodeRead += "SRA E";
		durationInCycles = 4;
		break;
	case 0x2C: // SRA H
		programCounter++;
		opSra(regH);
		if (debug)
			opcodeRead += "SRA H";
		durationInCycles = 4;
		break;
	case 0x2D: // SRA L
		programCounter++;
		opSra(regL);
		if (debug)
			opcodeRead += "SRA L";
		durationInCycles = 4;
		break;
	case 0x2E: // SRA (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opSra(val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "SRA (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x2F: // SRA A
		programCounter++;
		opSra(regA);
		if (debug)
			opcodeRead += "SRA A";
		durationInCycles = 4;
		break;
	case 0x30: // SWAP B
		programCounter++;
		opSwap(regB);
		if (debug)
			opcodeRead += "SWAP B";
		durationInCycles = 4;
		break;
	case 0x31: // SWAP C
		programCounter++;
		opSwap(regC);
		if (debug)
			opcodeRead += "SWAP C";
		durationInCycles = 4;
		break;
	case 0x32: // SWAP D
		programCounter++;
		opSwap(regD);
		if (debug)
			opcodeRead += "SWAP D";
		durationInCycles = 4;
		break;
	case 0x33: // SWAP E
		programCounter++;
		opSwap(regE);
		if (debug)
			opcodeRead += "SWAP E";
		durationInCycles = 4;
		break;
	case 0x34: // SWAP H
		programCounter++;
		opSwap(regH);
		if (debug)
			opcodeRead += "SWAP H";
		durationInCycles = 4;
		break;
	case 0x35: // SWAP L
		programCounter++;
		opSwap(regL);
		if (debug)
			opcodeRead += "SWAP L";
		durationInCycles = 4;
		break;
	case 0x36: // SWAP (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t value = memMng.readByte(addr);
		opSwap(value);
		memMng.writeByte(addr, value);
		if (debug)
			opcodeRead += "SWAP (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x37: // SWAP A
		programCounter++;
		opSwap(regA);
		if (debug)
			opcodeRead += "SWAP A";
		durationInCycles = 4;
		break;
	case 0x38: // SRL B
		programCounter++;
		opSrl(regB);
		if (debug)
			opcodeRead += "SRL B";
		durationInCycles = 4;
		break;
	case 0x39: // SRL C
		programCounter++;
		opSrl(regC);
		if (debug)
			opcodeRead += "SRL C";
		durationInCycles = 4;
		break;
	case 0x3A: // SRL D
		programCounter++;
		opSrl(regD);
		if (debug)
			opcodeRead += "SRL D";
		durationInCycles = 4;
		break;
	case 0x3B: // SRL E
		programCounter++;
		opSrl(regE);
		if (debug)
			opcodeRead += "SRL E";
		durationInCycles = 4;
		break;
	case 0x3C: // SRL H
		programCounter++;
		opSrl(regH);
		if (debug)
			opcodeRead += "SRL H";
		durationInCycles = 4;
		break;
	case 0x3D: // SRL L
		programCounter++;
		opSrl(regL);
		if (debug)
			opcodeRead += "SRL L";
		durationInCycles = 4;
		break;
	case 0x3E: // SRL (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t val = memMng.readByte(addr);
		opSrl(val);
		memMng.writeByte(addr, val);
		durationInCycles = 12;
		if (debug)
			opcodeRead += "SRL (HL)";
		break;
	}
	case 0x3F: // SRL A
		programCounter++;
		opSrl(regA);
		if (debug)
			opcodeRead += "SRL A";
		durationInCycles = 4;
		break;
	case 0x40: // BIT 0,B
		programCounter++;
		opBit(0, regB);
		if (debug)
			opcodeRead += "BIT 0, B";
		durationInCycles = 4;
		break;
	case 0x41: // BIT 0,C
		programCounter++;
		opBit(0, regC);
		if (debug)
			opcodeRead += "BIT 0, C";
		durationInCycles = 4;
		break;
	case 0x42: // BIT 0,D
		programCounter++;
		opBit(0, regD);
		if (debug)
			opcodeRead += "BIT 0, D";
		durationInCycles = 4;
		break;
	case 0x43: // BIT 0,E
		programCounter++;
		opBit(0, regE);
		if (debug)
			opcodeRead += "BIT 0, E";
		durationInCycles = 4;
		break;
	case 0x44: // BIT 0,H
		programCounter++;
		opBit(0, regH);
		if (debug)
			opcodeRead += "BIT 0, H";
		durationInCycles = 4;
		break;
	case 0x45: // BIT 0,L
		programCounter++;
		opBit(0, regL);
		if (debug)
			opcodeRead += "BIT 0, L";
		durationInCycles = 4;
		break;
	case 0x46: // BIT 0,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(0, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 0, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x47: // BIT 0,A
		programCounter++;
		opBit(0, regA);
		if (debug)
			opcodeRead += "BIT 0, A";
		durationInCycles = 4;
		break;
	case 0x48: // BIT 1,B
		programCounter++;
		opBit(1, regB);
		if (debug)
			opcodeRead += "BIT 1, B";
		durationInCycles = 4;
		break;
	case 0x49: // BIT 1,C
		programCounter++;
		opBit(1, regC);
		if (debug)
			opcodeRead += "BIT 1, C";
		durationInCycles = 4;
		break;
	case 0x4A: // BIT 1,D
		programCounter++;
		opBit(1, regD);
		if (debug)
			opcodeRead += "BIT 1, D";
		durationInCycles = 4;
		break;
	case 0x4B: // BIT 1,E
		programCounter++;
		opBit(1, regE);
		if (debug)
			opcodeRead += "BIT 1, E";
		durationInCycles = 4;
		break;
	case 0x4C: // BIT 1,H
		programCounter++;
		opBit(1, regH);
		if (debug)
			opcodeRead += "BIT 1, H";
		durationInCycles = 4;
		break;
	case 0x4D: // BIT 1,L
		programCounter++;
		opBit(1, regL);
		if (debug)
			opcodeRead += "BIT 1, L";
		durationInCycles = 4;
		break;
	case 0x4E: // BIT 1,(HL)
	{
		programCounter++;
		__int16 addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(1, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 1, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x4F: // BIT 1,A
		programCounter++;
		opBit(1, regA);
		if (debug)
			opcodeRead += "BIT 1, A";
		durationInCycles = 4;
		break;
	case 0x50: // BIT 2,B
		programCounter++;
		opBit(2, regB);
		if (debug)
			opcodeRead += "BIT 2, B";
		durationInCycles = 4;
		break;
	case 0x51: // BIT 2,C
		programCounter++;
		opBit(2, regC);
		if (debug)
			opcodeRead += "BIT 2, C";
		durationInCycles = 4;
		break;
	case 0x52: // BIT 2,D
		programCounter++;
		opBit(2, regD);
		if (debug)
			opcodeRead += "BIT 2, D";
		durationInCycles = 4;
		break;
	case 0x53: // BIT 2,E
		programCounter++;
		opBit(2, regE);
		if (debug)
			opcodeRead += "BIT 2, E";
		durationInCycles = 4;
		break;
	case 0x54: // BIT 2,H
		programCounter++;
		opBit(2, regH);
		if (debug)
			opcodeRead += "BIT 2, H";
		durationInCycles = 4;
		break;
	case 0x55: // BIT 2,L
		programCounter++;
		opBit(2, regL);
		if (debug)
			opcodeRead += "BIT 2, L";
		durationInCycles = 4;
		break;
	case 0x56: // BIT 2, (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(2, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 2, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x57: // BIT 2,A
		programCounter++;
		opBit(2, regA);
		if (debug)
			opcodeRead += "BIT 2, A";
		durationInCycles = 4;
		break;
	case 0x58: // BIT 3,B
		programCounter++;
		opBit(3, regB);
		if (debug)
			opcodeRead += "BIT 3, B";
		durationInCycles = 4;
		break;
	case 0x59: // BIT 3,C
		programCounter++;
		opBit(3, regC);
		if (debug)
			opcodeRead += "BIT 3, C";
		durationInCycles = 4;
		break;
	case 0x5A: // BIT 3,D
		programCounter++;
		opBit(3, regD);
		if (debug)
			opcodeRead += "BIT 3, D";
		durationInCycles = 4;
		break;
	case 0x5B: // BIT 3,E
		programCounter++;
		opBit(3, regE);
		if (debug)
			opcodeRead += "BIT 3, E";
		durationInCycles = 4;
		break;
	case 0x5C: // BIT 3,H
		programCounter++;
		opBit(3, regH);
		if (debug)
			opcodeRead += "BIT 3, H";
		durationInCycles = 4;
		break;
	case 0x5D: // BIT 3,L
		programCounter++;
		opBit(3, regL);
		if (debug)
			opcodeRead += "BIT 3, L";
		durationInCycles = 4;
		break;
	case 0x5E: // BIT 3,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(3, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 3, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x5F: // BIT 3,A
		programCounter++;
		opBit(3, regA);
		if (debug)
			opcodeRead += "BIT 3, A";
		durationInCycles = 4;
		break;
	case 0x60: // BIT 4,B
		programCounter++;
		opBit(4, regB);
		if (debug)
			opcodeRead += "BIT 4, B";
		durationInCycles = 4;
		break;
	case 0x61: // BIT 4,C
		programCounter++;
		opBit(4, regC);
		if (debug)
			opcodeRead += "BIT 4, C";
		durationInCycles = 4;
		break;
	case 0x62: // BIT 4,D
		programCounter++;
		opBit(4, regD);
		if (debug)
			opcodeRead += "BIT 4, D";
		durationInCycles = 4;
		break;
	case 0x63: // BIT 4,E
		programCounter++;
		opBit(4, regE);
		if (debug)
			opcodeRead += "BIT 4, E";
		durationInCycles = 4;
		break;
	case 0x64: // BIT 4,H
		programCounter++;
		opBit(4, regH);
		if (debug)
			opcodeRead += "BIT 4, H";
		durationInCycles = 4;
		break;
	case 0x65: // BIT 4,L
		programCounter++;
		opBit(4, regL);
		if (debug)
			opcodeRead += "BIT 4, L";
		durationInCycles = 4;
		break;
	case 0x66: // BIT 4,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(4, val);
		memMng.writeByte(addr, val);		if (debug)
			opcodeRead += "BIT 4, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x67: // BIT 4,A
		programCounter++;
		opBit(4, regA);
		if (debug)
			opcodeRead += "BIT 4, A";
		durationInCycles = 4;
		break;
	case 0x68: // BIT 5,B
		programCounter++;
		opBit(5, regB);
		if (debug)
			opcodeRead += "BIT 5, B";
		durationInCycles = 4;
		break;
	case 0x69: // BIT 5,C
		programCounter++;
		opBit(5, regC);
		if (debug)
			opcodeRead += "BIT 5, C";
		durationInCycles = 4;
		break;
	case 0x6A: // BIT 5,D
		programCounter++;
		opBit(5, regD);
		if (debug)
			opcodeRead += "BIT 5, D";
		durationInCycles = 4;
		break;
	case 0x6B: // BIT 5,E
		programCounter++;
		opBit(5, regE);
		if (debug)
			opcodeRead += "BIT 5, E";
		durationInCycles = 4;
		break;
	case 0x6C: // BIT 5,H
		programCounter++;
		opBit(5, regH);
		if (debug)
			opcodeRead += "BIT 5, H";
		durationInCycles = 4;
		break;
	case 0x6D: // BIT 5,L
		programCounter++;
		opBit(5, regL);
		if (debug)
			opcodeRead += "BIT 5, L";
		durationInCycles = 4;
		break;
	case 0x6E: // BIT 5,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(5, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 5, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x6F: // BIT 5,A
		programCounter++;
		opBit(5, regA);
		if (debug)
			opcodeRead += "BIT 5, A";
		durationInCycles = 4;
		break;
	case 0x70: // BIT 6,B
		programCounter++;
		opBit(6, regB);
		if (debug)
			opcodeRead += "BIT 6, B";
		durationInCycles = 4;
		break;
	case 0x71: // BIT 6,C
		programCounter++;
		opBit(6, regC);
		if (debug)
			opcodeRead += "BIT 6, C";
		durationInCycles = 4;
		break;
	case 0x72: // BIT 6,D
		programCounter++;
		opBit(6, regD);
		if (debug)
			opcodeRead += "BIT 6, D";
		durationInCycles = 4;
		break;
	case 0x73: // BIT 6,E
		programCounter++;
		opBit(6, regE);
		if (debug)
			opcodeRead += "BIT 6, E";
		durationInCycles = 4;
		break;
	case 0x74: // BIT 6,H
		programCounter++;
		opBit(6, regH);
		if (debug)
			opcodeRead += "BIT 6, H";
		durationInCycles = 4;
		break;
	case 0x75 : // BIT 6,L
		programCounter++;
		opBit(6, regL);
		if (debug)
			opcodeRead += "BIT 6, L";
		durationInCycles = 4;
		break;
	case 0x76 : // BIT 6,HL
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(6, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 6, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x77 : // BIT 6,A
		programCounter++;
		opBit(6, regA);
		if (debug)
			opcodeRead += "BIT 6, A";
		durationInCycles = 4;
		break;
	case 0x78 : // BIT 7,B
		programCounter++;
		opBit(7, regB);
		if (debug)
			opcodeRead += "BIT 7, B";
		durationInCycles = 4;
		break;
	case 0x79 : // BIT 7,C
		programCounter++;
		opBit(7, regC);
		if (debug)
			opcodeRead += "BIT 7, C";
		durationInCycles = 4;
		break;
	case 0x7A: // BIT 7,D
		programCounter++;
		opBit(7, regD);
		if (debug)
			opcodeRead += "BIT 7, D";
		durationInCycles = 4;
		break;
	case 0x7B: // BIT 7,E
		programCounter++;
		opBit(7, regE);
		if (debug)
			opcodeRead += "BIT 7, E";
		durationInCycles = 4;
		break;
	case 0x7C : // BIT 7,H
		programCounter++;
		opBit(7, regH);
		if (debug)
			opcodeRead += "BIT 7, H";
		// ENDIF
		durationInCycles = 4;
		break;
	case 0x7D: // BIT 7,L
		programCounter++;
		opBit(7, regL);
		if (debug)
			opcodeRead += "BIT 7, L";
		durationInCycles = 4;
		break;
	case 0x7E: // BIT 7,HL
	{
		programCounter++;
		uint16_t addr = getHLReg();
		__int8 val = memMng.readByte(addr);
		opBit(7, val);
		memMng.writeByte(addr, val);
		if (debug)
			opcodeRead += "BIT 7, (HL)";
		durationInCycles = 12;
		break;
	}
	case 0x7F: // BIT 7,A
		programCounter++;
		opBit(7, regA);
		if (debug)
			opcodeRead += "BIT 7, A";
		durationInCycles = 4;
		break;
	case 0x80: // RES 0,B
		programCounter++;
		opRes(0, regB);
		if (debug)
			opcodeRead += "RES 0, B";
		durationInCycles = 8;
		break;
	case 0x81: // RES 0,C
		programCounter++;
		opRes(0, regC);
		if (debug)
			opcodeRead += "RES 0, C";
		durationInCycles = 8;
		break;
	case 0x82: // RES 0,D
		programCounter++;
		opRes(0, regD);
		if (debug)
			opcodeRead += "RES 0, D";
		durationInCycles = 8;
		break;
	case 0x83: // RES 0,E
		programCounter++;
		opRes(0, regE);
		if (debug)
			opcodeRead += "RES 0, E";
		durationInCycles = 8;
		break;
	case 0x84: // RES 0,H
		programCounter++;
		opRes(0, regH);
		if (debug)
			opcodeRead += "RES 0, H";
		durationInCycles = 8;
		break;
	case 0x85: // RES 0,L
		programCounter++;
		opRes(0, regL);
		if (debug)
			opcodeRead += "RES 0, L";
		durationInCycles = 8;
		break;
	case 0x86: // RES 0, (HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(0, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 0, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0x87: // RES 0,A
		programCounter++;
		opRes(0, regA);
		if (debug)
			opcodeRead += "RES 0, A";
		durationInCycles = 8;
		break;
	case 0x88: // RES 1,B
		programCounter++;
		opRes(1, regB);
		if (debug)
			opcodeRead += "RES 1, B";
		durationInCycles = 8;
		break;
	case 0x89: // RES 1,C
		programCounter++;
		opRes(1, regC);
		if (debug)
			opcodeRead += "RES 1, C";
		durationInCycles = 8;
		break;
	case 0x8A: // RES 1,D
		programCounter++;
		opRes(1, regD);
		if (debug)
			opcodeRead += "RES 1, D";
		durationInCycles = 8;
		break;
	case 0x8B: // RES 1,E
		programCounter++;
		opRes(1, regE);
		if (debug)
			opcodeRead += "RES 1, E";
		durationInCycles = 8;
		break;
	case 0x8C: // RES 1,H
		programCounter++;
		opRes(1, regH);
		if (debug)
			opcodeRead += "RES 1, H";
		durationInCycles = 8;
		break;
	case 0x8D: // RES 1,L
		programCounter++;
		opRes(1, regL);
		if (debug)
			opcodeRead += "RES 1, L";
		durationInCycles = 8;
		break;
	case 0x8E: // RES 1,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(1, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 1, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0x8F: // RES 1,A
		programCounter++;
		opRes(1, regA);
		if (debug)
			opcodeRead += "RES 1, A";
		durationInCycles = 8;
		break;
	case 0x90: // RES 2,B
		programCounter++;
		opRes(2, regB);
		if (debug)
			opcodeRead += "RES 2, B";
		durationInCycles = 8;
		break;
	case 0x91: // RES 2,C
		programCounter++;
		opRes(2, regC);
		if (debug)
			opcodeRead += "RES 2, C";
		durationInCycles = 8;
		break;
	case 0x92: // RES 2,D
		programCounter++;
		opRes(2, regD);
		if (debug)
			opcodeRead += "RES 2, D";
		durationInCycles = 8;
		break;
	case 0x93: // RES 2,E
		programCounter++;
		opRes(2, regE);
		if (debug)
			opcodeRead += "RES 2, E";
		durationInCycles = 8;
		break;
	case 0x94: // RES 2,H
		programCounter++;
		opRes(2, regH);
		if (debug)
			opcodeRead += "RES 2, H";
		durationInCycles = 8;
		break;
	case 0x95: // RES 2,L
		programCounter++;
		opRes(2, regL);
		if (debug)
			opcodeRead += "RES 2, L";
		durationInCycles = 8;
		break;
	case 0x96: // RES 2,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(2, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 2, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0x97: // RES 2,A
		programCounter++;
		opRes(2, regA);
		if (debug)
			opcodeRead += "RES 2, A";
		durationInCycles = 8;
		break;
	case 0x98: // RES 3,B
		programCounter++;
		opRes(3, regB);
		if (debug)
			opcodeRead += "RES 3, B";
		durationInCycles = 8;
		break;
	case 0x99: // RES 3,C
		programCounter++;
		opRes(3, regC);
		if (debug)
			opcodeRead += "RES 3, C";
		durationInCycles = 8;
		break;
	case 0x9A: // RES 3,D
		programCounter++;
		opRes(3, regD);
		if (debug)
			opcodeRead += "RES 3, D";
		durationInCycles = 8;
		break;
	case 0x9B: // RES 3,E
		programCounter++;
		opRes(3, regE);
		if (debug)
			opcodeRead += "RES 3, E";
		durationInCycles = 8;
		break;
	case 0x9C: // RES 3,H
		programCounter++;
		opRes(3, regH);
		if (debug)
			opcodeRead += "RES 3, H";
		durationInCycles = 8;
		break;
	case 0x9D: // RES 3,L
		programCounter++;
		opRes(3, regL);
		if (debug)
			opcodeRead += "RES 3, L";
		durationInCycles = 8;
		break;
	case 0x9E: // RES 3,(HL)
		{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(3, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 3, (HL)";
		durationInCycles = 16;
		break;
		}
	case 0x9F: // RES 3,A
		programCounter++;
		opRes(3, regA);
		if (debug)
			opcodeRead += "RES 3, A";
		durationInCycles = 8;
		break;
	case 0xA0: // RES 4,B
		programCounter++;
		opRes(4, regB);
		if (debug)
			opcodeRead += "RES 4, B";
		durationInCycles = 8;
		break;
	case 0xA1: // RES 4,C
		programCounter++;
		opRes(4, regC);
		if (debug)
			opcodeRead += "RES 4, C";
		durationInCycles = 8;
		break;
	case 0xA2: // RES 4,D
		programCounter++;
		opRes(4, regD);
		if (debug)
			opcodeRead += "RES 4, D";
		durationInCycles = 8;
		break;
	case 0xA3: // RES 4,E
		programCounter++;
		opRes(4, regE);
		if (debug)
			opcodeRead += "RES 4, E";
		durationInCycles = 8;
		break;
	case 0xA4: // RES 4,H
		programCounter++;
		opRes(4, regH);
		if (debug)
			opcodeRead += "RES 4, H";
		durationInCycles = 8;
		break;
	case 0xA5: // RES 4,L
		programCounter++;
		opRes(4, regL);
		if (debug)
			opcodeRead += "RES 4, L";
		durationInCycles = 8;
		break;
	case 0xA6: // RES 4,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(4, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 4, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xA7: // RES 4,A
		programCounter++;
		opRes(4, regA);
		if (debug)
			opcodeRead += "RES 4, A";
		durationInCycles = 8;
		break;
	case 0xA8: // RES 5,B
		programCounter++;
		opRes(5, regB);
		if (debug)
			opcodeRead += "RES 5, B";
		durationInCycles = 8;
		break;
	case 0xA9: // RES 5,C
		programCounter++;
		opRes(5, regC);
		if (debug)
			opcodeRead += "RES 5, C";
		durationInCycles = 8;
		break;
	case 0xAA: // RES 5,D
		programCounter++;
		opRes(5, regD);
		if (debug)
			opcodeRead += "RES 5, D";
		durationInCycles = 8;
		break;
	case 0xAB: // RES 5,E
		programCounter++;
		opRes(5, regE);
		if (debug)
			opcodeRead += "RES 5, E";
		durationInCycles = 8;
		break;
	case 0xAC: // RES 5,H
		programCounter++;
		opRes(5, regH);
		if (debug)
			opcodeRead += "RES 5, H";
		durationInCycles = 8;
		break;
	case 0xAD: // RES 5,L
		programCounter++;
		opRes(5, regL);
		if (debug)
			opcodeRead += "RES 5, L";
		durationInCycles = 8;
		break;
	case 0xAE: // RES 5,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(5, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 5, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xAF: // RES 5,A
		programCounter++;
		opRes(5, regA);
		if (debug)
			opcodeRead += "RES 5, A";
		durationInCycles = 8;
		break;
	case 0xB0: // RES 6,B
		programCounter++;
		opRes(6, regB);
		if (debug)
			opcodeRead += "RES 6, B";
		durationInCycles = 8;
		break;
	case 0xB1: // RES 6,C
		programCounter++;
		opRes(6, regC);
		if (debug)
			opcodeRead += "RES 6, C";
		durationInCycles = 8;
		break;
	case 0xB2: // RES 6,D
		programCounter++;
		opRes(6, regD);
		if (debug)
			opcodeRead += "RES 6, D";
		durationInCycles = 8;
		break;
	case 0xB3: // RES 6,E
		programCounter++;
		opRes(6, regE);
		if (debug)
			opcodeRead += "RES 6, E";
		durationInCycles = 8;
		break;
	case 0xB4: // RES 6,H
		programCounter++;
		opRes(6, regH);
		if (debug)
			opcodeRead += "RES 6, H";
		durationInCycles = 8;
		break;
	case 0xB5: // RES 6,L
		programCounter++;
		opRes(6, regL);
		if (debug)
			opcodeRead += "RES 6, L";
		durationInCycles = 8;
		break;
	case 0xB6: // RES 6,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(6, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 6, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xB7: // RES 6,A
		programCounter++;
		opRes(6, regA);
		if (debug)
			opcodeRead += "RES 6, A";
		durationInCycles = 8;
		break;
	case 0xB8: // RES 7,B
		programCounter++;
		opRes(7, regB);
		if (debug)
			opcodeRead += "RES 7, B";
		durationInCycles = 8;
		break;
	case 0xB9: // RES 7,C
		programCounter++;
		opRes(7, regC);
		if (debug)
			opcodeRead += "RES 7, C";
		durationInCycles = 8;
		break;
	case 0xBA: // RES 7,D
		programCounter++;
		opRes(7, regD);
		if (debug)
			opcodeRead += "RES 7, D";
		durationInCycles = 8;
		break;
	case 0xBB: // RES 7,E
		programCounter++;
		opRes(7, regE);
		if (debug)
			opcodeRead += "RES 7, E";
		durationInCycles = 8;
		break;
	case 0xBC: // RES 7,H
		programCounter++;
		opRes(7, regH);
		if (debug)
			opcodeRead += "RES 7, h";
		durationInCycles = 8;
		break;
	case 0xBD: // RES 7,L
		programCounter++;
		opRes(7, regL);
		if (debug)
			opcodeRead += "RES 7, L";
		durationInCycles = 8;
		break;
	case 0xBE: // RES 7,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opRes(7, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "RES 7, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xBF: // RES 7,A
	{
		programCounter++;
		opRes(7, regA);
		if (debug)
			opcodeRead += "RES 7, A";
		durationInCycles = 8;
		break;
	}
	case 0xC0: // SET 0,B
		programCounter++;
		opSet(0, regB);
		if (debug)
			opcodeRead += "SET 0, B";
		durationInCycles = 8;
		break;
	case 0xC1: // SET 0,C
		programCounter++;
		opSet(0, regC);
		if (debug)
			opcodeRead += "SET 0, C";
		durationInCycles = 8;
		break;
	case 0xC2: // SET 0,D
		programCounter++;
		opSet(0, regD);
		if (debug)
			opcodeRead += "SET 0, D";
		durationInCycles = 8;
		break;
	case 0xC3: // SET 0,E
		programCounter++;
		opSet(0, regE);
		if (debug)
			opcodeRead += "SET 0, E";
		durationInCycles = 8;
		break;
	case 0xC4: // SET 0,H
		programCounter++;
		opSet(0, regH);
		if (debug)
			opcodeRead += "SET 0, H";
		durationInCycles = 8;
		break;
	case 0xC5: // SET 0,L
		programCounter++;
		opSet(0, regL);
		if (debug)
			opcodeRead += "SET 0, L";
		durationInCycles = 8;
		break;
	case 0xC6: // SET 0,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(0, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 0, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xC7: // SET 0,A
		programCounter++;
		opSet(0, regA);
		if (debug)
			opcodeRead += "SET 0, A";
		durationInCycles = 8;
		break;
	case 0xC8: // SET 1,B
		programCounter++;
		opSet(1, regB);
		if (debug)
			opcodeRead += "SET 1, B";
		durationInCycles = 8;
		break;
	case 0xC9: // SET 1,C
		programCounter++;
		opSet(1, regC);
		if (debug)
			opcodeRead += "SET 1, C";
		durationInCycles = 8;
		break;
	case 0xCA: // SET 1,D
		programCounter++;
		opSet(1, regD);
		if (debug)
			opcodeRead += "SET 1, D";
		durationInCycles = 8;
		break;
	case 0xCB: // SET 1,E
		programCounter++;
		opSet(1, regE);
		if (debug)
			opcodeRead += "SET 1, E";
		durationInCycles = 8;
		break;
	case 0xCC: // SET 1,H
		programCounter++;
		opSet(1, regH);
		if (debug)
			opcodeRead += "SET 1, H";
		durationInCycles = 8;
		break;
	case 0xCD: // SET 1,L
		programCounter++;
		opSet(1, regL);
		if (debug)
			opcodeRead += "SET 1, L";
		durationInCycles = 8;
		break;
	case 0xCE: // SET 1,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(1, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 1, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xCF: // SET 1,A
		programCounter++;
		opSet(1, regA);
		if (debug)
			opcodeRead += "SET 1, A";
		durationInCycles = 8;
		break;
	case 0xD0: // SET 2,B
		programCounter++;
		opSet(2, regB);
		if (debug)
			opcodeRead += "SET 2, B";
		durationInCycles = 8;
		break;
	case 0xD1: // SET 2,C
		programCounter++;
		opSet(2, regC);
		if (debug)
			opcodeRead += "SET 2, C";
		durationInCycles = 8;
		break;
	case 0xD2: // SET 2,D
		programCounter++;
		opSet(2, regD);
		if (debug)
			opcodeRead += "SET 2, D";
		durationInCycles = 8;
		break;
	case 0xD3: // SET 2,E
		programCounter++;
		opSet(2, regE);
		if (debug)
			opcodeRead += "SET 2, E";
		durationInCycles = 8;
		break;
	case 0xD4: // SET 2,H
		programCounter++;
		opSet(2, regH);
		if (debug)
			opcodeRead += "SET 2, H";
		durationInCycles = 8;
		break;
	case 0xD5: // SET 2,L
		programCounter++;
		opSet(2, regL);
		if (debug)
			opcodeRead += "SET 2, L";
		durationInCycles = 8;
		break;
	case 0xD6: // SET 2,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(2, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 2, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xD7: // SET 2,A
		programCounter++;
		opSet(2, regA);
		if (debug)
			opcodeRead += "SET 2, A";
		durationInCycles = 8;
		break;
	case 0xD8: // SET 3,B
		programCounter++;
		opSet(3, regB);
		if (debug)
			opcodeRead += "SET 3, B";
		durationInCycles = 8;
		break;
	case 0xD9: // SET 3,C
		programCounter++;
		opSet(3, regC);
		if (debug)
			opcodeRead += "SET 3, C";
		durationInCycles = 8;
		break;
	case 0xDA: // SET 3,D
		programCounter++;
		opSet(3, regD);
		if (debug)
			opcodeRead += "SET 3, D";
		durationInCycles = 8;
		break;
	case 0xDB: // SET 3,E
		programCounter++;
		opSet(3, regE);
		if (debug)
			opcodeRead += "SET 3, E";
		durationInCycles = 8;
		break;
	case 0xDC: // SET 3,H
		programCounter++;
		opSet(3, regH);
		if (debug)
			opcodeRead += "SET 3, H";
		durationInCycles = 8;
		break;
	case 0xDD: // SET 3,L
		programCounter++;
		opSet(3, regL);
		if (debug)
			opcodeRead += "SET 3, L";
		durationInCycles = 8;
		break;
	case 0xDE: // SET 3,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(3, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 3, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xDF: // SET 3,A
		programCounter++;
		opSet(3, regA);
		if (debug)
			opcodeRead += "SET 3, A";
		durationInCycles = 8;
		break;
	case 0xE0: // SET 4,B
		programCounter++;
		opSet(4, regB);
		if (debug)
			opcodeRead += "SET 4, B";
		durationInCycles = 8;
		break;
	case 0xE1: // SET 4,C
		programCounter++;
		opSet(4, regC);
		if (debug)
			opcodeRead += "SET 4, C";
		durationInCycles = 8;
		break;
	case 0xE2: // SET 4,D
		programCounter++;
		opSet(4, regD);
		if (debug)
			opcodeRead += "SET 4, D";
		durationInCycles = 8;
		break;
	case 0xE3: // SET 4,E
		programCounter++;
		opSet(4, regE);
		if (debug)
			opcodeRead += "SET 4, E";
		durationInCycles = 8;
		break;
	case 0xE4: // SET 4,H
		programCounter++;
		opSet(4, regH);
		if (debug)
			opcodeRead += "SET 4, H";
		durationInCycles = 8;
		break;
	case 0xE5: // SET 4,L
		programCounter++;
		opSet(4, regL);
		if (debug)
			opcodeRead += "SET 4, L";
		durationInCycles = 8;
		break;
	case 0xE6: // SET 4,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(4, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 4, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xE7: // SET 4,A
		programCounter++;
		opSet(4, regA);
		if (debug)
			opcodeRead += "SET 4, A";
		durationInCycles = 8;
		break;
	case 0xE8: // SET 5,B
		programCounter++;
		opSet(5, regB);
		if (debug)
			opcodeRead += "SET 5, B";
		durationInCycles = 8;
		break;
	case 0xE9: // SET 5,C
		programCounter++;
		opSet(5, regC);
		if (debug)
			opcodeRead += "SET 5, C";
		durationInCycles = 8;
		break;
	case 0xEA: // SET 5,D
		programCounter++;
		opSet(5, regD);
		if (debug)
			opcodeRead += "SET 5, D";
		durationInCycles = 8;
		break;
	case 0xEB: // SET 5,E
		programCounter++;
		opSet(5, regE);
		if (debug)
			opcodeRead += "SET 5, E";
		durationInCycles = 8;
		break;
	case 0xEC: // SET 5,H
		programCounter++;
		opSet(5, regH);
		if (debug)
			opcodeRead += "SET 5, H";
		durationInCycles = 8;
		break;
	case 0xED: // SET 5,L
		programCounter++;
		opSet(5, regL);
		if (debug)
			opcodeRead += "SET 5, L";
		durationInCycles = 8;
		break;
	case 0xEE: // SET 5,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(5, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 5, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xEF: // SET 5,A
		programCounter++;
		opSet(5, regA);
		if (debug)
			opcodeRead += "SET 5, A";
		durationInCycles = 8;
		break;
	case 0xF0: // SET 6,B
		programCounter++;
		opSet(6, regB);
		if (debug)
			opcodeRead += "SET 6, B";
		durationInCycles = 8;
		break;
	case 0xF1: // SET 6,C
		programCounter++;
		opSet(6, regC);
		if (debug)
			opcodeRead += "SET 6, C";
		durationInCycles = 8;
		break;
	case 0xF2: // SET 6,D
		programCounter++;
		opSet(6, regD);
		if (debug)
			opcodeRead += "SET 6, D";
		durationInCycles = 8;
		break;
	case 0xF3: // SET 6,E
		programCounter++;
		opSet(6, regE);
		if (debug)
			opcodeRead += "SET 6, E";
		durationInCycles = 8;
		break;
	case 0xF4: // SET 6,H
		programCounter++;
		opSet(6, regH);
		if (debug)
			opcodeRead += "SET 6, H";
		durationInCycles = 8;
		break;
	case 0xF5: // SET 6,L
		programCounter++;
		opSet(6, regL);
		if (debug)
			opcodeRead += "SET 6, L";
		durationInCycles = 8;
		break;
	case 0xF6: // SET 6,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(6, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 6, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xF7: // SET 6,A
		programCounter++;
		opSet(6, regA);
		if (debug)
			opcodeRead += "SET 6, A";
		durationInCycles = 8;
		break;
	case 0xF8: // SET 7,B
		programCounter++;
		opSet(7, regB);
		if (debug)
			opcodeRead += "SET 7, B";
		durationInCycles = 8;
		break;
	case 0xF9: // SET 7,C
		programCounter++;
		opSet(7, regC);
		if (debug)
			opcodeRead += "SET 7, C";
		durationInCycles = 8;
		break;
	case 0xFA: // SET 7,D
		programCounter++;
		opSet(7, regD);
		if (debug)
			opcodeRead += "SET 7, D";
		durationInCycles = 8;
		break;
	case 0xFB: // SET 7,E
		programCounter++;
		opSet(7, regE);
		if (debug)
			opcodeRead += "SET 7, E";
		durationInCycles = 8;
		break;
	case 0xFC: // SET 7,H
		programCounter++;
		opSet(7, regH);
		if (debug)
			opcodeRead += "SET 7, H";
		durationInCycles = 8;
		break;
	case 0xFD: // SET 7,L
		programCounter++;
		opSet(7, regL);
		if (debug)
			opcodeRead += "SET 7, L";
		durationInCycles = 8;
		break;
	case 0xFE: // SET 7,(HL)
	{
		programCounter++;
		uint16_t addr = getHLReg();
		uint8_t res = memMng.readByte(addr);
		opSet(7, res);
		memMng.writeByte(addr, res);
		if (debug)
			opcodeRead += "SET 7, (HL)";
		durationInCycles = 16;
		break;
	}
	case 0xFF: // SET 7,A
		programCounter++;
		opSet(7, regA);
		if (debug)
			opcodeRead += "SET 7, A";
		durationInCycles = 8;
		break;
	default :
		std::cout << "Unsupported prefix for CB opcode "<< (uint16_t)prefix << " At address" << programCounter  << std::endl;
		return -1;
	}
	return durationInCycles;
}