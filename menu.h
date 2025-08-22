#pragma once
// GameManager.h

#pragma once // 헤더 파일의 중복 포함을 방지합니다.

// 클래스 및 함수 선언에 필요한 최소한의 헤더만 포함합니다.
#include <iostream>
#include <windows.h>

//=========================================================
// 클래스 선언
//=========================================================

// Utility 클래스: 콘솔 화면 제어와 같은 보조 기능을 담당합니다.
class Utility {
public:
    // static 함수는 인스턴스 없이 'Utility::gotoxy(x, y)' 형태로 바로 호출할 수 있습니다.
    // 간단한 기능이므로 헤더 파일에 바로 정의합니다.
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

// UIManager 클래스: 메뉴 화면 등 UI 출력을 담당합니다.
class UIManager {
public:
    void drawMainMenu(); // 함수의 선언
    void drawGameMenu();
};

// GameManager 클래스: 게임의 전체적인 흐름을 관리합니다.
class GameManager {
private:
    UIManager uiManager; // UI 관리 객체를 멤버로 가집니다.

public:
    void run(); // 게임 실행 루프 함수의 선언
};


//=========================================================
// 전역 함수 선언
//=========================================================

void startTypingPractice_Menu();
void startRainGame_Menu();
void displayWinScreen();
void displayGameOverScreen();
void startAiBattleGame_Menu();
void startMemoryChallenge_Menu();
void startTypingBossBattle_Menu();
// Boss_Game.cpp
// 타자 보스전 게임의 메인 소스 파일입니다.
// 이 게임은 여러 단계의 보스를 상대로 제한 시간 내에 단어를 입력해 공격하는 방식입니다.

#include "Boss_Game.h"
#include "AiBattleGame.h"
#include "menu.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <cwctype> // 유니코드 문자 입력 처리를 위한 헤더

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
// 비-Windows 환경에서는 별도의 입력 처리가 필요합니다.
#endif

// [유틸리티] UTF-8 문자열을 유니코드(wstring)로 변환하는 함수
static std::wstring utf8_to_wstring_boss(const std::string& str) {
    if (str.empty()) return std::wstring();
    // UTF-8 문자열을 유니코드(wstring)로 변환
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [유틸리티] words.txt 파일을 읽어 유니코드(wstring) 벡터로 반환하는 함수
static std::vector<std::wstring> loadWordsToWstring_boss(const std::string& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary); // 바이너리 모드로 파일 열기
    if (!file.is_open()) return content;
    // 파일 전체를 읽어 string으로 저장
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // UTF-8 BOM(파일 맨 앞 3바이트) 제거
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    // UTF-8을 wstring으로 변환
    std::wstring w_contents = utf8_to_wstring_boss(file_contents);
    std::wstringstream wss(w_contents);
    std::wstring line;
    // 한 줄씩 읽어서 벡터에 저장
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back(); // CR 문자 제거
        if (!line.empty()) content.push_back(line);
    }
    return content;
}

// [자료구조] 보스 정보를 저장하는 구조체
struct Boss {
    int maxHp; // 보스의 최대 체력
    int currentHp; // 보스의 현재 체력
    int attackPower; // 보스의 공격력(한 번에 깎이는 플레이어 HP)
    std::vector<std::wstring> art; // 보스의 아스키 아트(유니코드)
};

// [유틸리티] 콘솔 커서 깜빡임을 숨기거나 보이게 하는 함수
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

// [유틸리티] 콘솔 커서를 (x, y) 위치로 이동시키는 함수
static void gotoxy_boss(int x, int y) {
#ifdef _WIN32
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

// [유틸리티] 콘솔 화면을 지우는 함수
static void clearTypingScreen() {
    system("cls");
}

// [유틸리티] 체력 바를 출력하는 함수 (유니코드 wcout 사용)
static void printHpBar(const std::wstring& name, int current, int max, int barWidth = 20) {
    std::wcout << name << L" HP: [";
    float hpRatio = (current > 0) ? static_cast<float>(current) / max : 0;
    int filledWidth = static_cast<int>(barWidth * hpRatio);

    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) std::wcout << L"■"; // 체력이 남은 부분
        else std::wcout << L" "; // 체력이 없는 부분
    }
    std::wcout << L"] " << current << L"/" << max << L"        ";
}

// [메인 함수] 타자 보스전 게임 시작 함수
void startTypingBossBattle() {
    setConsoleSize(100, 30); // 콘솔 창 크기 설정
    showCursor(false); // 커서 숨기기

    // 1. 단어 목록 불러오기 (words.txt에서 유니코드로)
    std::vector<std::wstring> allWords = loadWordsToWstring_boss("words.txt");
    if (allWords.empty()) {
        std::wcerr << L"\n오류: words.txt 파일을 열 수 없거나 내용이 비어있습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        showCursor(true);
        return;
    }

    // 2. 10단계 보스 데이터 설정 (각 보스마다 HP, 공격력, 아트가 다름)
    std::vector<Boss> stages(10);
    stages[0] = { 100, 0, 1, {L"       (o.o)       ", L"      /|   |\\      ", L"       /   \\      "} };
    stages[1] = { 120, 0, 1, {L"       (?.?)       ", L"      /|]--[|\\      ", L"       /   \\      "} };
    stages[2] = { 150, 0, 2, {L"      <(?w?)>      ", L"      /| | | |\\      ", L"       /    \\      "} };
    stages[3] = { 200, 0, 2, {L"     --{?w?}--     ", L"      /| | | |\\      ", L"      O-- --O      "} };
    stages[4] = { 250, 0, 3, {L"    \\|/(?Д?)\\|/    ", L"      /| | | |\\      ", L"       /\\ /\\       "} };
    stages[5] = { 300, 0, 3, {L"  <(((?(エ)?)))>  ", L"      /| | | |\\      ", L"      <|   |>      "} };
    stages[6] = { 380, 0, 4, {L"  ( ( (?益?) ) )  ", L"   ((--/| | | |\\--))   ", L"      <</ \\>>      "} };
    stages[7] = { 450, 0, 5, {L"  (##(?益?)##)  ", L"   ((--/| | | |\\--))   ", L"     ##/   \\##     "} };
    stages[8] = { 550, 0, 6, {L" /MM\\ (?益?) /MM\\ ", L" M ((--/| | | |\\--)) M ", L"     ##/   \\##     "} };
    stages[9] = { 700, 0, 7, {L"/|================|\\", L"| | ((?益?)) | |", L"| |/----\\| |", L"\\| | | |/", L" VVV VVV "} };

    int playerMaxHp = 10; // 플레이어 최대 체력
    int playerCurrentHp = playerMaxHp; // 플레이어 현재 체력

    // 3. 각 스테이지(보스)별로 반복
    for (int stage = 0; stage < 10; ++stage) {
        Boss currentBoss = stages[stage];
        currentBoss.currentHp = currentBoss.maxHp; // 보스 체력 초기화
        int successfulAttacks = 0; // 10초 내에 성공한 공격 횟수

        auto lastBossAttackTime = std::chrono::steady_clock::now();
        std::wstring currentWord; // 현재 입력해야 할 단어
        std::wstring userInput = L""; // 사용자가 입력 중인 내용

        // 단어 목록을 랜덤하게 섞고, 첫 번째 단어를 선택
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        currentWord = allWords[0];

        // --- 화면 초기화 및 보스 아트 출력 ---
        clearTypingScreen();
        std::wcout << L"========================= STAGE " << stage + 1 << L" =========================" << std::endl;
        std::wcout << L"\n";
        int art_start_y = 2;
        for (const auto& line : currentBoss.art) {
            gotoxy_boss(0, art_start_y++);
            std::wcout << line << std::endl;
        }

        // 각종 UI 위치 계산
        const int BOSS_HP_Y = art_start_y + 2;
        const int PLAYER_HP_Y = BOSS_HP_Y + 1;
        const int DIVIDER_Y = PLAYER_HP_Y + 2;
        const int TIMER_Y = DIVIDER_Y + 1;
        const int WORD_Y = TIMER_Y + 3;
        const int INPUT_Y = WORD_Y + 2;
        const int MESSAGE_Y = INPUT_Y + 2;

        // UI 출력
        gotoxy_boss(0, DIVIDER_Y);
        std::wcout << L"-----------------------------------------------------------";
        gotoxy_boss(0, WORD_Y);
        std::wcout << L">> " << currentWord << L" <<";
        gotoxy_boss(0, INPUT_Y);
        std::wcout << L"입력: ";

        // --- 전투 루프: 보스가 쓰러지거나 플레이어가 죽을 때까지 반복 ---
        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            float bossAttackTimer = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastBossAttackTime).count();

            // 체력바, 타이머 등 UI 갱신
            gotoxy_boss(0, BOSS_HP_Y);
            printHpBar(L"BOSS  ", currentBoss.currentHp, currentBoss.maxHp, 40);
            gotoxy_boss(0, PLAYER_HP_Y);
            printHpBar(L"PLAYER", playerCurrentHp, playerMaxHp, 40);
            gotoxy_boss(0, TIMER_Y);
            std::wcout << L"보스 공격까지: " << 10 - static_cast<int>(bossAttackTimer) << L"초 (성공횟수: " << successfulAttacks << L"/4)      ";
            gotoxy_boss(7, INPUT_Y);
            std::wcout << userInput << L" ";

            // 10초가 지나면 보스가 공격
            if (bossAttackTimer >= 10.0f) {
                if (successfulAttacks < 4) {
                    playerCurrentHp -= currentBoss.attackPower; // 플레이어 체력 감소
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"보스의 공격! " << currentBoss.attackPower << L"의 데미지를 입었습니다!        ";
                }
                else {
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"보스의 공격을 막아냈습니다!                      ";
                }
                successfulAttacks = 0; // 성공 횟수 초기화
                lastBossAttackTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gotoxy_boss(0, MESSAGE_Y);
                std::wcout << L"                                                  ";
            }

            // 플레이어가 죽으면 게임 오버
            if (playerCurrentHp <= 0) {
                clearTypingScreen();
                std::wcout << L"\n\n패배했습니다...\n\n";
                goto game_over;
            }

            // 키 입력 처리 (한글, 특수문자 포함)
            if (_kbhit()) {
                wchar_t ch = _getwch(); // 유니코드 문자 입력
                if (ch == L'\r' || ch == L'\n' || ch == L' ') { // 엔터 또는 스페이스바
                    if (userInput == currentWord) {
                        currentBoss.currentHp -= 10; // 보스 체력 감소
                        successfulAttacks++; // 성공 횟수 증가
                        if (currentBoss.currentHp <= 0) break; // 보스 처치 시 루프 탈출

                        // 다음 단어로 교체
                        std::shuffle(allWords.begin(), allWords.end(), g);
                        currentWord = allWords[0];
                        gotoxy_boss(0, WORD_Y);
                        std::wcout << L">> " << currentWord << L" <<" << L"                    ";
                    }
                    // 입력창 지우기
                    gotoxy_boss(7, INPUT_Y);
                    std::wcout << std::wstring(userInput.length() + 1, L' ');
                    userInput = L"";
                }
                else if (ch == L'\b') { // 백스페이스
                    if (!userInput.empty()) userInput.pop_back();
                }
                else if (iswprint(ch)) { // 일반 문자(한글, 영문, 특수문자 등)
                    userInput += ch;
                }
                else if (ch == 27) { // ESC 키로 게임 오버
                    goto game_over;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // CPU 사용량 절감
        }

        // 스테이지 클리어 메시지
        clearTypingScreen();
        std::wcout << L"\n\n축하합니다! STAGE " << stage + 1 << L" 클리어!\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // 모든 스테이지 클리어 메시지
    clearTypingScreen();
    std::wcout << L"\n\n모든 보스를 물리쳤습니다! 당신은 타자의 신입니다!\n\n";

game_over:
    displayGameOverScreen(); // 게임 오버 화면 출력(별도 함수)
    std::wcout << L"\n타자 보스전이 종료되었습니다. 잠시 후 메뉴로 돌아갑니다." << std::endl;
    showCursor(true); // 커서 다시 보이기
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
