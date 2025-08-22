// Typing_Game_Rain.cpp
// 산성비(타자) 게임의 메인 소스 파일입니다.
// 단어가 위에서 아래로 떨어지고, 사용자가 단어를 입력해 없애는 게임입니다.

#include "Typing_Game_Rain.h"
#include "menu.h"

#include <iostream>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cwctype>

// [유틸리티] UTF-8 문자열을 유니코드(wstring)로 변환하는 함수
static std::wstring utf8_to_wstring_rain(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [유틸리티] words.txt 파일을 읽어 유니코드(wstring) 벡터로 반환하는 함수
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

// [유틸리티] 콘솔 커서를 (x, y) 위치로 이동시키는 함수
void gotoxy_raingame(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// [유틸리티] 콘솔 창 크기와 커서 숨김 설정 함수
void setConsoleSize() {
    system("mode con cols=80 lines=40"); // 콘솔 창 크기 설정
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE; // 커서 숨기기
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// [메인 함수] 산성비(타자) 게임 시작 함수
void startRainGame() {
    setConsoleSize(); // 콘솔 크기 및 커서 설정
    system("cls"); // 화면 지우기
    srand((unsigned int)time(NULL)); // 난수 시드 설정

    std::vector<std::wstring> words; // 단어 목록(유니코드)
    loadWordsFromFile(words, "words.txt");

    if (words.empty()) {
        gotoxy_raingame(0, 0);
        std::wcout << L"words.txt 파일에 단어가 없습니다! 게임을 시작할 수 없습니다." << std::endl;
        Sleep(2000);
        return;
    }

    std::vector<std::pair<std::wstring, COORD>> fallingWords; // 떨어지는 단어와 위치
    int score = 0; // 점수
    int life = 5; // 목숨(체력)
    std::wstring currentInput = L""; // 사용자가 입력 중인 내용

    clock_t lastUpdateTime = clock(); // 마지막 단어 낙하 시간
    clock_t wordCreationTime = clock(); // 마지막 단어 생성 시간
    int fallInterval = 500; // 단어 낙하 간격(ms)
    int creationInterval = 1500; // 단어 생성 간격(ms)

    // 게임 루프: 목숨이 0이 될 때까지 반복
    while (life > 0) {
        clock_t currentTime = clock();

        // 일정 시간마다 새로운 단어 생성
        if (currentTime - wordCreationTime > creationInterval && fallingWords.size() < 15) {
            std::wstring newWord = words[rand() % words.size()];
            int max_x = BOARD_WIDTH - newWord.length() - 1;
            if (max_x < 1) max_x = 1;
            COORD newPos = { (SHORT)(rand() % max_x + 1), 0 }; // 랜덤 X좌표, Y=0(맨 위)
            fallingWords.push_back({ newWord, newPos });
            wordCreationTime = currentTime;
        }

        // 키 입력 처리
        if (_kbhit()) {
            wchar_t ch = _getwch(); // 유니코드 문자 입력
            if (ch == L'\r' || ch == L' ') { // 엔터 또는 스페이스바
                for (auto it = fallingWords.begin(); it != fallingWords.end(); ++it) {
                    if (it->first == currentInput) {
                        int wordLength = it->first.length();
                        int gainedScore = (wordLength <= 4) ? 10 : 10 + (wordLength - 4); // 단어 길이에 따라 점수
                        score += gainedScore;

                        gotoxy_raingame(it->second.X, it->second.Y);
                        std::wcout << std::wstring(it->first.length(), L' '); // 단어 지우기

                        fallingWords.erase(it);
                        break;
                    }
                }
                currentInput = L""; // 입력창 비우기
            }
            else if (ch == L'\b') { // 백스페이스
                if (!currentInput.empty()) {
                    currentInput.pop_back();
                }
            }
            else if (iswprint(ch)) { // 일반 문자(한글, 영문, 특수문자 등)
                currentInput += ch;
            }
            else if (ch == 27) // ESC 키로 게임 종료
                break;
        }

        // 일정 시간마다 단어를 한 칸씩 아래로 이동
        if (currentTime - lastUpdateTime > fallInterval) {
            for (auto it = fallingWords.begin(); it != fallingWords.end(); ) {
                gotoxy_raingame(it->second.X, it->second.Y);
                std::wcout << std::wstring(it->first.length(), L' '); // 이전 위치 지우기

                it->second.Y++; // Y좌표 증가(아래로 이동)

                if (it->second.Y > BOARD_HEIGHT - 2) {
                    life--; // 바닥에 닿으면 목숨 감소
                    it = fallingWords.erase(it); // 단어 삭제
                }
                else {
                    gotoxy_raingame(it->second.X, it->second.Y);
                    std::wcout << it->first; // 새 위치에 단어 출력
                    ++it;
                }
            }
            lastUpdateTime = currentTime;
        }

        // 점수, 목숨, 입력창 등 UI 출력
        gotoxy_raingame(0, BOARD_HEIGHT - 2);
        std::wcout << L"Score: " << score << L" | Life: " << life << L"    ";
        gotoxy_raingame(0, BOARD_HEIGHT - 1);
        std::wcout << L"Input: " << currentInput << std::wstring(50, L' ');
    }

    // 게임 오버 화면 출력
    system("cls");
    gotoxy_raingame(BOARD_WIDTH / 2 - 15, BOARD_HEIGHT / 2);
    std::wcout << std::endl << std::endl << std::endl;
    std::wcout << L"                           Game Over! Your score is " << score << std::endl;
    Sleep(3000);
}
