#pragma once
#include "ConsoleDisplayController.h"

extern ConsoleDisplayController console;

class GameController
{
public:
	GameController() :playerPosition({ 0,0 }) {
	}
public:
	void Draw();

public:
	COORD playerPosition;
};
