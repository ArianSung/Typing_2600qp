// menu.cpp

#include "menu.h"      // ���� ���� ��� ������ �����Ͽ� ����� ������ �����ɴϴ�.
#include "Typing_Game_Rain.h" // �꼺�� ����(playTypingGame) �Լ��� ����ϱ� ���� �����մϴ�.
#include "AiBattleGame.h"
#include "Memory_Game.h" // �޸� ���� Ŭ������ ����ϱ� ���� �����մϴ�.
#include "Boss_Game.h" // ���� ���� Ŭ������ ����ϱ� ���� �����մϴ�.
#include "TypingPractice.h"

// �� �ҽ� ������ �Լ� �������� �ʿ��� ������� �����մϴ�.
#include <conio.h> // _kbhit, _getch

//=========================================================
// UIManager Ŭ���� ��� �Լ��� ����
//=========================================================
void UIManager::drawMainMenu() {
    system("cls");
    // [����] std::cout -> std::wcout, "..." -> L"..." �� ����
    std::wcout << L"========================================\n";
    std::wcout << L"==            TYPING GAME             ==\n";
    std::wcout << L"========================================\n\n";
    std::wcout << L"              1. Ÿ�� ����\n";
    std::wcout << L"              2. ����\n";
    std::wcout << L"              3. ����\n\n";
    std::wcout << L"========================================\n";
    std::wcout << L">> �޴��� �����ϼ���: ";
}

//---------------------------------------------------------
// ���ο� "���� ����޴�" ��� �Լ� �߰�
//---------------------------------------------------------
void UIManager::drawGameMenu() {
    system("cls");
    // [����] std::cout -> std::wcout, "..." -> L"..." �� ����
    std::wcout << L"========================================\n";
    std::wcout << L"==             GAME MENU              ==\n";
    std::wcout << L"========================================\n\n";
    std::wcout << L"            1. �꼺�� ����\n";
    std::wcout << L"            2. �ܾ� ���\n";
    std::wcout << L"            3. �޸� ç����\n";
    std::wcout << L"            4. ������ ��ƶ�\n";
    std::wcout << L"            5. ���θ޴��� ���ư���\n\n";
    std::wcout << L"========================================\n";
    std::wcout << L">> ������ �����ϼ���: ";
}

//=========================================================
// GameManager Ŭ���� ��� �Լ��� ����
//=========================================================
void GameManager::run() {
    while (true) {
        uiManager.drawMainMenu();
        wchar_t choice; // [����] char -> wchar_t �� �����Ͽ� �����ڵ� ���� �Է¹ޱ�
        std::wcin >> choice; // [����] std::cin -> std::wcin ���� ����

        switch (choice) {
        case L'1': // Ÿ�� ����
            startTypingPractice_Menu();
            break;
        case L'2': { // ���� ����޴�
            uiManager.drawGameMenu();
            wchar_t gameChoice; // [����] char -> wchar_t
            std::wcin >> gameChoice; // [����] std::cin -> std::wcin
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
            case L'5': // ���� �޴� ���� -> ���� �޴� ����
                break;
            default:
                std::wcout << L"\n>> �߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n";
                system("pause");
                break;
            }
            break;

        }
        case L'3': // ����
            std::wcout << L"\n������ �����մϴ�.\n";
            return;

        default:
            std::wcout << L"\n>> �߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n";
            system("pause");
            break;
        }
    }
}

//=========================================================
// ���� �Լ��� ����
//=========================================================

/**
 * @brief �¸����� �� ǥ�õ� ASCII ��Ʈ ȭ��
 */
void displayWinScreen() {
    // [����] std::cout -> std::wcout, "..." -> L"..." �� ����
    std::wcout << L"\n\n\n\n";
    std::wcout << L"    $$$   $$$   $$$$$$$   $$$    $$$   $$$$$   \n";
    std::wcout << L"    $$$   $$$     $$$     $$$$   $$$   $$$$$   \n";
    std::wcout << L"    $$$ $ $$$     $$$     $$$ $$ $$$   $$$$$   \n";
    std::wcout << L"    $$$$$$$$$     $$$     $$$  $$$$$           \n";
    std::wcout << L"     $$$ $$$    $$$$$$$   $$$   $$$$   $$$$$   \n";
}

/**
 * @brief �й� �Ǵ� ���º� �� ǥ�õ� ASCII ��Ʈ ȭ��
 */
void displayGameOverScreen() {
    // [����] std::cout -> std::wcout, "..." -> L"..." �� ����
    std::wcout << L"\n\n\n";
    std::wcout << L"     $$$$$$$    $$$$$   $$$    $$$ $$$$$$$$       $$$$$$  $$$    $$$ $$$$$$$$ $$$$$$$ \n";
    std::wcout << L"    $$$       $$$   $$$ $$$$$$$$$$ $$$           $$$  $$$ $$$    $$$ $$$      $$$   $$$\n";
    std::wcout << L"    $$$  $$$$ $$$$$$$$$ $$$ $$ $$$ $$$$$$$       $$$  $$$ $$$    $$$ $$$$$$$  $$$$$$$ \n";
    std::wcout << L"    $$$   $$$ $$$   $$$ $$$    $$$ $$$           $$$  $$$  $$$  $$$  $$$      $$$   $$$\n";
    std::wcout << L"     $$$$$$$  $$$   $$$ $$$    $$$ $$$$$$$$       $$$$$$    $$$$$$   $$$$$$$$ $$$   $$$\n";
}

// Ÿ�� ���� ���� �Լ� (����� �ڸ��� ����)
void startTypingPractice_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*           << Ÿ�� ����  >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500);
    startTypingPractice();
    system("pause");
}

// �꼺�� ���� ���� �Լ�
void startRainGame_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << �꼺�� ���� >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    startRainGame();

    system("pause");
}
void startAiBattleGame_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*            << �ܾ� ��� >>           *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    startAiBattleGame();

    system("pause");
}

void startMemoryChallenge_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << �޸� ç���� >>         *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    startMemoryChallenge();

    system("pause");
}

void startTypingBossBattle_Menu() {
    system("cls");
    std::wcout << L"****************************************\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"*          << ������ ��ƶ� >>         *\n";
    std::wcout << L"*                                      *\n";
    std::wcout << L"****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    startTypingBossBattle();

    system("pause");
}
