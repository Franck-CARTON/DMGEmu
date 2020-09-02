#include "pch.h"
#include "DMG_Input.h"


DMG_Input::DMG_Input(memoryManager &mMng) : memMng(mMng)
{
	aPressed = false;
	bPressed = false;
	startPressed = false;
	selectPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;
}


DMG_Input::~DMG_Input()
{
}

void DMG_Input::setkeyDownStatus(bool status)
{
	downPressed = status;
}

void DMG_Input::setkeyUpStatus(bool status)
{
	upPressed = status;
}

void DMG_Input::setkeyLeftStatus(bool status)
{
	leftPressed = status;
}

void DMG_Input::setkeyRightStatus(bool status)
{
	rightPressed = status;
}

void DMG_Input::setkeyAStatus(bool status)
{
	aPressed = status;
}

void DMG_Input::setkeyBStatus(bool status)
{
	bPressed = status;
}

void DMG_Input::setkeySelectStatus(bool status)
{
	selectPressed = status;
}

void DMG_Input::setkeyStartStatus(bool status)
{
	startPressed = status;
}

void DMG_Input::upateInputRegister()
{
	uint8_t inputReg = memMng.readByte(0xff00, true);
	inputReg &= 0x30;

	// Both buttons & directions
	if (inputReg == 0x00)
	{
		if (!startPressed || !downPressed)
		{
			inputReg |= 0x08;
		}
		if (!selectPressed || !upPressed)
		{
			inputReg |= 0x04;
		}
		if (!bPressed || !leftPressed)
		{
			inputReg |= 0x02;
		}
		if (!aPressed || !rightPressed)
		{
			inputReg |= 0x01;
		}
	}
	// Buttons :
	if (inputReg == 0x10)
	{
		if (!startPressed)
		{
			inputReg |= 0x08;
		}
		if (!selectPressed)
		{
			inputReg |= 0x04;
		}
		if (!bPressed)
		{
			inputReg |= 0x02;
		}
		if (!aPressed)
		{
			inputReg |= 0x01;
		}
	}
	// Direction :
	else if (inputReg == 0x20)
	{
		if (!downPressed)
		{
			inputReg |= 0x08;
		}
		if (!upPressed)
		{
			inputReg |= 0x04;
		}
		if (!leftPressed)
		{
			inputReg |= 0x02;
		}
		if (!rightPressed)
		{
			inputReg |= 0x01;
		}
	}

	memMng.writeByte(0xff00, inputReg, true);
}
