// PwnAdventure3Ex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase, firstLevelPtr, manaAddr, healthAddr, speedAddr, jumpAddr = 0;
	bool bMana = false, bHealth = false, bSpeed = false, bJump = false;

	const int newMana = 1337;
	const int newHealth = 420;
	const int defaultMana = 100;
	const int defaultHealth = 100;
	const float newSpeed = 1000;
	const float defaultSpeed = 200;
	const float newJump = 2000;
	const float defaultJump = 420;

	DWORD procId = GetProcId(L"PwnAdventure3-Win32-Shipping.exe");

	if (procId)
	{
		moduleBase = GetModuleBaseAddress(procId, L"GameLogic.dll");							//get the base address of process PwnAdventure3-Win32-Shipping.exe

		firstLevelPtr = moduleBase + 0x00097D7C;												//add relative offset to get the first level pointer

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);								//need to use PROCESS_ALL_ACCESS for GetExitCodeProcess

		manaAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0xbc });					// determine dynamic addresses based off of static pointers
		healthAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x00 }) - 0x40;
		speedAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x120 });
		jumpAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x124 });

	}
	else
	{																							
		std::cout << "[1] Process not found, press enter to exit\n";								//if pwn adventure process is not found, exit
		getchar();
		return 0;
	}

	DWORD dwExit = 0;	
																			//if process has not terminated and function succeeds, status returned is STILL_ACTIVE
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)	//https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodeprocess 
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)								// listen for hotkey presses, set flags
		{
			bMana = !bMana;
			if (bMana)
			{
				std::cout << "Mana <ON>\n";
			}
			else
			{
				std::cout << "Mana <OFF>\n";
				mem::PatchEx((BYTE*)manaAddr, (BYTE*)&defaultMana, sizeof(defaultMana), hProcess);
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bHealth = !bHealth;
			if (bHealth)
			{
				std::cout << "Health <ON>\n";
			}
			else
			{
				std::cout << "Health <OFF>\n";
				mem::PatchEx((BYTE*)healthAddr, (BYTE*)&defaultHealth, sizeof(defaultHealth), hProcess);
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bSpeed = !bSpeed;
			if (bSpeed)
			{
				std::cout << "Speed <ON>\n";
				mem::PatchEx((BYTE*)speedAddr, (BYTE*)&newSpeed, sizeof(newSpeed), hProcess);
			}
			else
			{
				std::cout << "Speed <OFF>\n";
				mem::PatchEx((BYTE*)speedAddr, (BYTE*)&defaultSpeed, sizeof(newSpeed), hProcess);
			}

		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			bJump = !bJump;
			if (bJump)
			{
				std::cout << "Jump <ON>\n";
				mem::PatchEx((BYTE*)jumpAddr, (BYTE*)&newJump, sizeof(newJump), hProcess);
			} 
			else
			{
				std::cout << "Jump <OFF>\n";
				mem::PatchEx((BYTE*)jumpAddr, (BYTE*)&defaultJump, sizeof(newJump), hProcess);
			}
		}
		if (GetAsyncKeyState(VK_INSERT) & 1)													//this will exit the hack
		{
			return 0;
		}

		// continuous write or freeze
		if (bMana)
		{
			mem::PatchEx((BYTE*)manaAddr, (BYTE*)&newMana, sizeof(newMana), hProcess);			// flag is set, write to mana address every 10 ms
		}
		if (bHealth)
		{
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newHealth, sizeof(newHealth), hProcess);	// flag is set, write to health address every 10 ms
		}
		Sleep(10);
	}

	std::cout << "[2] Process not found, press enter to exit\n";
	getchar();

	return 0;

}