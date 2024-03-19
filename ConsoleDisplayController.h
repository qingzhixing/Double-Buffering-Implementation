#pragma once
#include <iostream>
#include <Windows.h>
#include "debug.h"

class ConsoleDisplayController
{
public:
    struct ConsoleSize {
        SHORT row, column;
        operator COORD() {
            return { row,column };
        }
    };

public:
    void ConsoleInitialize() {
        // 初始化标准显示缓冲区句柄和buffer缓冲区
        stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        bufferOutput = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,    //控制台缓冲安全与访问权限：读写
            FILE_SHARE_READ | FILE_SHARE_WRITE,   //共享模式：读写共享
            NULL,   //安全属性
            CONSOLE_TEXTMODE_BUFFER,    //缓冲区类型:控制台文本模式缓冲
            NULL    //保留
        );

        //隐藏两个缓冲区光标
        CONSOLE_CURSOR_INFO newConsoleCursorInfo = { };
        newConsoleCursorInfo.bVisible = 0;
        newConsoleCursorInfo.dwSize = 1;
        SetConsoleCursorInfo(bufferOutput, &newConsoleCursorInfo);
        SetConsoleCursorInfo(stdOutput, &newConsoleCursorInfo);
    
        UpdateConsoleSize();

        Debug(PrintConsoleSize(bufferOutput));

        // 使用这个当ActiveScreenBuffer,因为输入输出操作的都是stdOutput
        SetConsoleActiveScreenBuffer(bufferOutput);
    }

public:
    void UpdateConsoleSize() {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(bufferOutput, &consoleInfo);
        SMALL_RECT srWindow = consoleInfo.srWindow;
        consoleSize.row = srWindow.Right - srWindow.Left + 1;
        consoleSize.column = srWindow.Bottom - srWindow.Top + 1;

        //TODO:同步控制台属性
        /*PCONSOLE_SCREEN_BUFFER_INFOEX exInfo=new CONSOLE_SCREEN_BUFFER_INFOEX({});
        if (!GetConsoleScreenBufferInfoEx(console.bufferOutput, exInfo)) {
            std::cout << GetLastError() << std::endl;
        }
        printf("exInfo: srWindow.Top: %d\n", exInfo->srWindow.Top);
        if (!SetConsoleScreenBufferInfoEx(console.stdOutput, exInfo)) {
            std::cout << GetLastError() << std::endl;
        }*/
        SetConsoleScreenBufferSize(stdOutput, consoleSize);
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

    void RefreshFrame() {
        DWORD screenCharAmount = consoleSize.column * consoleSize.row;
        WCHAR* buf = new WCHAR[screenCharAmount + 10];
        DWORD bits(0);
        ReadConsoleOutputCharacter(stdOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);
        WriteConsoleOutputCharacter(bufferOutput, buf, screenCharAmount, COORD({ 0,0 }), &bits);
    }

public:
	HANDLE stdOutput;
	HANDLE bufferOutput;
    ConsoleSize consoleSize;
};