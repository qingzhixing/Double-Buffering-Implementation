#include "KeyboardController.h"
bool KeyboardController::CheckKeyDown(int vKey)
{
	//Debug(printf("Checking %d", vKey));
	return GetAsyncKeyState(vKey) & 0x8000;
	//GetAsyncKeyState()�ķ���ֵ��ʾ�������ݣ�һ�������λbit��ֵ������������Ƿ񱻰��£�
	// һ�������λbit��ֵ���������ϴε���GetAsyncKeyState()��������Ƿ񱻰��¡�
}