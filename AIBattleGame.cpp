// AiBattleGame.cpp

#include "AiBattleGame.h" // AI 대결 게임 헤더
#include "menu.h"  // Utility::gotoxy, displayWinScreen 등을 사용하기 위해 포함

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <conio.h>   // _kbhit, _getch
#include <windows.h> // Sleep, system

using namespace std;

/**
 * @brief 3. AI와 단어 지우기 대결 게임 (8x8 그리드 버전)
 */
void startAiBattleGame() {
    setConsoleSize(100, 30); // 원하는 크기로 지정 (예시)
    system("cls");
    // 파일 경로는 상대 경로로 지정하는 것이 좋습니다.
    ifstream file("words.txt");

    if (!file) {
        Utility::gotoxy(0, 0);
        cout << "'words.txt' 파일을 찾을 수 없습니다." << endl;
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
    const int AI_SPEED = 45; // 숫자가 작을수록 AI가 빨라집니다.

    auto startTime = chrono::steady_clock::now();
    double timeLimit = 60.0; // 시간 제한 60초

    // --- 2. 메인 게임 루프 ---
    while (true) {
        auto currentTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = currentTime - startTime;
        double remainingTime = timeLimit - elapsedTime.count();

        // 게임 종료 조건: 시간이 다 되거나 모든 단어가 사라졌을 때
        if (remainingTime <= 0 || activeWordCount <= 0) {
            break;
        }

        // UI 그리기
        Utility::gotoxy(0, 0);
        cout << "========================================= AI BATTLE ==========================================\n";
        cout << "                                      남은 시간: " << static_cast<int>(remainingTime) << "초                                  \n";
        cout << "==============================================================================================\n";

        // 단어 격자 그리기
        for (const auto& gw : gridWords) {
            Utility::gotoxy(gw.x, gw.y);
            if (gw.isActive) {
                cout << gw.text;
            }
            else {
                // 비활성화된 단어는 공백으로 덮어 지웁니다.
                cout << string(gw.text.length(), ' ');
            }
        }

        // 점수판 및 입력창 그리기
        Utility::gotoxy(0, 20);
        cout << "==============================================================================================\n";
        cout << "  [ 나: " << playerScore << " ]  vs  [ AI: " << aiScore << " ]\n";
        cout << "==============================================================================================\n";
        cout << "  입력: " << userInput << string(30, ' '); // 이전 입력 잔상 제거

        // AI 로직
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

        // 사용자 입력 처리
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 8) { // 백스페이스
                if (!userInput.empty()) userInput.pop_back();
            }
            else if (ch == 13 || ch == 32) { // 엔터
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
            else if (ch == 27)
                break;
        }
        Sleep(50); // 루프 지연으로 CPU 사용률 감소
    }

    // --- 3. 게임 종료 화면 ---
    system("cls");
    if (playerScore > aiScore) {
        displayWinScreen();
    }
    else {
        displayGameOverScreen();
    }
    Sleep(3000);
}

void setConsoleSize(int width, int height) {
    char command[128];
    sprintf_s(command, "mode con cols=%d lines=%d", width, height);
    system(command);

    // 커서 깜빡임 없애기 (선택)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
