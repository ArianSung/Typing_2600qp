#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <conio.h>    // _kbhit, _getch ��� (Windows ����)
#include <windows.h>  // gotoxy, Sleep, system ��� (Windows ����)

using namespace std;

// �Լ� ����
void startTypingPractice();
void startRainGame();
void startAiBattleGame();
void displayWinScreen();
void displayGameOverScreen();

//=========================================================
// Utility Ŭ����: �ܼ� ȭ�� ��� �ʿ��� ���
//=========================================================
class Utility {
public:
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

//=========================================================
// UIManager Ŭ����: �޴� ȭ�� UI ��� ���
//=========================================================
class UIManager {
public:
    void drawMainMenu() {
        system("cls");
        cout << "========================================\n";
        cout << "==             TYPING GAME            ==\n";
        cout << "========================================\n\n";
        cout << "           1. Ÿ�� ����\n";
        cout << "           2. �꼺�� ����\n";
        cout << "           3. �ܾ� ���\n";
        cout << "           4. ����\n\n";
        cout << "========================================\n";
        cout << ">> �޴��� �����ϼ���: ";
    }
};

//=========================================================
// GameManager Ŭ����: ��ü �帧 ����
//=========================================================
class GameManager {
private:
    UIManager uiManager;

public:
    void run() {
        while (true) {
            uiManager.drawMainMenu();
            char choice;
            cin >> choice;

            switch (choice) {
            case '1':
                startTypingPractice();
                break;
            case '2':
                startRainGame();
                break;
            case '3':
                startAiBattleGame();
                break;
            case '4':
                cout << "\n������ �����մϴ�.\n";
                return;
            default:
                cout << "\n>> �߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n";
                system("pause");
                break;
            }
        }
    }
};

//=========================================================
// main �Լ�: ���α׷� ������
//=========================================================
int main() {
    SetConsoleTitle(L"Typing Battle Game");
    GameManager game;
    game.run();
    return 0;
}


// 8x8 ���ڿ��� �ܾ��� ����(����, ��ǥ, ����)�� �����ϱ� ���� ����ü
struct GridWord {
    string text;
    int x, y;
    bool isActive = true;
};

/**
 * @brief 3. AI�� �ܾ� ����� ��� ���� (8x8 �׸��� ����)
 */
void startAiBattleGame() {
    // --- 1. ���� �ʱ� ���� ---
    system("cls");
    ifstream file("C:\\Users\\���\\Desktop\\words.txt");

    if (!file) {
        cout << "������ ã�� �� �����ϴ�. ��θ� Ȯ�����ּ���." << endl;
        cout << "���: C:\\Users\\���\\Desktop\\words.txt" << endl;
        system("pause");
        return;
    }

    vector<string> allWords;
    string word;
    while (file >> word) {
        allWords.push_back(word);
    }
    file.close();

    if (allWords.size() < 64) {
        cout << "words.txt ���Ͽ� �ܾ 64�� �̻� �ʿ��մϴ�." << endl;
        system("pause");
        return;
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(allWords.begin(), allWords.end(), g);

    vector<GridWord> gridWords(64);
    int startX = 3, startY = 3;
    int spacingX = 12, spacingY = 2;
    int activeWordCount = 64;

    for (int i = 0; i < 64; ++i) {
        gridWords[i].text = allWords[i];
        gridWords[i].x = startX + (i % 8) * spacingX;
        gridWords[i].y = startY + (i / 8) * spacingY;
    }

    int playerScore = 0;
    int aiScore = 0;
    string userInput = "";
    int aiTimer = 0;
    const int AI_SPEED = 45;

    auto startTime = chrono::steady_clock::now();
    double timeLimit = 10.0;

    // --- 2. ���� ���� ���� ---
    while (true) {
        auto currentTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = currentTime - startTime;
        double remainingTime = timeLimit - elapsedTime.count();

        if (remainingTime <= 0 || activeWordCount <= 0) {
            break;
        }

        Utility::gotoxy(0, 0);
        cout << "================================== AI BATTLE =====================================\n";
        cout << "                          ���� �ð�: " << static_cast<int>(remainingTime) << "��                                 \n";
        cout << "==================================================================================\n";

        for (const auto& gw : gridWords) {
            Utility::gotoxy(gw.x, gw.y);
            if (gw.isActive) {
                cout << gw.text;
            }
            else {
                cout << string(gw.text.length(), ' ');
            }
        }

        Utility::gotoxy(0, 20);
        cout << "==================================================================================\n";
        cout << "  [ ��: " << playerScore << " ]  vs  [ AI: " << aiScore << " ]\n";
        cout << "==================================================================================\n";
        cout << "  �Է�: " << userInput << "                  ";

        aiTimer++;
        if (aiTimer > AI_SPEED && activeWordCount > 0) {
            aiTimer = 0;
            vector<int> activeIndices;
            for (int i = 0; i < gridWords.size(); ++i) {
                if (gridWords[i].isActive) activeIndices.push_back(i);
            }
            if (!activeIndices.empty()) {
                uniform_int_distribution<> dist(0, activeIndices.size() - 1);
                int targetIndex = activeIndices[dist(g)];
                gridWords[targetIndex].isActive = false;
                aiScore++;
                activeWordCount--;
            }
        }

        if (_kbhit()) {
            char ch = _getch();
            if (ch == 8) {
                if (!userInput.empty()) userInput.pop_back();
            }
            else if (ch == 13) {
                for (auto& gw : gridWords) {
                    if (gw.isActive && gw.text == userInput) {
                        gw.isActive = false;
                        playerScore++;
                        activeWordCount--;
                        break;
                    }
                }
                userInput = "";
            }
            else if (isprint(ch)) {
                userInput += ch;
            }
        }
        Sleep(50);
    }

    // --- 3. ���� ���� ȭ�� ---
    system("cls"); // ���� ȭ���� ������ ����ϴ�.

    if (playerScore > aiScore) {
        displayWinScreen(); // �¸� ȭ�� ȣ��
    }
    else {
        displayGameOverScreen(); // �й� �Ǵ� ���º� ȭ�� ȣ��
    }

    Sleep(3000); // 3�� ���
    // �Լ��� ������ �ڵ����� run() �Լ��� while ������ ���ư� �޴��� �ٽ� ǥ�õ�
}

/**
 * @brief �¸����� �� ǥ�õ� ASCII ��Ʈ ȭ��
 */
void displayWinScreen() {
    cout << "\n\n\n\n";
    cout << "    $$$   $$$   $$$$$$$   $$$    $$$    $$$$$   \n";
    cout << "    $$$   $$$     $$$     $$$$   $$$    $$$$$   \n";
    cout << "    $$$ $ $$$     $$$     $$$ $$ $$$    $$$$$   \n";
    cout << "    $$$$$$$$$     $$$     $$$  $$$$$            \n";
    cout << "     $$$ $$$    $$$$$$$   $$$   $$$$    $$$$$   \n";
}

/**
 * @brief �й� �Ǵ� ���º� �� ǥ�õ� ASCII ��Ʈ ȭ��
 */
void displayGameOverScreen() {
    cout << "\n\n\n";
    cout << "    $$$$$$$    $$$$$    $$$    $$$  $$$$$$$$       $$$$$$   $$$    $$$  $$$$$$$$   $$$$$$$ \n";
    cout << "   $$$       $$$   $$$  $$$$$$$$$$  $$$           $$$  $$$  $$$    $$$  $$$        $$$  $$$\n";
    cout << "   $$$  $$$$ $$$$$$$$$  $$$ $$ $$$  $$$$$$$       $$$  $$$  $$$    $$$  $$$$$$$    $$$$$$ \n";
    cout << "   $$$   $$$ $$$   $$$  $$$    $$$  $$$           $$$  $$$   $$$  $$$   $$$        $$$  $$$\n";
    cout << "    $$$$$$$  $$$   $$$  $$$    $$$  $$$$$$$$       $$$$$$     $$$$$$    $$$$$$$$   $$$  $$$\n";
}



// (�� �Ʒ��� ��� �� �ٸ� ���� �Լ����Դϴ�)
void startTypingPractice() {
    system("cls");
    cout << "****************************************\n";
    cout << "* *\n";
    cout << "* << Ÿ�� ���� ���� ���� >>         *\n";
    cout << "* (�̰��� Ÿ�� ���� ���� �ڵ尡 ���ϴ�)   *\n";
    cout << "* *\n";
    cout << "****************************************\n\n";
    system("pause");
}

void startRainGame() {
    system("cls");
    cout << "****************************************\n";
    cout << "* *\n";
    cout << "* << �꼺�� ���� ���� >>          *\n";
    cout << "* (�̰��� �꼺�� ���� �ڵ尡 ���ϴ�)    *\n";
    cout << "* *\n";
    cout << "****************************************\n\n";
    system("pause");
}