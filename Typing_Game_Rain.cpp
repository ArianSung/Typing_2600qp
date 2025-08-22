// TypingGame.cpp

#include "Typing_Game_Rain.h"
#include "menu.h"

#include <iostream>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cwctype>

// [추가] UTF-8 변환 및 파일 로딩 헬퍼 함수
static std::wstring utf8_to_wstring_rain(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [수정] 기존 loadWordsFromFile 함수를 유니코드 지원 버전으로 교체
void loadWordsFromFile(std::vector<std::wstring>& words, const std::string& filename) {
    words.clear();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        gotoxy_raingame(0, 0);
        std::wcout << L"'" << utf8_to_wstring_rain(filename) << L"' 파일을 열 수 없습니다!" << std::endl;
        Sleep(2000);
        exit(1);
    }
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    std::wstring w_contents = utf8_to_wstring_rain(file_contents);
    std::wstringstream wss(w_contents);
    std::wstring line;
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty()) words.push_back(line);
    }
}


// ## 유틸리티 함수 정의 ##

void gotoxy_raingame(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setConsoleSize() {
    system("mode con cols=80 lines=40");
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


// ## 메인 게임 로직 함수 정의 ##

void startRainGame() {
    setConsoleSize();
    system("cls");
    srand((unsigned int)time(NULL));

    std::vector<std::wstring> words; // [수정] string -> wstring
    loadWordsFromFile(words, "words.txt");

    if (words.empty()) {
        gotoxy_raingame(0, 0);
        std::wcout << L"words.txt 파일에 단어가 없습니다! 게임을 시작할 수 없습니다." << std::endl;
        Sleep(2000);
        return;
    }

    std::vector<std::pair<std::wstring, COORD>> fallingWords; // [수정] string -> wstring
    int score = 0;
    int life = 5;
    std::wstring currentInput = L""; // [수정] string -> wstring

    clock_t lastUpdateTime = clock();
    clock_t wordCreationTime = clock();
    int fallInterval = 500;
    int creationInterval = 1500;

    while (life > 0) {
        clock_t currentTime = clock();

        if (currentTime - wordCreationTime > creationInterval && fallingWords.size() < 15) {
            std::wstring newWord = words[rand() % words.size()];
            int max_x = BOARD_WIDTH - newWord.length() - 1;
            if (max_x < 1) max_x = 1;
            COORD newPos = { (SHORT)(rand() % max_x + 1), 0 };
            fallingWords.push_back({ newWord, newPos });
            wordCreationTime = currentTime;
        }

        if (_kbhit()) {
            wchar_t ch = _getwch(); // [수정] _getch -> _getwch
            if (ch == L'\r' || ch == L' ') { // 엔터 또는 스페이스바
                for (auto it = fallingWords.begin(); it != fallingWords.end(); ++it) {
                    if (it->first == currentInput) {
                        int wordLength = it->first.length();
                        int gainedScore = (wordLength <= 4) ? 10 : 10 + (wordLength - 4);
                        score += gainedScore;

                        gotoxy_raingame(it->second.X, it->second.Y);
                        std::wcout << std::wstring(it->first.length(), L' ');

                        fallingWords.erase(it);
                        break;
                    }
                }
                currentInput = L"";
            }
            else if (ch == L'\b') { // 백스페이스
                if (!currentInput.empty()) {
                    currentInput.pop_back();
                }
            }
            else if (iswprint(ch)) { // [수정] isprint -> iswprint
                currentInput += ch;
            }
            else if (ch == 27)
                break;
        }

        if (currentTime - lastUpdateTime > fallInterval) {
            for (auto it = fallingWords.begin(); it != fallingWords.end(); ) {
                gotoxy_raingame(it->second.X, it->second.Y);
                std::wcout << std::wstring(it->first.length(), L' ');

                it->second.Y++;

                if (it->second.Y > BOARD_HEIGHT - 2) {
                    life--;
                    it = fallingWords.erase(it);
                }
                else {
                    gotoxy_raingame(it->second.X, it->second.Y);
                    std::wcout << it->first; // [수정] cout -> wcout
                    ++it;
                }
            }
            lastUpdateTime = currentTime;
        }

        gotoxy_raingame(0, BOARD_HEIGHT - 2);
        std::wcout << L"Score: " << score << L" | Life: " << life << L"    ";
        gotoxy_raingame(0, BOARD_HEIGHT - 1);
        std::wcout << L"Input: " << currentInput << std::wstring(50, L' ');
    }

    system("cls");
    gotoxy_raingame(BOARD_WIDTH / 2 - 15, BOARD_HEIGHT / 2);
    std::wcout << std::endl << std::endl << std::endl;
    std::wcout << L"                           Game Over! Your score is " << score << std::endl;
    Sleep(3000);
}
