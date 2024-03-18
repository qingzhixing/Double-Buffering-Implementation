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
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(outputHandle.bufferOutput, &consoleInfo);
    SMALL_RECT srWindow = consoleInfo.srWindow;
    consoleSize.row = srWindow.Right - srWindow.Left + 1;
    consoleSize.column = srWindow.Bottom - srWindow.Top + 1;

    //TODO:同步控制台属性
    PCONSOLE_SCREEN_BUFFER_INFOEX exInfo=new CONSOLE_SCREEN_BUFFER_INFOEX({});
    if (!GetConsoleScreenBufferInfoEx(outputHandle.bufferOutput, exInfo)) {
        std::cout << GetLastError() << std::endl;
    }
    printf("exInfo: srWindow.Top: %d\n", exInfo->srWindow.Top);
    if (!SetConsoleScreenBufferInfoEx(outputHandle.stdOutput, exInfo)) {
        std::cout << GetLastError() << std::endl;
    }
}

const ConsoleSize& GetConsoleSize(HANDLE consoleHandle) {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    ConsoleSize consoleSize = {};
    GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
    SMALL_RECT srWindow = consoleInfo.srWindow;
    consoleSize.row = srWindow.Right - srWindow.Left + 1;
    consoleSize.column = srWindow.Bottom - srWindow.Top + 1;
    return std::move(consoleSize);
}

void PrintConsoleSize(HANDLE consoleHandle) {
    const ConsoleSize& size = GetConsoleSize(consoleHandle);
    printf("Console Size: row: %d, column: %d \n", size.row, size.column);
}

void ClearScreen()
{
    HANDLE hConsole = outputHandle.stdOutput;
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
    SetConsoleCursorPosition(outputHandle.bufferOutput, coordScreen);
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
    CONSOLE_CURSOR_INFO newConsoleCursorInfo = { };
    newConsoleCursorInfo.bVisible = 0;
    newConsoleCursorInfo.dwSize = 1;
    SetConsoleCursorInfo(outputHandle.bufferOutput, &newConsoleCursorInfo);
    SetConsoleCursorInfo(outputHandle.stdOutput, &newConsoleCursorInfo);

    UpdateConsoleSize();
    printf("WindowSize: row: %d, column: %d \n", consoleSize.row,consoleSize.column);

    // 使用这个当ActiveScreenBuffer,因为输入输出操作的都是stdOutput
    SetConsoleActiveScreenBuffer(outputHandle.bufferOutput);
}

//游戏循环
void Loop() {
    DWORD startTime = GetTickCount();

    ClearScreen();
    UpdateConsoleSize();

    PrintConsoleSize(outputHandle.bufferOutput);
    PrintConsoleSize(outputHandle.stdOutput);
    
    RefreshFrame();

    DWORD endTime = GetTickCount();
    DWORD deltaTime = endTime - startTime;

    // 显示fps
    wchar_t fpsDisplayStr[1024] = {};
    wsprintf(fpsDisplayStr,L"FPS: %.2f",1);
    SetConsoleTitle(fpsDisplayStr);

    // 锁定刷新率60fps
    if (deltaTime < 1000 / 60) {
        Sleep(1000 / 60 - deltaTime);
    }
}


int main()
{
    std::cout << "Hello World!\n";
    Initial();
    while (true) {
        Loop();
    }
}