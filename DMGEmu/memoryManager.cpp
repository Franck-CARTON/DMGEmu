#include "pch.h"
#include "memoryManager.h"

#include <fstream>
#include <ios>
#include <string>
#include <iostream>


memoryManager::memoryManager()
{
	DMAInProgress = false;
	memory = new __int8[65536];
	// Boot on DMG ROM
	memory[0xff50] = 0x00;
}

memoryManager::~memoryManager()
{
	// Free cartridge rom banks
	delete memory;
	for (int i = 0; i < ROMBanks.size(); i++)
	{
		delete ROMBanks[i];
	}
}

void memoryManager::readDMGRom(std::string& fileName)
{
	// TODO : Add a lot of check and error management
	std::ifstream romFile(fileName.c_str(), std::ios::binary);
	if (romFile.good())
	{
		romFile.read(DMGRom, 256);
	}
	else
	{
		std::cout << "Error while loading DMG Rom" << std::endl;
	}
}

bool memoryManager::loadCartridge(std::string& fileName)
{
	// TODO : Add a lot of check and error management
	std::cout << "On essai d'ouvrir " << fileName << std::endl;
	std::ifstream romFile(fileName.c_str(), std::ios::binary);
	int offset = 0;
	if (romFile.good())
	{

		for (int i = 0;(i < 32); i++)
		{
			romFile.read(memory+offset, 1024);
			offset += 1024;
		}

		// Now extract Info :
		char cartridgeName[12];
		for (int i = 0; i < 0xb; i++)
			cartridgeName[i] = memory[0x134 + i];

		std::string cName(cartridgeName);

		std::cout << "offset = " << std::hex << offset << std::endl;
		std::cout <<"Game name = "<< cName << std::endl;
		std::cout << "Type cartouche = " << std::hex << (uint16_t) memory[0x147] << std::endl;
	}

	return true;
}

void memoryManager::setDMAInProgress(bool enable)
{
	DMAInProgress = enable;
}

__int8 memoryManager::readByte(uint16_t addr, bool force)
{
	if (force) 
	{
		if (memory[DMGRomStatus] == 0 && (addr >= DMGRomLowAddr) && (addr <= DMGRomHiAddr))
		{
			return DMGRom[addr];
		}
		else
		{
			return memory[addr];
		}
	}
	else
	{
		if (DMAInProgress)
		{
			if ((addr >= HIRAMLowAddr) && (addr <= HIRAMHiAddr))
			{
				return memory[addr];
			}
			else
			{
				return 0xff; // WARNING : Not to sure about this. Further investigation needed
			}
		}
		else if (memory[DMGRomStatus] == 0 && (addr >= DMGRomLowAddr) && (addr <= DMGRomHiAddr))
		{
			return DMGRom[addr];
		}
		else
		{
			return memory[addr];
		}
	}
}

void memoryManager::writeByte(uint16_t addr, __int8 val, bool force)
{
	// If force equal true then you can write wherever you want ^^
	if (force)
	{
		memory[addr] = val;
	}
	// If DMA in progress it is only possible to write into High RAM memory
	else if (DMAInProgress)
	{
		if ((addr >= HIRAMLowAddr) && (addr <= HIRAMHiAddr))
		{
			memory[addr] = val;
		}
		// WARNING : Should we do something in else case ? Glitch emulation ?
	}
	else 
	{
		__int8 videoMode = memory[0xFF41] & 0x3;
		__int8 LCDenable = memory[0xFF40] & 0x80;
		// Working RAM is always accessible if there is no DMA transfer
		if (addr >= workingRAMLowAddr && addr <= workingRAMHiAddr)
		{
			memory[addr] = val;
		}
		// High RAM is always accessible if there is no DMA transfer
		else if ((addr >= HIRAMLowAddr) && (addr <= HIRAMHiAddr))
		{
			memory[addr] = val;
		}
		// IO registers are always accessible if there is no DMA transfer
		else if (addr >= 0xff00 && addr < 0xff7F)
		{
			memory[addr] = val;
		}
		else if (((videoMode != 3) || (!LCDenable)) && addr >= VRAMLowAddr && addr <= VRAMHiAddr)
		{
			memory[addr] = val;
		}
		else if (((videoMode < 2) || (!LCDenable) ) && addr >= OAMLowAddr && addr <= OAMHiAddr)
		{
			//std::cout << "writing to OAM in mode " << (int)videoMode << "value : " << std::hex << (int) val << "at addr :" << std::hex << addr<< std::endl;
			memory[addr] = val;
		}
		else if (((videoMode > 2) || ( LCDenable)) && addr >= OAMLowAddr && addr <= OAMHiAddr)
		{
			//std::cout << "Forbidden write to OAM in mode " << (int)videoMode << std::endl;
		}
		else if (addr == IRQEnableAddr)
		{
			memory[addr] = val;
		}
	}
}
