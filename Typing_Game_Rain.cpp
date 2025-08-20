#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>

// 전역으로 사용할 상수 정의
#define BOARD_WIDTH 80
#define BOARD_HEIGHT 40

// ## 유틸리티 함수들 ##

// 지정된 좌표로 콘솔 커서를 이동시키는 함수
void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 콘솔 창의 크기를 설정하고 커서를 숨기는 함수
void setConsoleSize() {
    char command[128];
    sprintf_s(command, "mode con cols=%d lines=%d", BOARD_WIDTH, BOARD_HEIGHT);
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
        gotoxy(0, 0);
        std::cout << filename << " 파일을 열 수 없습니다!" << std::endl;
        Sleep(2000);
        exit(1);
    }
    std::string word;
    while (file >> word) {
        words.push_back(word);
    }
    file.close();
}

// ## 메인 게임 로직 함수 ##

void playTypingGame() {
    // --- 1. 게임 초기화 ---
    setConsoleSize(); // 게임 시작 시점에 맞춰 창 크기 변경
    system("cls"); // 게임 시작 전 화면 정리
    srand((unsigned int)time(NULL));

    std::vector<std::string> words;
    loadWordsFromFile(words, "words.txt");

    if (words.empty()) {
        gotoxy(0, 0);
        std::cout << "words.txt 파일에 단어가 없습니다! 게임을 시작할 수 없습니다." << std::endl;
        Sleep(2000);
        return; // main으로 복귀
    }

    std::vector<std::pair<std::string, COORD>> fallingWords;
    int score = 0;
    int life = 5;
    std::string currentInput = "";

    // 시간 기반 로직을 위한 변수들
    clock_t lastUpdateTime = clock();
    clock_t wordCreationTime = clock();
    int fallInterval = 500;
    int creationInterval = 1500; // 단어 생성 빈도 조절 (1.5초)

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
                        // --- 단어 길이에 따른 점수 계산 로직 ---
                        int wordLength = it->first.length();
                        int gainedScore = 0;

                        if (wordLength <= 4) {
                            gainedScore = 10; // 4글자 이하면 기본 10점
                        }
                        else {
                            // 4글자 초과 시, 기본 10점에 초과된 글자 수만큼 점수 추가
                            gainedScore = 10 + (wordLength - 4);
                        }
                        score += gainedScore;
                        // --- 점수 계산 로직 끝 ---

                        gotoxy(it->second.X, it->second.Y);
                        std::cout << std::string(it->first.length(), ' ');
                        fallingWords.erase(it);
                        break;
                    }
                }
                currentInput = "";
            }
            else if (ch == 8) { // 백스페이스
                if (!currentInput.empty()) {
                    currentInput.pop_back();
                }
            }
            else if (isprint(ch)) {
                currentInput += ch;
            }
        }

        // 단어 낙하 및 화면 그리기 로직 (시간 기반)
        if (currentTime - lastUpdateTime > fallInterval) {
            for (auto it = fallingWords.begin(); it != fallingWords.end(); ) {
                gotoxy(it->second.X, it->second.Y);
                std::cout << std::string(it->first.length(), ' ');

                it->second.Y++;

                if (it->second.Y > BOARD_HEIGHT - 2) {
                    life--;
                    it = fallingWords.erase(it);
                }
                else {
                    gotoxy(it->second.X, it->second.Y);
                    std::cout << it->first;
                    ++it;
                }
            }
            lastUpdateTime = currentTime;
        }

        // UI 그리기 로직
        gotoxy(0, BOARD_HEIGHT - 2);
        std::cout << "Score: " << score << " | Life: " << life << "    ";
        gotoxy(0, BOARD_HEIGHT - 1);
        std::cout << "Input: " << currentInput << std::string(50, ' ');
    }

    // --- 3. 게임 오버 처리 ---
    system("cls");
    gotoxy(BOARD_WIDTH / 2 - 10, BOARD_HEIGHT / 2);
    std::cout << "Game Over! Your score is " << score << std::endl;
    Sleep(3000);
}


//int main() {
//    // 프로그램의 기본 콘솔 환경 설정
//    // (메뉴 표시 등을 위해 필요하다면 여기에 추가)
//
//    // 메뉴를 표시하고 사용자 선택을 받는 로직을 구현할 수 있습니다.
//    // ...
//
//    // 사용자가 게임 시작을 선택했다고 가정하고 게임 함수 호출
//    playTypingGame();
//
//    return 0;
//}