#include "KeyboardController.h"
bool KeyboardController::CheckKeyDown(int vKey)
{
	//Debug(printf("Checking %d", vKey));
	return GetAsyncKeyState(vKey) & 0x8000;
	//GetAsyncKeyState()的返回值表示两个内容，一个是最高位bit的值，代表这个键是否被按下，
	// 一个是最低位bit的值，代表在上次调用GetAsyncKeyState()后，这个键是否被按下。
}