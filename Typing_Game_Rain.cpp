// TypingGame.cpp

#include "Typing_Game_Rain.h" // 우리가 만든 헤더 파일을 포함합니다.
#include "menu.h"

// 추가적으로 이 파일 내부에서만 필요한 헤더들을 포함합니다.
#include <iostream>
#include <conio.h>   // _kbhit, _getch
#include <ctime>     // time, clock
#include <fstream>   // ifstream

// ## 유틸리티 함수 정의 ##

// 지정된 좌표로 콘솔 커서를 이동시키는 함수
void gotoxy_raingame(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 콘솔 창의 크기를 설정하고 커서를 숨기는 함수
void setConsoleSize() {
    char command[128];
    // C++ 스타일로 안전하게 문자열을 만들기 위해 snprintf를 사용하는 것이 더 좋습니다.
    snprintf(command, sizeof(command), "mode con cols=%d lines=%d", BOARD_WIDTH, BOARD_HEIGHT);
    system(command);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 'words.txt' 파일에서 단어 목록을 불러오는 함수
void loadWordsFromFile(std::vector<std::string>& words, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        gotoxy_raingame(0, 0);
        std::cout << filename << " 파일을 열 수 없습니다!" << std::endl;
        Sleep(2000);
        exit(1); // 파일을 못 열면 게임 진행이 불가능하므로 종료
    }
    std::string word;
    while (file >> word) {
        words.push_back(word);
    }
    file.close();
}


// ## 메인 게임 로직 함수 정의 ##

void playTypingGame() {
    // --- 1. 게임 초기화 ---
    setConsoleSize();
    system("cls");
    srand((unsigned int)time(NULL));

    std::vector<std::string> words;
    loadWordsFromFile(words, "words.txt");

    if (words.empty()) {
        gotoxy_raingame(0, 0);
        std::cout << "words.txt 파일에 단어가 없습니다! 게임을 시작할 수 없습니다." << std::endl;
        Sleep(2000);
        return;
    }

    std::vector<std::pair<std::string, COORD>> fallingWords;
    int score = 0;
    int life = 5;
    std::string currentInput = "";

    clock_t lastUpdateTime = clock();
    clock_t wordCreationTime = clock();
    int fallInterval = 500;
    int creationInterval = 1500;

    // --- 2. 메인 게임 루프 ---
    while (life > 0) {
        clock_t currentTime = clock();

        // 단어 생성 로직
        if (currentTime - wordCreationTime > creationInterval && fallingWords.size() < 15) {
            std::string newWord = words[rand() % words.size()];
            int max_x = BOARD_WIDTH - newWord.length() - 1;
            if (max_x < 1) max_x = 1;
            COORD newPos = { (SHORT)(rand() % max_x + 1), 0 };
            fallingWords.push_back({ newWord, newPos });
            wordCreationTime = currentTime;
        }

        // 사용자 입력 처리 로직
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 13 || ch == 32) { // 엔터 또는 스페이스바
                for (auto it = fallingWords.begin(); it != fallingWords.end(); ++it) {
                    if (it->first == currentInput) {
                        // 단어 길이에 따른 점수 계산 로직
                        int wordLength = it->first.length();
                        int gainedScore = (wordLength <= 4) ? 10 : 10 + (wordLength - 4);
                        score += gainedScore;

                        // 맞춘 단어 화면에서 지우기
                        gotoxy_raingame(it->second.X, it->second.Y);
                        std::cout << std::string(it->first.length(), ' ');

                        fallingWords.erase(it);
                        break; // 단어를 찾았으면 루프 종료
                    }
                }
                currentInput = ""; // 입력 초기화
            }
            else if (ch == 8) { // 백스페이스
                if (!currentInput.empty()) {
                    currentInput.pop_back();
                }
            }
            else if (isprint(ch)) { // 출력 가능한 문자인 경우
                currentInput += ch;
            }
            else if (ch == 27)
                break;
        }

        // 단어 낙하 및 화면 그리기 로직 (시간 기반)
        if (currentTime - lastUpdateTime > fallInterval) {
            for (auto it = fallingWords.begin(); it != fallingWords.end(); ) {
                // 이전 위치 지우기
                gotoxy_raingame(it->second.X, it->second.Y);
                std::cout << std::string(it->first.length(), ' ');

                it->second.Y++; // y좌표 증가 (낙하)

                // 바닥에 닿았는지 체크
                if (it->second.Y > BOARD_HEIGHT - 2) {
                    life--;
                    it = fallingWords.erase(it); // 바닥에 닿은 단어 삭제
                }
                else {
                    // 새 위치에 단어 그리기
                    gotoxy_raingame(it->second.X, it->second.Y);
                    std::cout << it->first;
                    ++it;
                }
            }
            lastUpdateTime = currentTime;
        }

        // UI 그리기 로직
        gotoxy_raingame(0, BOARD_HEIGHT - 2);
        std::cout << "Score: " << score << " | Life: " << life << "    ";
        gotoxy_raingame(0, BOARD_HEIGHT - 1);
        std::cout << "Input: " << currentInput << std::string(50, ' '); // 이전 입력 잔상 제거
    }

    // --- 3. 게임 오버 처리 ---
    system("cls");
    gotoxy_raingame(BOARD_WIDTH / 2 - 10, BOARD_HEIGHT / 2);
    std::cout << std::endl << std::endl << std::endl;
    std::cout << "                           Game Over! Your score is " << score << std::endl;
    Sleep(3000);
}
