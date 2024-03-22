#include "GameController.h"

void GameController::Draw()
{
	console.ClearScreen();
	SetConsoleCursorPosition(console.stdOutput, RotateCoord(playerPosition));
	putchar('@');
}

void GameController::Update()
{
	// ¥¶¿Ì ‰»Î
	InputHandler();
}

void GameController::InputHandler()
{
	int newX = playerPosition.X;
	int newY = playerPosition.Y;
	//W
	if (KeyboardController::CheckKeyDown(KEYBOARD_W)) {
		newX -= 1;
	}

	//A
	if (KeyboardController::CheckKeyDown(KEYBOARD_A)) {
		newY -= 1;
	}

	//S
	if (KeyboardController::CheckKeyDown(KEYBOARD_S)) {
		newX += 1;
	}

	//D
	if (KeyboardController::CheckKeyDown(KEYBOARD_D)) {
		newY += 1;
	}

	newX = max(0, newX);
	newX = min(newX, (console.consoleSize.column - 1));
	newY = max(0, newY);
	newY = min(newY, (console.consoleSize.row - 1));

	playerPosition.X = newX;
	playerPosition.Y = newY;
}

COORD RotateCoord(const COORD& instance) {
	return { instance.Y, instance.X };
}