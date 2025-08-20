#include <iostream>
#include <string>
#include "menu.h"

//=========================================================
// main 함수: 프로그램 시작점
//=========================================================
int main() {
    SetConsoleTitle(L"Typing Battle Game");
    GameManager game;
    game.run();
    return 0;
}
