#include "ConsoleDisplayController.h"

void ConsoleDisplayController::ConsoleInitialize() {
	// 初始化标准显示缓冲区句柄和buffer缓冲区
	stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	bufferOutput = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,    //控制台缓冲安全与访问权限：读写
		FILE_SHARE_READ | FILE_SHARE_WRITE,   //共享模式：读写共享
		NULL,   //安全属性
		CONSOLE_TEXTMODE_BUFFER,    //缓冲区类型:控制台文本模式缓冲
		NULL    //保留
	);

	mainWindow = GetForegroundWindow();

	//设置全屏
	//LONG l_WinStyle = GetWindowLong(mainWindow, GWL_STYLE);   /* 获取窗口信息 */
	//SetWindowLong(mainWindow, GWL_STYLE, (l_WinStyle | WS_MAXIMIZE)/* & ~WS_THICKFRAME*/);

	// TODO: 缓冲区同步改变
	//int cx = GetSystemMetrics(SM_CXSCREEN);            /* 屏幕宽度 像素 */
	//int cy = GetSystemMetrics(SM_CYSCREEN);            /* 屏幕高度 像素 */
	//SetWindowPos(mainWindow, HWND_TOP, 50, 50, cx, cy, 0);

	//更新Buffer
	UpdateScreenBufferSize();

	Debug(PrintConsoleSize(bufferOutput));

	// 使用这个当ActiveScreenBuffer,因为输入输出操作的都是stdOutput
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

	//同步ConsoleScreenBuffer大小
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

	// 防止垂直高度过小导致闪退(超过类型范围)
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

	//设置光标隐藏，防止光标残留
	HideConsoleCursor(stdOutput);
	HideConsoleCursor(bufferOutput);
}