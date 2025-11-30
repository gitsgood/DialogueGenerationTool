#pragma once

#ifndef CORE_H
#define CORE_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string> 
#include <sstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <algorithm> // For std::max

#include "raylib.h"
#include "raymath.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
char getch() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}
#endif

#endif // !CORE.H