// AiBattleGame.cpp

#include "AiBattleGame.h" // AI ��� ���� ���
#include "menu.h"  // Utility::gotoxy, displayWinScreen ���� ����ϱ� ���� ����

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <conio.h>   // _kbhit, _getch
#include <windows.h> // Sleep, system

using namespace std;

/**
 * @brief 3. AI�� �ܾ� ����� ��� ���� (8x8 �׸��� ����)
 */
void startAiBattleGame() {
    setConsoleSize(100, 30); // ���ϴ� ũ��� ���� (����)
    system("cls");
    // ���� ��δ� ��� ��η� �����ϴ� ���� �����ϴ�.
    ifstream file("words.txt");

    if (!file) {
        Utility::gotoxy(0, 0);
        cout << "'words.txt' ������ ã�� �� �����ϴ�." << endl;
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
    const int AI_SPEED = 45; // ���ڰ� �������� AI�� �������ϴ�.

    auto startTime = chrono::steady_clock::now();
    double timeLimit = 60.0; // �ð� ���� 60��

    // --- 2. ���� ���� ���� ---
    while (true) {
        auto currentTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = currentTime - startTime;
        double remainingTime = timeLimit - elapsedTime.count();

        // ���� ���� ����: �ð��� �� �ǰų� ��� �ܾ ������� ��
        if (remainingTime <= 0 || activeWordCount <= 0) {
            break;
        }

        // UI �׸���
        Utility::gotoxy(0, 0);
        cout << "========================================= AI BATTLE ==========================================\n";
        cout << "                                      ���� �ð�: " << static_cast<int>(remainingTime) << "��                                  \n";
        cout << "==============================================================================================\n";

        // �ܾ� ���� �׸���
        for (const auto& gw : gridWords) {
            Utility::gotoxy(gw.x, gw.y);
            if (gw.isActive) {
                cout << gw.text;
            }
            else {
                // ��Ȱ��ȭ�� �ܾ�� �������� ���� ����ϴ�.
                cout << string(gw.text.length(), ' ');
            }
        }

        // ������ �� �Է�â �׸���
        Utility::gotoxy(0, 20);
        cout << "==============================================================================================\n";
        cout << "  [ ��: " << playerScore << " ]  vs  [ AI: " << aiScore << " ]\n";
        cout << "==============================================================================================\n";
        cout << "  �Է�: " << userInput << string(30, ' '); // ���� �Է� �ܻ� ����

        // AI ����
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

        // ����� �Է� ó��
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 8) { // �齺���̽�
                if (!userInput.empty()) userInput.pop_back();
            }
            else if (ch == 13 || ch == 32) { // ����
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
        Sleep(50); // ���� �������� CPU ���� ����
    }

    // --- 3. ���� ���� ȭ�� ---
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

    // Ŀ�� ������ ���ֱ� (����)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
