#pragma once
// GameManager.h

#pragma once // ��� ������ �ߺ� ������ �����մϴ�.

// Ŭ���� �� �Լ� ���� �ʿ��� �ּ����� ����� �����մϴ�.
#include <iostream>
#include <windows.h>

//=========================================================
// Ŭ���� ����
//=========================================================

// Utility Ŭ����: �ܼ� ȭ�� ����� ���� ���� ����� ����մϴ�.
class Utility {
public:
    // static �Լ��� �ν��Ͻ� ���� 'Utility::gotoxy(x, y)' ���·� �ٷ� ȣ���� �� �ֽ��ϴ�.
    // ������ ����̹Ƿ� ��� ���Ͽ� �ٷ� �����մϴ�.
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

// UIManager Ŭ����: �޴� ȭ�� �� UI ����� ����մϴ�.
class UIManager {
public:
    void drawMainMenu(); // �Լ��� ����
};

// GameManager Ŭ����: ������ ��ü���� �帧�� �����մϴ�.
class GameManager {
private:
    UIManager uiManager; // UI ���� ��ü�� ����� �����ϴ�.

public:
    void run(); // ���� ���� ���� �Լ��� ����
};


//=========================================================
// ���� �Լ� ����
//=========================================================

void startTypingPractice();
void startRainGame_Menu();
void displayWinScreen();
void displayGameOverScreen();
void startAiBattleGame_Menu();
void startMemoryChallenge_Menu();
void startTypingBossBattle_Menu();
