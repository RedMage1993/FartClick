//*********************************
// Author: Fritz Ammon
// Date: 4 November 2014
// Program: FartClick
// Description: Makes a fart noise
// whenever a click is handled.
//*********************************

#pragma comment(lib, "Winmm")

#include "resource.h"
#include <ctime>
#include <cstdlib>
#include <Windows.h>
#include <mmsystem.h>

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
bool improveSleepAcc(bool = true);
LRESULT CALLBACK LLMProc(int, WPARAM, LPARAM);
DWORD WINAPI STProc(LPVOID);

bool done = false;
HMODULE hModule = GetModuleHandle(NULL);
DWORD threadId = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	HHOOK hHook = NULL;
	HANDLE hStopThread = NULL;
	DWORD status;
	MSG msg;

	threadId = GetCurrentThreadId();

	srand(static_cast<unsigned int> (time(0)));

	MessageBox(NULL, TEXT("Press Ctrl + Q to stop."),
		TEXT("All right, listen up."), MB_OK);

	// Create thread to handle quitting the program.
	hStopThread = CreateThread(NULL, 0, STProc,
		NULL, 0, 0);

	if (!hStopThread)
	{
		MessageBox(NULL, TEXT("You suck."), TEXT("Error"), MB_OK | MB_ICONERROR);
		return 0;
	}

	// Create the hook that will play sound at mouse event.
	hHook = SetWindowsHookEx(WH_MOUSE_LL, LLMProc,
		hInstance, 0);

	if (!hHook)
	{
		MessageBox(NULL, TEXT("You suck."), TEXT("Error"), MB_OK | MB_ICONERROR);
		return 0;
	}

	// Message loop (to keep alive).
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hHook);

	// Let thread finish completely.
	do
	{
		if (!GetExitCodeThread(hStopThread, &status))
			return 0; // Oh, well.
	} while (status == STILL_ACTIVE);

	CloseHandle(hStopThread);

	return 0;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool improveSleepAcc(bool activate)
{
    TIMECAPS tc;
    MMRESULT mmr;
 
    // Fill the TIMECAPS structure.
    if (timeGetDevCaps(&tc, sizeof(tc)) != MMSYSERR_NOERROR)
        return false;
 
    if (activate)
        mmr = timeBeginPeriod(tc.wPeriodMin);
    else
        mmr = timeEndPeriod(tc.wPeriodMin);
 
    if (mmr != TIMERR_NOERROR)
        return false;
 
    return true;
}

LRESULT CALLBACK LLMProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	int sound;

	if (nCode == HC_ACTION && wParam == WM_LBUTTONDOWN)
	{
		sound = 101 + rand() % 7;

		// PlaySound asynchronously (we don't want this func to take too long).
		PlaySound(MAKEINTRESOURCE(sound), hModule, SND_RESOURCE | SND_ASYNC);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI STProc(LPVOID)
{
	while (!done)
	{
		// Hotkey handler to check for Ctrl + Q.
		// Set done to false when hit.
		if (GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState('Q') & 0x8000)
		{
			done = true;
			continue;
		}

		improveSleepAcc(true);
		Sleep(20);
		improveSleepAcc(false);
	}

	PostThreadMessage(threadId, WM_QUIT, 0, 0);

	return 0;
}