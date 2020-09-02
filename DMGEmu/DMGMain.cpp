#include "pch.h"
#include "Zx80GB.h"
#include "DMG_ppu.h"
#include "DMG_Input.h"
#include "DMG_Timer.h"
#include "memoryManager.h"

#include <iostream>
#include <stdint.h>
#include <string>
#include <allegro5/allegro.h>
#include <Windows.h>

LARGE_INTEGER frequency;
LARGE_INTEGER t1, t2;
double elapsedTime;
ALLEGRO_KEYBOARD_STATE kbStatus;


void run(Zx80GB &cpu, DMG_Timer &timerMng, DMG_ppu &ppuMng, DMG_Input &inputMng, bool debug)
{
	// First run CPU :
	int durationInCycles = cpu.runZx80(debug);
	// Then the Timer :
	timerMng.run(durationInCycles);
	// And the PPU :
	bool vBlank = ppuMng.run(durationInCycles);

	// Force frame per sec to at most 1/60 sec :
	if (vBlank)
	{
		double frameRate = 1. / 60.;
		QueryPerformanceCounter(&t2);
		elapsedTime = (t2.QuadPart - t1.QuadPart)* 1000.0/ frequency.QuadPart;
		std::cout << "Elapsed time for computing = " << elapsedTime << std::endl;
		bool endFrame = false;

		// Horrible way to wait until the end of expected frame
		while (!endFrame)
		{
			QueryPerformanceCounter(&t2);
			elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / (double)frequency.QuadPart;
			if (elapsedTime > frameRate)
				endFrame = true;
		}
		QueryPerformanceCounter(&t1);
	}

	// Finaly Update Input register :
	// retrieve keyboard state :
	al_get_keyboard_state(&kbStatus);
	// Then poll button status :
	inputMng.setkeyDownStatus(al_key_down(&kbStatus, ALLEGRO_KEY_DOWN));
	inputMng.setkeyUpStatus(al_key_down(&kbStatus, ALLEGRO_KEY_UP));
	inputMng.setkeyLeftStatus(al_key_down(&kbStatus, ALLEGRO_KEY_LEFT));
	inputMng.setkeyRightStatus(al_key_down(&kbStatus, ALLEGRO_KEY_RIGHT));
	inputMng.setkeyAStatus(al_key_down(&kbStatus, ALLEGRO_KEY_A));
	inputMng.setkeyBStatus(al_key_down(&kbStatus, ALLEGRO_KEY_Z));
	inputMng.setkeySelectStatus(al_key_down(&kbStatus, ALLEGRO_KEY_ENTER));
	inputMng.setkeyStartStatus(al_key_down(&kbStatus, ALLEGRO_KEY_SPACE));
	// And update inputManager :
	inputMng.upateInputRegister();
}

int main()
{
	// Init Allegro :
	ALLEGRO_DISPLAY *display = NULL;

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to install allegro keyboard!\n");
		return -1;
	}

	display = al_create_display(160, 144);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	al_clear_to_color(al_map_rgb(255, 0, 255));
	al_flip_display();

	// Create instance for every DMG devices
	memoryManager memMng;
	Zx80GB cpu(memMng);
	DMG_ppu ppuMng(memMng);
	DMG_Input inputMng(memMng);
	DMG_Timer timerMng(memMng);

	// Read Boot and cartridge roms :
	std::string bootROMFile("D:\\dev\\Data\\Emulation\\DMG\\DMGBoot\\DMG_ROM.bin");
	std::string cartridgeFile("D:\\dev\\Data\\Emulation\\DMG\\DMGRom\\bpong.gb"); //bgbtest.gb

	memMng.readDMGRom(bootROMFile);
	memMng.loadCartridge(cartridgeFile);

	// For FPS computation
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	// #TODO : write a better interpreter (with thread management ?)

	std::cout << "DMG Zx80 CPU Emu" << std::endl;
	std::cout << "Enter command :" << std::endl;
	bool quit = false;
	while (quit != true)
	{
		std::cout << ":> ";
		std::string cmd;
		std::getline(std::cin, cmd);

		if (cmd.compare("quit") == 0)
			quit = true;
		else if (cmd.compare("next") == 0)
		{
			run(cpu, timerMng, ppuMng, inputMng, true);
		}
		else if (cmd.compare("pc") == 0)
		{
			std::cout << "programCounter = " << std::hex << cpu.programCounter << std::endl;
		}
		else
		{
			std::string delimiters = " ";
			size_t next = cmd.find_first_of(delimiters, 0);
			std::string field = cmd.substr(0, next);
			if (field.compare("read") == 0)
			{
				size_t current = next+1;
				next = cmd.find_first_of(delimiters, current);
				std::string field = cmd.substr(current, next-current);

				__int16 value = std::stoi(field, 0, 16);
				__int16 readFrom = value & 0xfff0;
				__int16 readTo = readFrom + 0x10;

				for (int i = readFrom; i < readTo; i++)
				{
					unsigned char c = memMng.readByte(i, true);
					__int16 c16 = (__int16)c;
					std::cout << std::hex << c16 << ", ";
				}
				std::cout << std::endl;
			}
			else if (field.compare("break") == 0)
			{
				size_t current = next + 1;
				next = cmd.find_first_of(delimiters, current);
				std::string field = cmd.substr(current, next - current);
				__int16 value = std::stoi(field, 0, 16);
				while (cpu.programCounter != value)
				{
					run(cpu, timerMng, ppuMng, inputMng, false);
				}
				std::cout << "run until pc equal " << value << std::endl;
			}
			else if (field.compare("vblank") == 0)
			{
				while (!ppuMng.readVblankStatus())
				{
					run(cpu, timerMng, ppuMng, inputMng, false);
				}
				std::cout << "run until vblanck " << std::endl;
			}
			else
				std::cout << "Unrecognized command" << std::endl;
		}
	}

	al_destroy_display(display);
}
