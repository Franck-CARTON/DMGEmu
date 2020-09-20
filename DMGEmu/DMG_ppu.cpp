#include "pch.h"
#include "DMG_ppu.h"

#include <cstring>
#include <iostream>
#include <allegro5/allegro.h>


/*
 * TODO LIST : 
 * Add a mehod to extract Sprite page as an image for debug
 */

DMG_ppu::DMG_ppu(memoryManager &mMng) : memMng(mMng)
{
	initIORegister();
	previousDisplayStatus = false;
	firstFrameOK = false;
	display = al_get_current_display();
	screen = al_get_backbuffer(display);
	al_set_target_bitmap(screen);
	black = al_map_rgb(24, 48, 16);
	grayD = al_map_rgb(48, 128, 48);
	grayL = al_map_rgb(92, 192, 72);
	white = al_map_rgb(160, 208, 144);
	nbSpriteToDisplay = 0;
	vblankDebugStatus = false;
}

DMG_ppu::~DMG_ppu()
{
}

void DMG_ppu::initIORegister()
{
	ppuCycleCounter = 0;
	currentCoincidenceFlag = 0;
	currentMode = 0;
	currentLine = 0;
	memMng.writeByte(LCDC_CTRL_REG, 0x0, true);
	memMng.writeByte(LCDC_STATUS_REG, 0x0, true);
	memMng.writeByte(SCROLL_Y_REG, 0x0, true);
	memMng.writeByte(SCROLL_X_REG, 0x0, true);
	memMng.writeByte(LY_REG, 0x0, true);
	memMng.writeByte(LY_COMPARE_REG, 0x0, true);
	memMng.writeByte(DMA_REG, 0x0, true);
	memMng.writeByte(BG_PALETTE_REG, 0x0, true);
	memMng.writeByte(OBJ_PALETTE_0_REG, 0x0, true);
	memMng.writeByte(OBJ_PALETTE_1_REG, 0x0, true);
	memMng.writeByte(WIN_Y_REG, 0x0, true);
	memMng.writeByte(WIN_X_REG, 0x0, true);
}

bool DMG_ppu::readVblankStatus()
{
	bool tmp = vblankDebugStatus;
	vblankDebugStatus = false;
	return tmp;
}

bool DMG_ppu::checkLCDDisplayStatus()
{
	unsigned char lcdReg = memMng.readByte(LCDC_CTRL_REG, true);
	bool isLCDEnable = ((lcdReg & 0x80) == 0x80);

	if (previousDisplayStatus != isLCDEnable)
	{
		previousDisplayStatus = isLCDEnable;
		if (isLCDEnable)
		{
			// Switch the screen on
			std::cout << "LCDC Display enable" << std::endl;
			al_clear_to_color(al_map_rgb(255, 255, 255));
			return true;
		}
		else
		{
			// Switch the screen off
			ppuCycleCounter = 0;
			currentCoincidenceFlag = 0;
			currentMode = 2;
			currentLine = 0;
			std::cout << "LCDC Display disable" << std::endl;
			al_clear_to_color(al_map_rgb(0, 0, 0));
			return false;
		}
	}
}

void DMG_ppu::setLCDReadOnlyRegister()
{
	// Check IO Read only status
	unsigned char statusReg = memMng.readByte(LCDC_STATUS_REG, true);

	unsigned char mask = 0xf8 + currentMode;
	if (currentCoincidenceFlag)
		mask += 0x4;

	statusReg = statusReg & mask;
	memMng.writeByte(LCDC_STATUS_REG, statusReg, true);

	// Set LY to current line
	memMng.writeByte(LY_REG, currentLine, true);
}

// #TODO : Check (rewrite ?) this automat and add more comment :
bool DMG_ppu::run(int nCycle)
{
	// Check if DMA is activated :
	uint8_t DMAAddr = (uint8_t) memMng.readByte(DMA_REG, true);
	if (DMAAddr != 0)
	{
		//std::cout << "DMA in progress..." << std::endl;
		DMAInProgress = true;
		DMABaseAddr = DMAAddr << 8;
		//std::cout << "Start addr = " << DMABaseAddr << std::endl;
		DMACount = 0;
		memMng.writeByte(DMA_REG, 0, true);
		memMng.setDMAInProgress(true);
	}

	// Handle DMA
	if (DMAInProgress)
	{
		uint8_t nbOctet = nCycle / 4;
		//std::cout << "DMA Copy " << nbOctet << "octets" <<std::endl;
		for (int i = 0; (i < nbOctet) && (DMACount + i < 160); i++)
		{
			uint8_t valToCopy = (uint8_t)memMng.readByte(DMABaseAddr+DMACount+i, true);
			memMng.writeByte(0xFE00+DMACount + i, valToCopy, true);
		}
		DMACount += nbOctet;
		if (DMACount >= 160)
		{
			DMAInProgress = false;
			DMABaseAddr = 0;
			DMACount = 0;
			memMng.setDMAInProgress(false);
		}
	}

	bool displayEnable = checkLCDDisplayStatus();
	bool vBlank = false;
	if (displayEnable)
	{
		int prevPpuCycleCounter = ppuCycleCounter;
		ppuCycleCounter += nCycle;

		// Behave according to current mode :
		unsigned char LCDStatusReg = memMng.readByte(LCDC_STATUS_REG, true);
		switch (currentMode)
		{
		case 2: // Mode 2 
		{
			if (ppuCycleCounter > NB_DOT_MODE2)
			{
				currentMode = 3;
				findOverlapingSprites(currentLine);
				readPalettes();
				ppuCycleCounter = ppuCycleCounter - NB_DOT_MODE2;
				currentX = 0;
			}
			break;
		}
		case 3: // Mode 3 - Drawing on screen
		{
			for (int i = currentX; (i < ppuCycleCounter) & (i < 160); i++)
			{
				DrawBackground(i, currentLine);
				DrawSprite(i, currentLine);
			}
			currentX = ppuCycleCounter;

			if (ppuCycleCounter > NB_DOT_MODE3)
			{
				currentMode = 0;
				ppuCycleCounter = ppuCycleCounter - NB_DOT_MODE3;
			}
			break;
		}
		case 0: // Mode 0 - Horizontal Blank
		{

			if (ppuCycleCounter > NB_DOT_MODE0)
			{
				currentLine++;
				// Check LYC
				uint8_t lineC = memMng.readByte(LY_COMPARE_REG, true);
				if (currentLine == lineC)
				{
					// Raise IRQ
					uint8_t irqFlag = memMng.readByte(0xff0f, true);
					irqFlag |= 0x2;
					memMng.writeByte(0xff0f, irqFlag, true);
				}

				ppuCycleCounter = ppuCycleCounter - NB_DOT_MODE0;

				if (currentLine == DMG_NB_LINE)
				{
					firstFrameOK = true;
					currentMode = 1;
					al_unlock_bitmap(screen);
					al_flip_display();

					vBlank = true;
					// Raise IRQ
					uint8_t irqFlag = memMng.readByte(0xff0f, true);
					irqFlag |= 0x1;
					vblankDebugStatus = true;
					//std::cout << "Enable vBlank " << std::hex << (uint16_t)irqFlag <<std::endl;
					memMng.writeByte(0xff0f, irqFlag, true);
				}
				else
				{
					currentMode = 2;
				}
			}
			break;
		}
		case 1: // Mode 0 - Vertical Blank (or line 144 to 153)
		{
			if (ppuCycleCounter > NB_DOT_MODE1)
			{
				al_lock_bitmap(screen, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
				currentMode = 2;
				currentLine = 0;
				ppuCycleCounter = ppuCycleCounter - NB_DOT_MODE0;

			}
			else
			{
				if ((prevPpuCycleCounter%NB_DOT_SCANLINE) > (ppuCycleCounter%NB_DOT_SCANLINE))
				{
					currentLine++;
					// Check LYC
					uint8_t lineC = memMng.readByte(LY_COMPARE_REG, true);
					if (currentLine == lineC)
					{
						// Raise IRQ
						uint8_t irqFlag = memMng.readByte(0xff0f, true);
						irqFlag &= 0x2;
						memMng.writeByte(0xff0f, irqFlag, true);
					}
				}
			}
			break;
		}
		}
	}

	// Set readOnly LCD registers & flag with their value
	setLCDReadOnlyRegister();

	if (vBlank)
		return true;
	else
		return false;
}

void DMG_ppu::DrawBackground(uint16_t x, uint16_t y)
{
	unsigned char lcdReg = memMng.readByte(LCDC_CTRL_REG, true);
	unsigned char is8000Mode = lcdReg & 0x10;

	// Is background enable ?
	bool bckgrEnable = ((lcdReg & 0x20) != 0x20);

	if (bckgrEnable && firstFrameOK )
	{
		uint16_t posX = x + memMng.readByte(SCROLL_X_REG, true) % 256; // A calculer (le %256) une fois pour le background pendant mode 2 et corriger uniquement si SCX change ?
		uint16_t posY = y + memMng.readByte(SCROLL_Y_REG, true) % 256; // A calculer une fois pour le background pendant mode 2 et corriger uniquement si SCY change ?

		// Wich tiles should be read ?
		uint16_t tileRow = posY/8; // A calculer une fois pour le background
		uint16_t tileCol = posX/8;
		uint16_t tileOffset = 32*tileRow + tileCol; //  A calculer une fois pour le background

		// TODO : Addr shoulb computed using the right tilemap base address 
		uint16_t addr = 0x9800 + tileOffset;

		// retrieve Tiles
		// TODO : Becarfull in some case, tile id may be a signed char !!!!
		unsigned char tileId = memMng.readByte(addr, true);

		// Wich pixel should be read ?
		uint16_t xPix = posX % 8; 
		uint16_t yPix = 2 *(posY % 8); // A calculer une fois pour le background

		uint16_t baseAddr = 0x8000;

		if (is8000Mode != 0 )
			baseAddr = 0x8000 + 16*tileId + yPix;
		else
		{
			if (tileId < 128)
				baseAddr = 0x9000 + 16*tileId + yPix;
			else
				baseAddr = 0x9000 - (16 * (256-tileId)) + yPix;
		}
		//uint16_t baseAddr = 0x8000 + tileMemOffset + yPix;
		unsigned char tileLineDescr1 = memMng.readByte(baseAddr, true);
		unsigned char tileLineDescr2 = memMng.readByte(baseAddr + 1, true);

		uint16_t mask = 0x80 >> xPix;
		uint16_t maskInv = (7 - xPix);
		uint16_t colorIdxLow  = (tileLineDescr1 & mask) >> maskInv;
		uint16_t colorIdxHigh = (tileLineDescr2 & mask) >> maskInv;
		uint16_t  colorIdx = colorIdxLow + 2 * colorIdxHigh;

		al_put_pixel(x, y, bgrPal[colorIdx]);
	}
}

void DMG_ppu::DrawSprite(uint16_t x, uint16_t y)
{
	// For each sprite :
	for (int i = 0; i < nbSpriteToDisplay; i++)
	{
		uint8_t spriteXPos = (uint8_t)memMng.readByte(0xfe00 + 4 * sprites[i] + 1, true);
		uint8_t spriteAttributes = (uint8_t)memMng.readByte(0xfe00 + 4 * sprites[i] + 3, true);
		bool xFlip = spriteAttributes & 0x20;

		if (xFlip)
			spriteXPos = 160 - spriteXPos;

		if (x >= spriteXPos - 8 && x < spriteXPos)
		{

			uint8_t spriteYPos = (uint8_t)memMng.readByte(0xfe00 + 4 * sprites[i], true);
			uint16_t spriteNumber = 16 * (uint8_t)memMng.readByte(0xfe00 + 4 * sprites[i] + 2, true);
			uint8_t spriteAttributes = (uint8_t)memMng.readByte(0xfe00 + 4 * sprites[i] + 3, true);

			bool xFlip = spriteAttributes & 0x20;
			bool yFlip = spriteAttributes & 0x40;

			// Wich pixel from the sprite should be read ?
			uint16_t xPix = x - (spriteXPos - 8);
			if (xFlip)
				xPix = 7 - (xPix-8);

			uint16_t yPix = 2*(y - (spriteYPos - 16));
			if (yFlip)
				yPix = 7 - yPix;

			uint16_t baseAddr = 0x8000 + spriteNumber + yPix;
			unsigned char tileLineDescr1 = (uint8_t)memMng.readByte(baseAddr, true);
			unsigned char tileLineDescr2 = (uint8_t)memMng.readByte(baseAddr + 1, true);

			uint16_t mask = 0x80 >> xPix;
			uint16_t maskInv = (7 - xPix);
			uint16_t colorIdxLow = (tileLineDescr1 & mask) >> maskInv;
			uint16_t colorIdxHigh = (tileLineDescr2 & mask) >> maskInv;
			uint16_t  colorIdx = colorIdxLow + 2 * colorIdxHigh;

			if (colorIdx != 0)
				al_put_pixel(x, y, obj0Pal[colorIdx]);
		}
	}
}

void DMG_ppu::findOverlapingSprites(uint16_t y)
{
	nbSpriteToDisplay = 0;
	for (int i = 0; i < 40; i++)
	{
		// Read sprite info :
		uint8_t spriteYPos = memMng.readByte(0xfe00 + 4 * i, true);
		// Easy way, only 8*8 sprites 
		if (spriteYPos != 0  && y >= (spriteYPos-16)  && y < (spriteYPos -8) && nbSpriteToDisplay < 10)
		{
			//std::cout << "Overlapping Sprite for Y = " << y << " is sprite number : " << i << ", at pos : " << (int)spriteYPos  <<std::endl;
			sprites[nbSpriteToDisplay] = i;
			nbSpriteToDisplay++;
		}
	}
}

void DMG_ppu::getColorFromValue(ALLEGRO_COLOR &dest, uint8_t value)
{
	//std::cout << "color value = " << (int) value << std::endl;
	switch (value) {
	case 0: dest = white;
		break;
	case 1: dest = grayL;
		break;
	case 2: dest = grayD;
		break;
	default : 
		dest = black;
	}
}

void DMG_ppu::readPalettes()
{

	// Recupere la palette BG
	//std::cout << "Retrieve background palette " << std::endl;
	uint8_t paletteRaw = memMng.readByte(BG_PALETTE_REG, true);

	uint8_t col = paletteRaw & 0x3;
	getColorFromValue(bgrPal[0], col);

	col = (paletteRaw & 0xc) >> 2;
	getColorFromValue(bgrPal[1], col);

	col = (paletteRaw & 0x30) >> 4;
	getColorFromValue(bgrPal[2], col);

	col = (paletteRaw & 0xc0) >> 6;
	getColorFromValue(bgrPal[3], col);

	// Recupere la palette OBJ 1
	paletteRaw = memMng.readByte(OBJ_PALETTE_0_REG, true);
	//std::cout << "Retrieve obj1 palette " << std::hex << (uint16_t) paletteRaw << std::endl;

	col = paletteRaw & 0x3;
	getColorFromValue(obj0Pal[0], col);

	col = (paletteRaw & 0xc) >> 2;
	getColorFromValue(obj0Pal[1], col);

	col = (paletteRaw & 0x30) >> 4;
	getColorFromValue(obj0Pal[2], col);

	col = (paletteRaw & 0xc0) >> 6;
	getColorFromValue(obj0Pal[3], col);

	// Recupere la palette OBJ 2
	paletteRaw = memMng.readByte(OBJ_PALETTE_1_REG, true);
	//std::cout << "Retrieve obj2 palette " << std::hex << (uint16_t)paletteRaw << std::endl;

	col = paletteRaw & 0x3;
	getColorFromValue(obj1Pal[0], col);

	col = (paletteRaw & 0xc) >> 2;
	getColorFromValue(obj1Pal[1], col);

	col = (paletteRaw & 0x30) >> 4;
	getColorFromValue(obj1Pal[2], col);

	col = (paletteRaw & 0xc0) >> 6;
	getColorFromValue(obj1Pal[3], col);
}