#include "Boss_Game.h"
#include "AiBattleGame.h"
#include "menu.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <cctype> // isprint 함수를 사용하기 위해 추가

// Windows/Linux 호환을 위한 헤더 및 함수
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// --- 게임 데이터 구조 ---
struct Boss {
    int maxHp;
    int currentHp; // 현재 HP를 추적하기 위한 변수
    int attackPower;
    std::vector<std::string> art;
};

// --- 헬퍼 함수 ---

// 콘솔 커서 숨기기/보이기 함수 (깜빡임 제거용)
static void showCursor(bool show) {
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = show;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
#else
    printf(show ? "\033[?25h" : "\033[?25l");
#endif
}

// 콘솔 커서 위치를 이동시키는 함수 (깜빡임 방지용)
static void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

static void clearTypingScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void printHpBar(const std::string& name, int current, int max, int barWidth = 20) {
    std::cout << name << " HP: [";
    float hpRatio = (current > 0) ? static_cast<float>(current) / max : 0;
    int filledWidth = static_cast<int>(barWidth * hpRatio);

    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) std::cout << "■";
        else std::cout << " ";
    }
    // 라인의 나머지 부분을 공백으로 채워 이전 출력을 지웁니다.
    std::cout << "] " << current << "/" << max << "        ";
}

// --- 게임 메인 함수 ---
void startTypingBossBattle() {
    setConsoleSize(100, 30);
    showCursor(false); // 게임 시작 시 커서 숨기기

    // 1. 단어 목록 불러오기
    std::vector<std::string> allWords;
    std::ifstream file("words.txt");
    if (!file.is_open()) {
        std::cerr << "\n오류: words.txt 파일을 열 수 없습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        showCursor(true); // 함수 종료 전 커서 보이기
        return;
    }
    std::string word;
    while (file >> word) allWords.push_back(word);
    file.close();
    if (allWords.empty()) {
        std::cerr << "\n오류: words.txt 파일에 단어가 없습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        showCursor(true); // 함수 종료 전 커서 보이기
        return;
    }

    // 2. 10단계 보스 데이터 설정
    std::vector<Boss> stages(10);
    stages[0] = { 100, 0, 1, {"       (o.o)       ", "      /|   |\\      ", "       /   \\      "} };
    stages[1] = { 120, 0, 1, {"       (Ò.Ó)       ", "      /|]--[|\\      ", "       /   \\      "} };
    stages[2] = { 150, 0, 2, {"      <(ÒwÓ)>      ", "      /| | | |\\      ", "       /    \\      "} };
    stages[3] = { 200, 0, 2, {"     --{ÒwÓ}--     ", "      /| | | |\\      ", "      O-- --O      "} };
    stages[4] = { 250, 0, 3, {"    \\|/(ÒДÓ)\\|/    ", "      /| | | |\\      ", "       /\\ /\\       "} };
    stages[5] = { 300, 0, 3, {"  <(((Ò(エ)Ó)))>  ", "      /| | | |\\      ", "      <|   |>      "} };
    stages[6] = { 380, 0, 4, {"  ( ( (Ò益Ó) ) )  ", "   ((--/| | | |\\--))   ", "      <</ \\>>      "} };
    stages[7] = { 450, 0, 5, {"  (##(Ò益Ó)##)  ", "   ((--/| | | |\\--))   ", "     ##/   \\##     "} };
    stages[8] = { 550, 0, 6, {" /MM\\ (Ò益Ó) /MM\\ ", " M ((--/| | | |\\--)) M ", "     ##/   \\##     "} };
    stages[9] = { 700, 0, 7, {"/|================|\\", "| | ((Ò益Ó)) | |", "| |/----\\| |", "\\| | | |/", " VVV VVV "} };

    // 플레이어 HP를 스테이지 루프 밖에서 초기화합니다.
    int playerMaxHp = 10;
    int playerCurrentHp = playerMaxHp;

    // 3. 게임 루프 시작 (스테이지별)
    for (int stage = 0; stage < 10; ++stage) {
        // 현재 스테이지 게임 변수 설정
        Boss currentBoss = stages[stage];
        currentBoss.currentHp = currentBoss.maxHp; // 현재 HP 초기화
        int successfulAttacks = 0;

        auto lastBossAttackTime = std::chrono::steady_clock::now();
        std::string currentWord;
        std::string userInput = "";

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        currentWord = allWords[0];

        // --- 초기 화면 그리기 ---
        clearTypingScreen();
        std::cout << "========================= STAGE " << stage + 1 << " =========================" << std::endl;
        std::cout << "\n";
        int art_start_y = 2;
        for (const auto& line : currentBoss.art) {
            gotoxy(0, art_start_y++);
            std::cout << line << std::endl;
        }

        const int BOSS_HP_Y = art_start_y + 2;
        const int PLAYER_HP_Y = BOSS_HP_Y + 1;
        const int DIVIDER_Y = PLAYER_HP_Y + 2;
        const int TIMER_Y = DIVIDER_Y + 1;
        const int WORD_Y = TIMER_Y + 3;
        const int INPUT_Y = WORD_Y + 2;
        const int MESSAGE_Y = INPUT_Y + 2;

        gotoxy(0, DIVIDER_Y);
        std::cout << "-----------------------------------------------------------";
        gotoxy(0, WORD_Y);
        std::cout << ">> " << currentWord << " <<";

        // 4. 전투 루프 시작 (현재 스테이지)
        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            float bossAttackTimer = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastBossAttackTime).count();

            // --- UI 업데이트 (깜빡임 최소화) ---
            gotoxy(0, BOSS_HP_Y);
            printHpBar("BOSS  ", currentBoss.currentHp, currentBoss.maxHp, 40);
            gotoxy(0, PLAYER_HP_Y);
            printHpBar("PLAYER", playerCurrentHp, playerMaxHp, 40);
            gotoxy(0, TIMER_Y);
            std::cout << "보스 공격까지: " << 10 - static_cast<int>(bossAttackTimer) << "초 (성공횟수: " << successfulAttacks << "/4)      ";
            gotoxy(7, INPUT_Y); // "입력: " 뒤로 커서 이동
            std::cout << userInput << " "; // 이전 글자 지우기용 공백

            // 보스 공격 처리
            if (bossAttackTimer >= 10.0f) {
                if (successfulAttacks < 4) {
                    playerCurrentHp -= currentBoss.attackPower;
                    gotoxy(0, MESSAGE_Y);
                    std::cout << "보스의 공격! " << currentBoss.attackPower << "의 데미지를 입었습니다!        ";
                }
                else {
                    gotoxy(0, MESSAGE_Y);
                    std::cout << "보스의 공격을 막아냈습니다!                      ";
                }
                successfulAttacks = 0;
                lastBossAttackTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gotoxy(0, MESSAGE_Y);
                std::cout << "                                                  "; // 메시지 지우기
            }

            // 게임 오버 조건 확인
            if (playerCurrentHp <= 0) {
                clearTypingScreen();
                std::cout << "\n\n패배했습니다...\n\n";
                goto game_over;
            }

            // 키 입력 처리 (Windows 전용 비동기 방식)
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '\r' || ch == '\n' || ch == ' ') { // Enter 키, space 키
                    if (userInput == currentWord) {
                        currentBoss.currentHp -= 10; // 현재 HP를 감소시킴
                        successfulAttacks++;
                        if (currentBoss.currentHp <= 0) break; // 보스 처치

                        std::shuffle(allWords.begin(), allWords.end(), g);
                        currentWord = allWords[0];
                        gotoxy(0, WORD_Y);
                        std::cout << ">> " << currentWord << " <<" << "                    ";
                    }
                    // 입력창을 깨끗하게 비웁니다.
                    gotoxy(7, INPUT_Y);
                    std::cout << "                                        ";
                    userInput = "";
                }
                else if (ch == '\b') { // Backspace 키
                    if (!userInput.empty()) userInput.pop_back();
                }
                else if (isprint(ch)) { // 일반 문자
                    userInput += ch;
                }
                else if (ch == 27) {
                    goto game_over;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // 스테이지 클리어
        clearTypingScreen();
        std::cout << "\n\n축하합니다! STAGE " << stage + 1 << " 클리어!\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // 모든 스테이지 클리어
    clearTypingScreen();
    std::cout << "\n\n모든 보스를 물리쳤습니다! 당신은 타자의 신입니다!\n\n";

game_over:
    displayGameOverScreen();
    std::cout << "\n타자 보스전이 종료되었습니다. 잠시 후 메뉴로 돌아갑니다." << std::endl;
    showCursor(true); // 게임 종료 시 커서 다시 보이기
    std::this_thread::sleep_for(std::chrono::seconds(3));
}


