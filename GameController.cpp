#include "GameController.h"

void GameController::Draw()
{
	console.ClearScreen();
	SetConsoleCursorPosition(console.stdOutput, playerPosition);
	putchar('@');
}