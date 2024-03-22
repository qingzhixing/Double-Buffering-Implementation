#include "ConsoleDisplayController.h"

void ConsoleDisplayController::ConsoleInitialize() {
	// ��ʼ����׼��ʾ�����������buffer������
	stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	bufferOutput = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,    //����̨���尲ȫ�����Ȩ�ޣ���д
		FILE_SHARE_READ | FILE_SHARE_WRITE,   //����ģʽ����д����
		NULL,   //��ȫ����
		CONSOLE_TEXTMODE_BUFFER,    //����������:����̨�ı�ģʽ����
		NULL    //����
	);

	mainWindow = GetForegroundWindow();

	//����ȫ��
	//LONG l_WinStyle = GetWindowLong(mainWindow, GWL_STYLE);   /* ��ȡ������Ϣ */
	//SetWindowLong(mainWindow, GWL_STYLE, (l_WinStyle | WS_MAXIMIZE)/* & ~WS_THICKFRAME*/);

	// TODO: ������ͬ���ı�
	//int cx = GetSystemMetrics(SM_CXSCREEN);            /* ��Ļ��� ���� */
	//int cy = GetSystemMetrics(SM_CYSCREEN);            /* ��Ļ�߶� ���� */
	//SetWindowPos(mainWindow, HWND_TOP, 50, 50, cx, cy, 0);

	//����Buffer
	UpdateScreenBufferSize();

	Debug(PrintConsoleSize(bufferOutput));

	// ʹ�������ActiveScreenBuffer,��Ϊ������������Ķ���stdOutput
	SetConsoleActiveScreenBuffer(bufferOutput);
}

void ConsoleDisplayController::HideConsoleCursor(HANDLE bufferHandle) {
	CONSOLE_CURSOR_INFO newConsoleCursorInfo = { };
	newConsoleCursorInfo.bVisible = 0;
	newConsoleCursorInfo.dwSize = 1;
	SetConsoleCursorInfo(bufferHandle, &newConsoleCursorInfo);
}

void ConsoleDisplayController::UpdateScreenBufferSize() {
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(bufferOutput, &consoleInfo);
	SMALL_RECT srWindow = consoleInfo.srWindow;
	SHORT newRow, newColumn;
	newRow = srWindow.Right - srWindow.Left + 1;
	newColumn = srWindow.Bottom - srWindow.Top + 1;

	if (newRow != consoleSize.row || newColumn != consoleSize.column) {
		onConsoleSizeChanged();
	}

	consoleSize.row = newRow;
	consoleSize.column = newColumn;

	//ͬ��ConsoleScreenBuffer��С
	SetConsoleSize(stdOutput, consoleSize);
	SetConsoleSize(bufferOutput, consoleSize);
}

void ConsoleDisplayController::SetConsoleSize(HANDLE bufferHandle, ConsoleSize consoleSize)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT rect;
	COORD      coord;
	if (!GetConsoleScreenBufferInfo(bufferHandle, &csbi)) {
		printf("Cannot GetConsoleScreenBufferInfo\n");
		Halt();
	}

	rect.Left = 0;
	rect.Top = 0;

	// ��ֹ��ֱ�߶ȹ�С��������(�������ͷ�Χ)
	if ((consoleSize.column - 1) < 3) {
		Debug(printf("False: !(consoleSize.column - 1) < 3)\n"));
		return;
	}

	rect.Right = (consoleSize.row - 1);
	rect.Bottom = (consoleSize.column - 1);

	SetConsoleWindowInfo(bufferHandle, TRUE, &rect);

	coord.X = consoleSize.row;
	coord.Y = consoleSize.column;
	SetConsoleScreenBufferSize(bufferHandle, coord);
}

const ConsoleDisplayController::ConsoleSize& ConsoleDisplayController::GetConsoleSize(HANDLE consoleHandle) {
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	ConsoleSize consoleSize = {};
	GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
	SMALL_RECT srWindow = consoleInfo.srWindow;
	consoleSize.row = srWindow.Right - srWindow.Left + 1;
	consoleSize.column = srWindow.Bottom - srWindow.Top + 1;
	return std::move(consoleSize);
}

void ConsoleDisplayController::PrintConsoleSize(HANDLE consoleHandle) {
	const ConsoleSize& size = GetConsoleSize(consoleHandle);
	printf("Console Size: row: %d, column: %d \n", size.row, size.column);
}

void ConsoleDisplayController::ClearScreen()
{
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(stdOutput, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(stdOutput,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(stdOutput, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(stdOutput,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(stdOutput, coordScreen);
	SetConsoleCursorPosition(bufferOutput, coordScreen);
}

void ConsoleDisplayController::RefreshFrame() {
	DWORD screenCharAmount = consoleSize.column * consoleSize.row;
	WCHAR* buf = new WCHAR[screenCharAmount + 10];
	DWORD bits(0);
	ReadConsoleOutputCharacter(stdOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);
	WriteConsoleOutputCharacter(bufferOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);

	//���ù�����أ���ֹ������
	HideConsoleCursor(stdOutput);
	HideConsoleCursor(bufferOutput);
}