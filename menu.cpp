// menu.cpp

#include "menu.h"      // 직접 만든 헤더 파일을 포함하여 선언된 내용을 가져옵니다.
#include "Typing_Game_Rain.h" // 산성비 게임(playTypingGame) 함수를 사용하기 위해 포함합니다.
#include "AiBattleGame.h"
#include "Memory_Game.h" // 메모리 게임 클래스를 사용하기 위해 포함합니다.
#include "Boss_Game.h" // 보스 게임 클래스를 사용하기 위해 포함합니다.
#include "TypingPractice.h"

// 이 소스 파일의 함수 구현에만 필요한 헤더들을 포함합니다.
#include <conio.h> // _kbhit, _getch

//=========================================================
// UIManager 클래스 멤버 함수의 정의
//=========================================================
void UIManager::drawMainMenu() {
    system("cls");
    // [수정] std::cout -> std::wcout, "..." -> L"..." 로 변경
    std::wcout << L"========================================\n";
    std::wcout << L"==            TYPING GAME             ==\n";
    std::wcout << L"========================================\n\n";
    std::wcout << L"              1. 타자 연습\n";
    std::wcout << L"              2. 게임\n";
    std::wcout << L"              3. 종료\n\n";
    std::wcout << L"========================================\n";
    std::wcout << L">> 메뉴를 선택하세요: ";
}

//---------------------------------------------------------
// 새로운 "게임 서브메뉴" 출력 함수 추가
//---------------------------------------------------------
void UIManager::drawGameMenu() {
    system("cls");
    // [수정] std::cout -> std::wcout, "..." -> L"..." 로 변경
    std::wcout << L"========================================\n";
    std::wcout << L"==             GAME MENU              ==\n";
    std::wcout << L"========================================\n\n";
    std::wcout << L"            1. 산성비 게임\n";
    std::wcout << L"            2. 단어 대결\n";
    std::wcout << L"            3. 메모리 챌린지\n";
    std::wcout << L"            4. 보스를 잡아라\n";
    std::wcout << L"            5. 메인메뉴로 돌아가기\n\n";
    std::wcout << L"========================================\n";
    std::wcout << L">> 게임을 선택하세요: ";
}

//=========================================================
// GameManager 클래스 멤버 함수의 정의
//=========================================================
void GameManager::run() {
    while (true) {
        uiManager.drawMainMenu();
        wchar_t choice; // [수정] char -> wchar_t 로 변경하여 유니코드 문자 입력받기
        std::wcin >> choice; // [수정] std::cin -> std::wcin 으로 변경

        switch (choice) {
        case L'1': // 타자 연습
            startTypingPractice_Menu();
            break;
        case L'2': { // 게임 서브메뉴
            uiManager.drawGameMenu();
            wchar_t gameChoice; // [수정] char -> wchar_t
            std::wcin >> gameChoice; // [수정] std::cin -> std::wcin
            switch (gameChoice) {
            case L'1':
                startRainGame_Menu();
                break;
            case L'2':
                startAiBattleGame_Menu();
                break;
            case L'3':
                startMemoryChallenge_Menu();
                break;
            case L'4':
                startTypingBossBattle_Menu();
                break;
            case L'5': // 서브 메뉴 종료 -> 메인 메뉴 복귀
                break;
            default:
                std::wcout << L"\n>> 잘못된 입력입니다. 다시 선택해주세요.\n";
                system("pause");
                break;
            }
            break;

        }
        case L'3': // 종료
            std::wcout << L"\n게임을 종료합니다.\n";
            return;

        default:
            std::wcout << L"\n>> 잘못된 입력입니다. 다시 선택해주세요.\n";
            system("pause");
            break;
        }
    }
}

//=========================================================
// 전역 함수의 정의
//=========================================================

/**
 * @brief 승리했을 때 표시될 ASCII 아트 화면
 */
void displayWinScreen() {
    // [수정] std::cout -> std::wcout, "..." -> L"..." 로 변경
    std::wcout << L"\n\n\n\n";
    std::wcout << L"    $$$   $$$   $$$$$$$   $$$    $$$   $$$$$   \n";
    std::wcout << L"    $$$   $$$     $$$     $$$$   $$$   $$$$$   \n";
    std::wcout << L"    $$$ $ $$$     $$$     $$$ $$ $$$   $$$$$   \n";
    std::wcout << L"    $$$$$$$$$     $$$     $$$  $$$$$           \n";
    std::wcout << L"     $$$ $$$    $$$$$$$   $$$   $$$$   $$$$$   \n";
}

/**
 * @brief 패배 또는 무승부 시 표시될 ASCII 아트 화면
 */
void displayGameOverScreen() {
    // [수정] std::cout -> std::wcout, "..." -> L"..." 로 변경
    std::wcout << L"\n\n\n";
    std::wcout << L"     $$$$$$$    $$$$$   $$$    $$$ $$$$$$$$       $$$$$$  $$$    $$$ $$$$$$$$ $$$$$$$ \n";
    std::wcout << L"    $$$       $$$   $$$ $$$$$$$$$$ $$$           $$$  $$$ $$$    $$$ $$$      $$$   $$$\n";
    std::wcout << L"    $$$  $$$$ $$$$$$$$$ $$$ $$ $$$ $$$$$$$       $$$  $$$ $$$    $$$ $$$$$$$  $$$$$$$ \n";
    std::wcout << L"    $$$   $$$ $$$   $$$ $$$    $$$ $$$           $$$  $$$  $$$  $$$  $$$      $$$   $$$\n";
    std::wcout << L"     $$$$$$$  $$$   $$$ $$$    $$$ $$$$$$$$       $$$$$$    $$$$$$   $$$$$$$$ $$$   $$$\n";
}

// 타자 연습 게임 함수 (현재는 자리만 마련)
void startTypingPractice_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*           << 타자 연습  >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500);
    startTypingPractice();
    system("pause");
}

// 산성비 게임 시작 함수
void startRainGame_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << 산성비 게임 >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5초 후 게임 시작

    startRainGame();

    system("pause");
}
void startAiBattleGame_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*            << 단어 대결 >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5초 후 게임 시작

    startAiBattleGame();

    system("pause");
}

void startMemoryChallenge_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << 메모리 챌린지 >>         *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5초 후 게임 시작

    startMemoryChallenge();

    system("pause");
}

void startTypingBossBattle_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << 보스를 잡아라 >>         *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5초 후 게임 시작

    startTypingBossBattle();

    system("pause");
}
