#include <iostream>
#include <string>
#include "menu.h"

//=========================================================
// main �Լ�: ���α׷� ������
//=========================================================
int main() {
    SetConsoleTitle(L"Typing Battle Game");
    GameManager game;
    game.run();
    return 0;
}
