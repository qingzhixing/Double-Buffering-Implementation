#pragma once
#include <iostream>
#include <Windows.h>
#include "debug.h"
#include <stdexcept>

class ConsoleDisplayController
{
public:
	struct ConsoleSize {
		SHORT row, column;
		operator COORD() {
			return { row,column };
		}
	};

public:
	void ConsoleInitialize();

public:
	void HideConsoleCursor(HANDLE bufferHandle);

	void UpdateScreenBufferSize();

	void SetConsoleSize(HANDLE bufferHandle, ConsoleSize consoleSize);

	const ConsoleSize& GetConsoleSize(HANDLE consoleHandle);

	void PrintConsoleSize(HANDLE consoleHandle);

	void ClearScreen();

	void RefreshFrame();

public:
	HANDLE stdOutput;
	HANDLE bufferOutput;
	ConsoleSize consoleSize;
	void (*onConsoleSizeChanged)() = []()->void {};
	HWND mainWindow;
};