#pragma once
#include "ConsoleDisplayController.h"
#include "KeyboardController.h"
#include "debug.h"
#include <iostream>

extern ConsoleDisplayController console;

class GameController
{
public:
	GameController() :playerPosition({ 0,0 }) {
	}
public:
	void Draw();

	// ÿһ֡����
	void Update();

	void InputHandler();
public:
	COORD playerPosition;
};

COORD RotateCoord(const COORD& instance);
