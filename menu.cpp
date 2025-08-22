// GameManager.cpp

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
    std::cout << "========================================\n";
    std::cout << "==            TYPING GAME             ==\n";
    std::cout << "========================================\n\n";
    std::cout << "              1. Ÿ�� ����\n";
    std::cout << "              2. ����\n";
    std::cout << "              3. ����\n\n";
    std::cout << "========================================\n";
    std::cout << ">> �޴��� �����ϼ���: ";
}

//---------------------------------------------------------
// ���ο� "���� ����޴�" ��� �Լ� �߰�
//---------------------------------------------------------
void UIManager::drawGameMenu() {
    system("cls");
    std::cout << "========================================\n";
    std::cout << "==             GAME MENU              ==\n";
    std::cout << "========================================\n\n";
    std::cout << "            1. �꼺�� ����\n";
    std::cout << "            2. �ܾ� ���\n";
    std::cout << "            3. �޸� ç����\n";
    std::cout << "            4. ������ ��ƶ�\n";
    std::cout << "            5. ���θ޴��� ���ư���\n\n";
    std::cout << "========================================\n";
    std::cout << ">> ������ �����ϼ���: ";
}

//=========================================================
// GameManager Ŭ���� ��� �Լ��� ����
//=========================================================
void GameManager::run() {
    while (true) {
        uiManager.drawMainMenu();
        char choice;
        std::cin >> choice;

        switch (choice) {
        case '1': // Ÿ�� ����
            startTypingPractice_Menu();
            break;
        case '2': { // ���� ����޴�
            uiManager.drawGameMenu();
            char gameChoice;
            std::cin >> gameChoice;
            switch (gameChoice) {
            case '1':
                startRainGame_Menu();
                break;
            case '2':
                startAiBattleGame_Menu();
                break;
            case '3':
                startMemoryChallenge_Menu();
                break;
            case '4':
                startTypingBossBattle_Menu();
                break;
            case '5': // ���� �޴� ���� -> ���� �޴� ����
                break;
                //goto BACK_TO_MAIN;
            default:
                std::cout << "\n>> �߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n";
                system("pause");
                break;
            }
            break;
            
        }
        case '3': // ����
            std::cout << "\n������ �����մϴ�.\n";
            return;

        default:
            std::cout << "\n>> �߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n";
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
    std::cout << "\n\n\n\n";
    std::cout << "    $$$   $$$   $$$$$$$   $$$    $$$   $$$$$   \n";
    std::cout << "    $$$   $$$     $$$     $$$$   $$$   $$$$$   \n";
    std::cout << "    $$$ $ $$$     $$$     $$$ $$ $$$   $$$$$   \n";
    std::cout << "    $$$$$$$$$     $$$     $$$  $$$$$           \n";
    std::cout << "     $$$ $$$    $$$$$$$   $$$   $$$$   $$$$$   \n";
}

/**
 * @brief �й� �Ǵ� ���º� �� ǥ�õ� ASCII ��Ʈ ȭ��
 */
void displayGameOverScreen() {
    std::cout << "\n\n\n";
    std::cout << "     $$$$$$$    $$$$$   $$$    $$$ $$$$$$$$       $$$$$$  $$$    $$$ $$$$$$$$ $$$$$$$ \n";
    std::cout << "    $$$       $$$   $$$ $$$$$$$$$$ $$$           $$$  $$$ $$$    $$$ $$$      $$$   $$$\n";
    std::cout << "    $$$  $$$$ $$$$$$$$$ $$$ $$ $$$ $$$$$$$       $$$  $$$ $$$    $$$ $$$$$$$  $$$$$$$ \n";
    std::cout << "    $$$   $$$ $$$   $$$ $$$    $$$ $$$           $$$  $$$  $$$  $$$  $$$      $$$   $$$\n";
    std::cout << "     $$$$$$$  $$$   $$$ $$$    $$$ $$$$$$$$       $$$$$$    $$$$$$   $$$$$$$$ $$$   $$$\n";
}

// Ÿ�� ���� ���� �Լ� (����� �ڸ��� ����)
void startTypingPractice_Menu() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*                                      *\n";
    std::cout << "*        << Ÿ�� ����  >>              *\n";
    std::cout << "*                                      *\n";
    std::cout << "****************************************\n\n";
    Sleep(1500);
    startTypingPractice();
    system("pause");
}

// �꼺�� ���� ���� �Լ�
void startRainGame_Menu() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*                                      *\n";
    std::cout << "*         << �꼺�� ���� >>            *\n";
    std::cout << "*                                      *\n";
    std::cout << "****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    // ������ ���� �꼺�� ���� �Լ��� ȣ���մϴ�.
    startRainGame();

    // ������ ������ ��� ��� �� �޴��� ���ư��ϴ�.
    system("pause");
}
void startAiBattleGame_Menu() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*                                      *\n";
    std::cout << "*         << �ܾ� ��� >>              *\n";
    std::cout << "*                                      *\n";
    std::cout << "****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    // ������ ���� �꼺�� ���� �Լ��� ȣ���մϴ�.
    startAiBattleGame();

    // ������ ������ ��� ��� �� �޴��� ���ư��ϴ�.
    system("pause");
}

void startMemoryChallenge_Menu() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*                                      *\n";
    std::cout << "*         << �޸� ç���� >>          *\n";
    std::cout << "*                                      *\n";
    std::cout << "****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    // ������ ���� �꼺�� ���� �Լ��� ȣ���մϴ�.
    startMemoryChallenge();

    // ������ ������ ��� ��� �� �޴��� ���ư��ϴ�.
    system("pause");
}

void startTypingBossBattle_Menu() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*                                      *\n";
    std::cout << "*         << ������ ��ƶ� >>          *\n";
    std::cout << "*                                      *\n";
    std::cout << "****************************************\n\n";
    Sleep(1500); // 1.5�� �� ���� ����

    // ������ ���� �꼺�� ���� �Լ��� ȣ���մϴ�.
    startTypingBossBattle();

    // ������ ������ ��� ��� �� �޴��� ���ư��ϴ�.
    system("pause");
}
