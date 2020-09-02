#pragma once
#include "memoryManager.h"
#include <stdint.h>
#include <allegro5/allegro.h>
class DMG_ppu
{
public:
	DMG_ppu(memoryManager &mMng);
	~DMG_ppu();

	bool run(int nCycle);
	void initIORegister();
	bool checkLCDDisplayStatus();
	void setLCDReadOnlyRegister();

	void readPalettes();
	void getColorFromValue(ALLEGRO_COLOR &dest, uint8_t value);

	void DrawBackground(uint16_t x, uint16_t y);
	void findOverlapingSprites(uint16_t y);
	void DrawSprite(uint16_t x, uint16_t y);

	// For debug 
	bool readVblankStatus();

	static const  uint16_t LCDC_CTRL_REG = 0xff40;
	static const  uint16_t LCDC_STATUS_REG = 0xff41;
	static const  uint16_t SCROLL_Y_REG = 0xff42;
	static const  uint16_t SCROLL_X_REG = 0xff43;
	static const  uint16_t LY_REG = 0xff44;
	static const  uint16_t LY_COMPARE_REG = 0xff45;
	static const  uint16_t DMA_REG = 0xff46;
	static const  uint16_t BG_PALETTE_REG = 0xff47;
	static const  uint16_t OBJ_PALETTE_0_REG = 0xff48;
	static const  uint16_t OBJ_PALETTE_1_REG = 0xff49;
	static const  uint16_t WIN_Y_REG = 0xff4A;
	static const  uint16_t WIN_X_REG = 0xff4B;

	static const int NB_DOT_MODE2 = 80;   // scanline
	static const int NB_DOT_MODE3 = 291;  // VRAM OAM reading
	static const int NB_DOT_MODE0 = 85;   // hBlank
	static const int NB_DOT_MODE1 = 4560; // vBlank
	static const int NB_DOT_SCANLINE = NB_DOT_MODE2 + NB_DOT_MODE3 + NB_DOT_MODE0;
	static const int DMG_NB_LINE = 144;

	memoryManager &memMng;

	int ppuCycleCounter;
	int currentMode;
	int currentLine;
	int currentX;
	bool currentCoincidenceFlag;
	bool firstFrameOK;
	bool previousDisplayStatus;

	// DMA Mgmt
	bool DMAInProgress;
	uint16_t DMABaseAddr;
	uint8_t DMACount;

	// For debug
	bool vblankDebugStatus;

	// Sprite list :
	uint8_t sprites[10]; // Max 10 sprite by line
	uint8_t nbSpriteToDisplay;

	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *screen;
	ALLEGRO_COLOR white;
	ALLEGRO_COLOR grayL;
	ALLEGRO_COLOR grayD;
	ALLEGRO_COLOR black;

	ALLEGRO_COLOR bgrPal[4];
	ALLEGRO_COLOR obj0Pal[4];
	ALLEGRO_COLOR obj1Pal[4];
};

