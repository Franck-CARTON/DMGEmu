#pragma once
#include <stdint.h>
#include <string>
#include<vector>

class memoryManager
{
public:
	memoryManager();
	~memoryManager();

	uint8_t readByte(uint16_t addr, bool force=false);
	void writeByte(uint16_t addr, uint8_t val, bool force = false);
	void readDMGRom(std::string& fileName);
	void setDMAInProgress(bool enable);
	bool loadCartridge(std::string& fileName);

private:

	std::vector<uint8_t*> ROMBanks;


	uint8_t DMGRom[256];
	uint8_t*memory;

	bool DMGRomEnabled;
	bool DMAInProgress;
	bool isOAMAccessEnable;
	bool isVRAMAccessEnable;

	static const int accessibleMemory = 0x100000;

	static const uint16_t DMGRomLowAddr = 0x0000;
	static const uint16_t DMGRomHiAddr = 0x00FF;

	static const uint16_t romBank0LowAddr = 0x0000;
	static const uint16_t romBank0HiAddr = 0x3FFF;
	static const uint16_t romOtherBankLowAddr = 0x4000;
	static const uint16_t romOtherBankHiAddr = 0x7FFF;

	static const uint16_t VRAMLowAddr = 0x8000;
	static const uint16_t VRAMHiAddr = 0x9FFF;

	static const uint16_t OAMLowAddr = 0xFE00;
	static const uint16_t OAMHiAddr = 0xFE9F;

	static const uint16_t HIRAMLowAddr = 0xFF80;
	static const uint16_t HIRAMHiAddr = 0xFFFE;

	static const uint16_t IRQEnableAddr = 0xFFFF;

	static const uint16_t workingRAMLowAddr = 0xC000;
	static const uint16_t workingRAMHiAddr = 0xDFFF;

	static const  uint16_t DMGRomStatus = 0xff50;
	static const  uint16_t LCDCStatus = 0xff41;
	static const  uint16_t vMode_mask = 0x03;
};

