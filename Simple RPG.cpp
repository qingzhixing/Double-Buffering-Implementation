#include <iostream>
#include <Windows.h>

struct Hud {
    HANDLE stdOutput;
    HANDLE bufferOutput;
};

Hud outputHandle;

struct ConsoleSize {
    DWORD row, column;
};

ConsoleSize consoleSize;

void UpdateConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO stdConsoleInfo;
    GetConsoleScreenBufferInfo(outputHandle.stdOutput, &stdConsoleInfo);
    SMALL_RECT srWindow = stdConsoleInfo.srWindow;
    consoleSize.row = srWindow.Right - srWindow.Left + 1;
    consoleSize.column = srWindow.Bottom - srWindow.Top + 1;
}

void ClearScreen(HANDLE hConsole)
{
    COORD coordScreen = { 0, 0 };    // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.
    if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
        (TCHAR)' ',      // Character to write to the buffer
        dwConSize,       // Number of cells to write
        coordScreen,     // Coordinates of first cell
        &cCharsWritten)) // Receive number of characters written
    {
        return;
    }

    // Get the current text attribute.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Set the buffer's attributes accordingly.
    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
        csbi.wAttributes, // Character attributes to use
        dwConSize,        // Number of cells to set attribute
        coordScreen,      // Coordinates of first cell
        &cCharsWritten))  // Receive number of characters written
    {
        return;
    }

    // Put the cursor at its home coordinates.
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void TestHalt() {
    while (1) {

    }
}

void RefreshFrame() {
    DWORD screenCharAmount = consoleSize.column * consoleSize.row;
    WCHAR* buf = new WCHAR[screenCharAmount + 10];
    DWORD bits(0);
    ReadConsoleOutputCharacter(outputHandle.stdOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);
    WriteConsoleOutputCharacter(outputHandle.bufferOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);
}

// 初始化
void Initial() {
    // 初始化标准显示缓冲区句柄和buffer缓冲区
    outputHandle.stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    outputHandle.bufferOutput = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,    //控制台缓冲安全与访问权限：读写
        FILE_SHARE_READ|FILE_SHARE_WRITE,   //共享模式：读写共享
        NULL,   //安全属性
        CONSOLE_TEXTMODE_BUFFER,    //缓冲区类型:控制台文本模式缓冲
        NULL    //保留
    );

    //隐藏两个缓冲区光标
    CONSOLE_CURSOR_INFO newConsoleCursorInfo;
    newConsoleCursorInfo.bVisible = 0;
    newConsoleCursorInfo.dwSize = 1;
    SetConsoleCursorInfo(outputHandle.bufferOutput, &newConsoleCursorInfo);
    SetConsoleCursorInfo(outputHandle.stdOutput, &newConsoleCursorInfo);

    UpdateConsoleSize();
    printf("WindowSize: row: %d, column: %d \n", consoleSize.row,consoleSize.column);

    // 使用这个当ActiveScreenBuffer,因为输入输出操作的都是stdOutput
    SetConsoleActiveScreenBuffer(outputHandle.bufferOutput);
    system("pause");
}

//游戏循环
void Loop() {
    ClearScreen(outputHandle.stdOutput);
    UpdateConsoleSize();
    printf("WindowSize: row: %d, column: %d \n", consoleSize.row, consoleSize.column);
    RefreshFrame();
    system("pause");
}


int main()
{
    std::cout << "Hello World!\n";
    Initial();
    while (true) {
        Loop();
    }
}