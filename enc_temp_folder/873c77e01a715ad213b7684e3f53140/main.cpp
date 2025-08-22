#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <cstdio>
#include "menu.h"

//=========================================================
// main 함수: 프로그램 시작점
//=========================================================
int main() {
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    SetConsoleTitle(L"Typing Battle Game");
    GameManager game;
    game.run();
    return 0;
}
