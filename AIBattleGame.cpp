// AiBattleGame.cpp

#include "AiBattleGame.h" // AI 대결 게임 헤더
#include "menu.h"  // Utility::gotoxy, displayWinScreen 등을 사용하기 위해 포함

#include <iostream>
#include <fstream>
#include <sstream>      // [추가] wstringstream 사용
#include <random>
#include <algorithm>
#include <chrono>
#include <conio.h>   // _kbhit, _getch
#include <windows.h> // Sleep, system
#include <cwctype>      // [추가] iswprint 사용

// [추가] UTF-8 문자열을 wstring으로 변환하는 헬퍼 함수
static std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [추가] UTF-8 인코딩된 텍스트 파일을 wstring 벡터로 읽어오는 함수
static std::vector<std::wstring> loadWordsToWstring(const std::string& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return content;
    }
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    if (file_contents.empty()) {
        return content;
    }
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    std::wstring w_contents = utf8_to_wstring(file_contents);
    if (w_contents.empty()) {
        return content;
    }
    std::wstringstream wss(w_contents);
    std::wstring line;
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            content.push_back(line);
        }
    }
    return content;
}


void startAiBattleGame() {
    setConsoleSize(100, 30);
    system("cls");

    // [수정] 유니코드를 지원하는 파일 로딩 함수 사용
    std::vector<std::wstring> allWords = loadWordsToWstring("words.txt");

    if (allWords.empty()) {
        Utility::gotoxy(0, 0);
        std::wcout << L"'words.txt' 파일을 찾을 수 없거나 내용이 비어있습니다." << std::endl;
        system("pause");
        return;
    }

    if (allWords.size() < 64) {
        std::wcout << L"words.txt 파일에 단어가 64개 이상 필요합니다." << std::endl;
        system("pause");
        return;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allWords.begin(), allWords.end(), g);

    std::vector<GridWord> gridWords(64);
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
    std::wstring userInput = L""; // [수정] string -> wstring
    int aiTimer = 0;
    const int AI_SPEED = 45;

    auto startTime = std::chrono::steady_clock::now();
    double timeLimit = 60.0;

    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - startTime;
        double remainingTime = timeLimit - elapsedTime.count();

        if (remainingTime <= 0 || activeWordCount <= 0) {
            break;
        }

        // [수정] UI 그리기 (wcout 사용)
        Utility::gotoxy(0, 0);
        std::wcout << L"========================================= AI BATTLE ==========================================\n";
        std::wcout << L"                                      남은 시간: " << static_cast<int>(remainingTime) << L"초                                  \n";
        std::wcout << L"==============================================================================================\n";

        for (const auto& gw : gridWords) {
            Utility::gotoxy(gw.x, gw.y);
            if (gw.isActive) {
                std::wcout << gw.text;
            }
            else {
                std::wcout << std::wstring(gw.text.length(), L' ');
            }
        }

        Utility::gotoxy(0, 20);
        std::wcout << L"==============================================================================================\n";
        std::wcout << L"  [ 나: " << playerScore << L" ]  vs  [ AI: " << aiScore << L" ]\n";
        std::wcout << L"==============================================================================================\n";
        std::wcout << L"  입력: " << userInput << std::wstring(30, L' ');

        // AI 로직 (변경 없음)
        aiTimer++;
        if (aiTimer > AI_SPEED && activeWordCount > 0) {
            aiTimer = 0;
            std::vector<int> activeIndices;
            for (int i = 0; i < gridWords.size(); ++i) {
                if (gridWords[i].isActive) activeIndices.push_back(i);
            }
            if (!activeIndices.empty()) {
                std::uniform_int_distribution<> dist(0, activeIndices.size() - 1);
                int targetIndex = activeIndices[dist(g)];
                gridWords[targetIndex].isActive = false;
                aiScore++;
                activeWordCount--;
            }
        }

        // [수정] 사용자 입력 처리 (유니코드)
        if (_kbhit()) {
            wchar_t ch = _getwch(); // _getch -> _getwch
            if (ch == L'\b') { // 백스페이스
                if (!userInput.empty()) userInput.pop_back();
            }
            else if (ch == L'\r' || ch == L' ') { // 엔터 또는 스페이스
                for (auto& gw : gridWords) {
                    if (gw.isActive && gw.text == userInput) {
                        gw.isActive = false;
                        playerScore++;
                        activeWordCount--;
                        break;
                    }
                }
                userInput = L"";
            }
            else if (iswprint(ch)) { // isprint -> iswprint
                userInput += ch;
            }
            else if (ch == 27) // ESC
                break;
        }
        Sleep(50);
    }

    system("cls");
    if (playerScore > aiScore) {
        displayWinScreen();
    }
    else {
        displayGameOverScreen();
    }
    Sleep(3000);
}

// [복원] 실수로 삭제되었던 setConsoleSize 함수를 다시 추가합니다.
void setConsoleSize(int width, int height) {
    char command[128];
    // C++11 표준에 더 안전한 sprintf_s를 사용합니다.
    sprintf_s(command, "mode con cols=%d lines=%d", width, height);
    system(command);

    // 커서 깜빡임 없애기 (선택)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
