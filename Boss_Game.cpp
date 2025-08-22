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
#include <cwctype> // [수정] isprint -> iswprint

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
// ... (비-Windows 코드는 생략)
#endif

// [추가] UTF-8 변환 및 파일 로딩 헬퍼 함수
static std::wstring utf8_to_wstring_boss(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

static std::vector<std::wstring> loadWordsToWstring_boss(const std::string& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return content;
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    std::wstring w_contents = utf8_to_wstring_boss(file_contents);
    std::wstringstream wss(w_contents);
    std::wstring line;
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty()) content.push_back(line);
    }
    return content;
}


// --- 게임 데이터 구조 ---
struct Boss {
    int maxHp;
    int currentHp;
    int attackPower;
    std::vector<std::wstring> art; // [수정] string -> wstring
};

// --- 헬퍼 함수 ---
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

static void gotoxy_boss(int x, int y) { // 이름 충돌 방지를 위해 gotoxy_boss로 변경
#ifdef _WIN32
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

static void clearTypingScreen() {
    system("cls");
}

// [수정] wcout 사용
static void printHpBar(const std::wstring& name, int current, int max, int barWidth = 20) {
    std::wcout << name << L" HP: [";
    float hpRatio = (current > 0) ? static_cast<float>(current) / max : 0;
    int filledWidth = static_cast<int>(barWidth * hpRatio);

    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) std::wcout << L"■";
        else std::wcout << L" ";
    }
    std::wcout << L"] " << current << L"/" << max << L"        ";
}

// --- 게임 메인 함수 ---
void startTypingBossBattle() {
    setConsoleSize(100, 30);
    showCursor(false);

    // [수정] 유니코드 파일 로딩
    std::vector<std::wstring> allWords = loadWordsToWstring_boss("words.txt");
    if (allWords.empty()) {
        std::wcerr << L"\n오류: words.txt 파일을 열 수 없거나 내용이 비어있습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        showCursor(true);
        return;
    }

    // [수정] 보스 아트를 L"..." 로 변경
    std::vector<Boss> stages(10);
    stages[0] = { 100, 0, 1, {L"       (o.o)       ", L"      /|   |\\      ", L"       /   \\      "} };
    stages[1] = { 120, 0, 1, {L"       (Ò.Ó)       ", L"      /|]--[|\\      ", L"       /   \\      "} };
    stages[2] = { 150, 0, 2, {L"      <(ÒwÓ)>      ", L"      /| | | |\\      ", L"       /    \\      "} };
    stages[3] = { 200, 0, 2, {L"     --{ÒwÓ}--     ", L"      /| | | |\\      ", L"      O-- --O      "} };
    stages[4] = { 250, 0, 3, {L"    \\|/(ÒДÓ)\\|/    ", L"      /| | | |\\      ", L"       /\\ /\\       "} };
    stages[5] = { 300, 0, 3, {L"  <(((Ò(エ)Ó)))>  ", L"      /| | | |\\      ", L"      <|   |>      "} };
    stages[6] = { 380, 0, 4, {L"  ( ( (Ò益Ó) ) )  ", L"   ((--/| | | |\\--))   ", L"      <</ \\>>      "} };
    stages[7] = { 450, 0, 5, {L"  (##(Ò益Ó)##)  ", L"   ((--/| | | |\\--))   ", L"     ##/   \\##     "} };
    stages[8] = { 550, 0, 6, {L" /MM\\ (Ò益Ó) /MM\\ ", L" M ((--/| | | |\\--)) M ", L"     ##/   \\##     "} };
    stages[9] = { 700, 0, 7, {L"/|================|\\", L"| | ((Ò益Ó)) | |", L"| |/----\\| |", L"\\| | | |/", L" VVV VVV "} };

    int playerMaxHp = 10;
    int playerCurrentHp = playerMaxHp;

    for (int stage = 0; stage < 10; ++stage) {
        Boss currentBoss = stages[stage];
        currentBoss.currentHp = currentBoss.maxHp;
        int successfulAttacks = 0;

        auto lastBossAttackTime = std::chrono::steady_clock::now();
        std::wstring currentWord; // [수정] string -> wstring
        std::wstring userInput = L""; // [수정] string -> wstring

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        currentWord = allWords[0];

        clearTypingScreen();
        std::wcout << L"========================= STAGE " << stage + 1 << L" =========================" << std::endl;
        std::wcout << L"\n";
        int art_start_y = 2;
        for (const auto& line : currentBoss.art) {
            gotoxy_boss(0, art_start_y++);
            std::wcout << line << std::endl;
        }

        const int BOSS_HP_Y = art_start_y + 2;
        const int PLAYER_HP_Y = BOSS_HP_Y + 1;
        const int DIVIDER_Y = PLAYER_HP_Y + 2;
        const int TIMER_Y = DIVIDER_Y + 1;
        const int WORD_Y = TIMER_Y + 3;
        const int INPUT_Y = WORD_Y + 2;
        const int MESSAGE_Y = INPUT_Y + 2;

        gotoxy_boss(0, DIVIDER_Y);
        std::wcout << L"-----------------------------------------------------------";
        gotoxy_boss(0, WORD_Y);
        std::wcout << L">> " << currentWord << L" <<";
        gotoxy_boss(0, INPUT_Y);
        std::wcout << L"입력: ";

        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            float bossAttackTimer = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastBossAttackTime).count();

            gotoxy_boss(0, BOSS_HP_Y);
            printHpBar(L"BOSS  ", currentBoss.currentHp, currentBoss.maxHp, 40);
            gotoxy_boss(0, PLAYER_HP_Y);
            printHpBar(L"PLAYER", playerCurrentHp, playerMaxHp, 40);
            gotoxy_boss(0, TIMER_Y);
            std::wcout << L"보스 공격까지: " << 10 - static_cast<int>(bossAttackTimer) << L"초 (성공횟수: " << successfulAttacks << L"/4)      ";
            gotoxy_boss(7, INPUT_Y);
            std::wcout << userInput << L" ";

            if (bossAttackTimer >= 10.0f) {
                if (successfulAttacks < 4) {
                    playerCurrentHp -= currentBoss.attackPower;
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"보스의 공격! " << currentBoss.attackPower << L"의 데미지를 입었습니다!        ";
                }
                else {
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"보스의 공격을 막아냈습니다!                      ";
                }
                successfulAttacks = 0;
                lastBossAttackTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gotoxy_boss(0, MESSAGE_Y);
                std::wcout << L"                                                  ";
            }

            if (playerCurrentHp <= 0) {
                clearTypingScreen();
                std::wcout << L"\n\n패배했습니다...\n\n";
                goto game_over;
            }

            if (_kbhit()) {
                wchar_t ch = _getwch(); // [수정] _getch -> _getwch
                if (ch == L'\r' || ch == L'\n' || ch == L' ') {
                    if (userInput == currentWord) {
                        currentBoss.currentHp -= 10;
                        successfulAttacks++;
                        if (currentBoss.currentHp <= 0) break;

                        std::shuffle(allWords.begin(), allWords.end(), g);
                        currentWord = allWords[0];
                        gotoxy_boss(0, WORD_Y);
                        std::wcout << L">> " << currentWord << L" <<" << L"                    ";
                    }
                    gotoxy_boss(7, INPUT_Y);
                    std::wcout << std::wstring(userInput.length() + 1, L' ');
                    userInput = L"";
                }
                else if (ch == L'\b') {
                    if (!userInput.empty()) userInput.pop_back();
                }
                else if (iswprint(ch)) { // [수정] isprint -> iswprint
                    userInput += ch;
                }
                else if (ch == 27) {
                    goto game_over;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        clearTypingScreen();
        std::wcout << L"\n\n축하합니다! STAGE " << stage + 1 << L" 클리어!\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    clearTypingScreen();
    std::wcout << L"\n\n모든 보스를 물리쳤습니다! 당신은 타자의 신입니다!\n\n";

game_over:
    displayGameOverScreen();
    std::wcout << L"\n타자 보스전이 종료되었습니다. 잠시 후 메뉴로 돌아갑니다." << std::endl;
    showCursor(true);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
