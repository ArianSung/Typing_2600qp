#pragma once
// GameManager.h

#pragma once // 헤더 파일의 중복 포함을 방지합니다.

// 클래스 및 함수 선언에 필요한 최소한의 헤더만 포함합니다.
#include <iostream>
#include <windows.h>

//=========================================================
// 클래스 선언
//=========================================================

// Utility 클래스: 콘솔 화면 제어와 같은 보조 기능을 담당합니다.
class Utility {
public:
    // static 함수는 인스턴스 없이 'Utility::gotoxy(x, y)' 형태로 바로 호출할 수 있습니다.
    // 간단한 기능이므로 헤더 파일에 바로 정의합니다.
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

// UIManager 클래스: 메뉴 화면 등 UI 출력을 담당합니다.
class UIManager {
public:
    void drawMainMenu(); // 함수의 선언
};

// GameManager 클래스: 게임의 전체적인 흐름을 관리합니다.
class GameManager {
private:
    UIManager uiManager; // UI 관리 객체를 멤버로 가집니다.

public:
    void run(); // 게임 실행 루프 함수의 선언
};


//=========================================================
// 전역 함수 선언
//=========================================================

void startTypingPractice();
void startRainGame_Menu();
void displayWinScreen();
void displayGameOverScreen();
void startAiBattleGame_Menu();
void startMemoryChallenge_Menu();
void startTypingBossBattle_Menu();
