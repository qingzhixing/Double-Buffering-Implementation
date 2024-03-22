#pragma once
#include <Windows.h>
#include "debug.h"
#include <iostream>

class KeyboardController
{
public:
	static bool CheckKeyDown(int vKey);
};

#define KEYBOARD_W 0x57
#define KEYBOARD_A 0x41
#define KEYBOARD_S 0x53
#define KEYBOARD_D 0x44
