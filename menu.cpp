#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <conio.h>    // _kbhit, _getch 사용 (Windows 전용)
#include <windows.h>  // gotoxy, Sleep, system 사용 (Windows 전용)

using namespace std;

// 함수 선언
void startTypingPractice();
void startRainGame();
void startAiBattleGame();
void displayWinScreen();
void displayGameOverScreen();

//=========================================================
// Utility 클래스: 콘솔 화면 제어에 필요한 기능
//=========================================================
class Utility {
public:
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

//=========================================================
// UIManager 클래스: 메뉴 화면 UI 출력 담당
//=========================================================
class UIManager {
public:
    void drawMainMenu() {
        system("cls");
        cout << "========================================\n";
        cout << "==             TYPING GAME            ==\n";
        cout << "========================================\n\n";
        cout << "           1. 타자 연습\n";
        cout << "           2. 산성비 게임\n";
        cout << "           3. 단어 대결\n";
        cout << "           4. 종료\n\n";
        cout << "========================================\n";
        cout << ">> 메뉴를 선택하세요: ";
    }
};

//=========================================================
// GameManager 클래스: 전체 흐름 관리
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
                cout << "\n게임을 종료합니다.\n";
                return;
            default:
                cout << "\n>> 잘못된 입력입니다. 다시 선택해주세요.\n";
                system("pause");
                break;
            }
        }
    }
};

//=========================================================
// main 함수: 프로그램 시작점
//=========================================================
int main() {
    SetConsoleTitle(L"Typing Battle Game");
    GameManager game;
    game.run();
    return 0;
}


// 8x8 격자에서 단어의 정보(내용, 좌표, 상태)를 관리하기 위한 구조체
struct GridWord {
    string text;
    int x, y;
    bool isActive = true;
};

/**
 * @brief 3. AI와 단어 지우기 대결 게임 (8x8 그리드 버전)
 */
void startAiBattleGame() {
    // --- 1. 게임 초기 설정 ---
    system("cls");
    ifstream file("C:\\Users\\모블\\Desktop\\words.txt");

    if (!file) {
        cout << "파일을 찾을 수 없습니다. 경로를 확인해주세요." << endl;
        cout << "경로: C:\\Users\\모블\\Desktop\\words.txt" << endl;
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
        cout << "words.txt 파일에 단어가 64개 이상 필요합니다." << endl;
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

    // --- 2. 메인 게임 루프 ---
    while (true) {
        auto currentTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = currentTime - startTime;
        double remainingTime = timeLimit - elapsedTime.count();

        if (remainingTime <= 0 || activeWordCount <= 0) {
            break;
        }

        Utility::gotoxy(0, 0);
        cout << "================================== AI BATTLE =====================================\n";
        cout << "                          남은 시간: " << static_cast<int>(remainingTime) << "초                                 \n";
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
        cout << "  [ 나: " << playerScore << " ]  vs  [ AI: " << aiScore << " ]\n";
        cout << "==================================================================================\n";
        cout << "  입력: " << userInput << "                  ";

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

    // --- 3. 게임 종료 화면 ---
    system("cls"); // 게임 화면을 완전히 지웁니다.

    if (playerScore > aiScore) {
        displayWinScreen(); // 승리 화면 호출
    }
    else {
        displayGameOverScreen(); // 패배 또는 무승부 화면 호출
    }

    Sleep(3000); // 3초 대기
    // 함수가 끝나면 자동으로 run() 함수의 while 루프로 돌아가 메뉴가 다시 표시됨
}

/**
 * @brief 승리했을 때 표시될 ASCII 아트 화면
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
 * @brief 패배 또는 무승부 시 표시될 ASCII 아트 화면
 */
void displayGameOverScreen() {
    cout << "\n\n\n";
    cout << "    $$$$$$$    $$$$$    $$$    $$$  $$$$$$$$       $$$$$$   $$$    $$$  $$$$$$$$   $$$$$$$ \n";
    cout << "   $$$       $$$   $$$  $$$$$$$$$$  $$$           $$$  $$$  $$$    $$$  $$$        $$$  $$$\n";
    cout << "   $$$  $$$$ $$$$$$$$$  $$$ $$ $$$  $$$$$$$       $$$  $$$  $$$    $$$  $$$$$$$    $$$$$$ \n";
    cout << "   $$$   $$$ $$$   $$$  $$$    $$$  $$$           $$$  $$$   $$$  $$$   $$$        $$$  $$$\n";
    cout << "    $$$$$$$  $$$   $$$  $$$    $$$  $$$$$$$$       $$$$$$     $$$$$$    $$$$$$$$   $$$  $$$\n";
}



// (이 아래는 비워 둔 다른 게임 함수들입니다)
void startTypingPractice() {
    system("cls");
    cout << "****************************************\n";
    cout << "* *\n";
    cout << "* << 타자 연습 게임 시작 >>         *\n";
    cout << "* (이곳에 타자 연습 게임 코드가 들어갑니다)   *\n";
    cout << "* *\n";
    cout << "****************************************\n\n";
    system("pause");
}

void startRainGame() {
    system("cls");
    cout << "****************************************\n";
    cout << "* *\n";
    cout << "* << 산성비 게임 시작 >>          *\n";
    cout << "* (이곳에 산성비 게임 코드가 들어갑니다)    *\n";
    cout << "* *\n";
    cout << "****************************************\n\n";
    system("pause");
}