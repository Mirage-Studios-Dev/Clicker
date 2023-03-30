#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <unordered_map>
static void keybind(int& bind) {
    int g = 0;
    while (g != 1) {
        for (auto i = 1; i < 256; i++)
            if (GetAsyncKeyState(i) & 0x8000)
            {
                if (i == 27 | i == 1 | i == 2 | i == 4) {
                    bind = 0;
                    g = 1;
                    break;
                }
                else {
                    bind = i;
                    g = 1;
                    break;
                }
                std::cout << "CLKD" << g << "\n";
            }
    }
}

static std::string get_key_name_by_id(int id)
{
    static std::unordered_map<int, std::string> key_names = {
        { 0, "Click to Bind" },
        { VK_LBUTTON, "M1" },
        { VK_RBUTTON, "M2" },
        { VK_MBUTTON, "M3" },
        { VK_XBUTTON1, "M4" },
        { VK_XBUTTON2, "M5" },
        { VK_BACK, "BACK" },
        { VK_TAB, "TAB" },
        { VK_CLEAR, "CLR" },
        { VK_RETURN, "ENTER" },
        { VK_SHIFT, "SHFT" },
        { VK_CONTROL, "CTRL" },
        { VK_MENU, "ALT" },
        { VK_PAUSE, "PAUSE" },
        { VK_CAPITAL, "CAPS" },
        { VK_ESCAPE, "ESC" },
        { VK_SPACE, "SPACE" },
        { VK_PRIOR, "PUP" },
        { VK_NEXT, "PDWN" },
        { VK_END, "END" },
        { VK_HOME, "HOME" },
        { VK_LEFT, "LEFT" },
        { VK_UP, "UP" },
        { VK_RIGHT, "RIGHT" },
        { VK_DOWN, "DOWN" },
        { VK_SELECT, "SLC" },
        { VK_PRINT, "PSCRN" },
        { VK_INSERT, "INS" },
        { VK_DELETE, "DLT" },
        { VK_HELP, "HLP" },
        { VK_SLEEP, "SLP" },
        { VK_MULTIPLY, "*" },
        { VK_ADD, "+" },
        { VK_SUBTRACT, "-" },
        { VK_DECIMAL, "." },
        { VK_DIVIDE, "/" },
        { VK_NUMLOCK, "NL" },
        { VK_SCROLL, "SCRL" },
        { VK_LSHIFT, "LShft" },
        { VK_RSHIFT, "RShft" },
        { VK_LCONTROL, "LCtrl" },
        { VK_RCONTROL, "RCtrl" },
        { VK_LMENU, "LAlt" },
        { VK_RMENU, "RAlt" },
    };

    if (id >= 0x30 && id <= 0x5A)
        return std::string(1, (char)id);

    if (id >= 0x60 && id <= 0x69)
        return "N " + std::to_string(id - 0x60);

    if (id >= 0x70 && id <= 0x87)
        return "F" + std::to_string((id - 0x70) + 1);

    return key_names[id];
}