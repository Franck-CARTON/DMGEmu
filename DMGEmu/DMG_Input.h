#pragma once
#include "memoryManager.h"
#include <stdint.h>

class DMG_Input
{
public:
	DMG_Input(memoryManager &mMng);
	~DMG_Input();
	void upateInputRegister();
	void setkeyDownStatus(bool status);
	void setkeyUpStatus(bool status);
	void setkeyLeftStatus(bool status);
	void setkeyRightStatus(bool status);
	void setkeyAStatus(bool status);
	void setkeyBStatus(bool status);
	void setkeySelectStatus(bool status);
	void setkeyStartStatus(bool status);

private :
	memoryManager &memMng;
	bool aPressed;
	bool bPressed;
	bool startPressed;
	bool selectPressed;
	bool upPressed;
	bool downPressed;
	bool leftPressed;
	bool rightPressed;
};

