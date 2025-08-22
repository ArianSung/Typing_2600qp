#pragma once
// GameManager.h

#pragma once // ��� ������ �ߺ� ������ �����մϴ�.

// Ŭ���� �� �Լ� ���� �ʿ��� �ּ����� ����� �����մϴ�.
#include <iostream>
#include <windows.h>

//=========================================================
// Ŭ���� ����
//=========================================================

// Utility Ŭ����: �ܼ� ȭ�� ����� ���� ���� ����� ����մϴ�.
class Utility {
public:
    // static �Լ��� �ν��Ͻ� ���� 'Utility::gotoxy(x, y)' ���·� �ٷ� ȣ���� �� �ֽ��ϴ�.
    // ������ ����̹Ƿ� ��� ���Ͽ� �ٷ� �����մϴ�.
    static void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
};

// UIManager Ŭ����: �޴� ȭ�� �� UI ����� ����մϴ�.
class UIManager {
public:
    void drawMainMenu(); // �Լ��� ����
    void drawGameMenu();
};

// GameManager Ŭ����: ������ ��ü���� �帧�� �����մϴ�.
class GameManager {
private:
    UIManager uiManager; // UI ���� ��ü�� ����� �����ϴ�.

public:
    void run(); // ���� ���� ���� �Լ��� ����
};


//=========================================================
// ���� �Լ� ����
//=========================================================

void startTypingPractice_Menu();
void startRainGame_Menu();
void displayWinScreen();
void displayGameOverScreen();
void startAiBattleGame_Menu();
void startMemoryChallenge_Menu();
void startTypingBossBattle_Menu();
// Boss_Game.cpp
// Ÿ�� ������ ������ ���� �ҽ� �����Դϴ�.
// �� ������ ���� �ܰ��� ������ ���� ���� �ð� ���� �ܾ �Է��� �����ϴ� ����Դϴ�.

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
#include <cwctype> // �����ڵ� ���� �Է� ó���� ���� ���

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
// ��-Windows ȯ�濡���� ������ �Է� ó���� �ʿ��մϴ�.
#endif

// [��ƿ��Ƽ] UTF-8 ���ڿ��� �����ڵ�(wstring)�� ��ȯ�ϴ� �Լ�
static std::wstring utf8_to_wstring_boss(const std::string& str) {
    if (str.empty()) return std::wstring();
    // UTF-8 ���ڿ��� �����ڵ�(wstring)�� ��ȯ
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [��ƿ��Ƽ] words.txt ������ �о� �����ڵ�(wstring) ���ͷ� ��ȯ�ϴ� �Լ�
static std::vector<std::wstring> loadWordsToWstring_boss(const std::string& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary); // ���̳ʸ� ���� ���� ����
    if (!file.is_open()) return content;
    // ���� ��ü�� �о� string���� ����
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // UTF-8 BOM(���� �� �� 3����Ʈ) ����
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    // UTF-8�� wstring���� ��ȯ
    std::wstring w_contents = utf8_to_wstring_boss(file_contents);
    std::wstringstream wss(w_contents);
    std::wstring line;
    // �� �پ� �о ���Ϳ� ����
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back(); // CR ���� ����
        if (!line.empty()) content.push_back(line);
    }
    return content;
}

// [�ڷᱸ��] ���� ������ �����ϴ� ����ü
struct Boss {
    int maxHp; // ������ �ִ� ü��
    int currentHp; // ������ ���� ü��
    int attackPower; // ������ ���ݷ�(�� ���� ���̴� �÷��̾� HP)
    std::vector<std::wstring> art; // ������ �ƽ�Ű ��Ʈ(�����ڵ�)
};

// [��ƿ��Ƽ] �ܼ� Ŀ�� �������� ����ų� ���̰� �ϴ� �Լ�
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

// [��ƿ��Ƽ] �ܼ� Ŀ���� (x, y) ��ġ�� �̵���Ű�� �Լ�
static void gotoxy_boss(int x, int y) {
#ifdef _WIN32
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

// [��ƿ��Ƽ] �ܼ� ȭ���� ����� �Լ�
static void clearTypingScreen() {
    system("cls");
}

// [��ƿ��Ƽ] ü�� �ٸ� ����ϴ� �Լ� (�����ڵ� wcout ���)
static void printHpBar(const std::wstring& name, int current, int max, int barWidth = 20) {
    std::wcout << name << L" HP: [";
    float hpRatio = (current > 0) ? static_cast<float>(current) / max : 0;
    int filledWidth = static_cast<int>(barWidth * hpRatio);

    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) std::wcout << L"��"; // ü���� ���� �κ�
        else std::wcout << L" "; // ü���� ���� �κ�
    }
    std::wcout << L"] " << current << L"/" << max << L"        ";
}

// [���� �Լ�] Ÿ�� ������ ���� ���� �Լ�
void startTypingBossBattle() {
    setConsoleSize(100, 30); // �ܼ� â ũ�� ����
    showCursor(false); // Ŀ�� �����

    // 1. �ܾ� ��� �ҷ����� (words.txt���� �����ڵ��)
    std::vector<std::wstring> allWords = loadWordsToWstring_boss("words.txt");
    if (allWords.empty()) {
        std::wcerr << L"\n����: words.txt ������ �� �� ���ų� ������ ����ֽ��ϴ�." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        showCursor(true);
        return;
    }

    // 2. 10�ܰ� ���� ������ ���� (�� �������� HP, ���ݷ�, ��Ʈ�� �ٸ�)
    std::vector<Boss> stages(10);
    stages[0] = { 100, 0, 1, {L"       (o.o)       ", L"      /|   |\\      ", L"       /   \\      "} };
    stages[1] = { 120, 0, 1, {L"       (?.?)       ", L"      /|]--[|\\      ", L"       /   \\      "} };
    stages[2] = { 150, 0, 2, {L"      <(?w?)>      ", L"      /| | | |\\      ", L"       /    \\      "} };
    stages[3] = { 200, 0, 2, {L"     --{?w?}--     ", L"      /| | | |\\      ", L"      O-- --O      "} };
    stages[4] = { 250, 0, 3, {L"    \\|/(?��?)\\|/    ", L"      /| | | |\\      ", L"       /\\ /\\       "} };
    stages[5] = { 300, 0, 3, {L"  <(((?(��)?)))>  ", L"      /| | | |\\      ", L"      <|   |>      "} };
    stages[6] = { 380, 0, 4, {L"  ( ( (?��?) ) )  ", L"   ((--/| | | |\\--))   ", L"      <</ \\>>      "} };
    stages[7] = { 450, 0, 5, {L"  (##(?��?)##)  ", L"   ((--/| | | |\\--))   ", L"     ##/   \\##     "} };
    stages[8] = { 550, 0, 6, {L" /MM\\ (?��?) /MM\\ ", L" M ((--/| | | |\\--)) M ", L"     ##/   \\##     "} };
    stages[9] = { 700, 0, 7, {L"/|================|\\", L"| | ((?��?)) | |", L"| |/----\\| |", L"\\| | | |/", L" VVV VVV "} };

    int playerMaxHp = 10; // �÷��̾� �ִ� ü��
    int playerCurrentHp = playerMaxHp; // �÷��̾� ���� ü��

    // 3. �� ��������(����)���� �ݺ�
    for (int stage = 0; stage < 10; ++stage) {
        Boss currentBoss = stages[stage];
        currentBoss.currentHp = currentBoss.maxHp; // ���� ü�� �ʱ�ȭ
        int successfulAttacks = 0; // 10�� ���� ������ ���� Ƚ��

        auto lastBossAttackTime = std::chrono::steady_clock::now();
        std::wstring currentWord; // ���� �Է��ؾ� �� �ܾ�
        std::wstring userInput = L""; // ����ڰ� �Է� ���� ����

        // �ܾ� ����� �����ϰ� ����, ù ��° �ܾ ����
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        currentWord = allWords[0];

        // --- ȭ�� �ʱ�ȭ �� ���� ��Ʈ ��� ---
        clearTypingScreen();
        std::wcout << L"========================= STAGE " << stage + 1 << L" =========================" << std::endl;
        std::wcout << L"\n";
        int art_start_y = 2;
        for (const auto& line : currentBoss.art) {
            gotoxy_boss(0, art_start_y++);
            std::wcout << line << std::endl;
        }

        // ���� UI ��ġ ���
        const int BOSS_HP_Y = art_start_y + 2;
        const int PLAYER_HP_Y = BOSS_HP_Y + 1;
        const int DIVIDER_Y = PLAYER_HP_Y + 2;
        const int TIMER_Y = DIVIDER_Y + 1;
        const int WORD_Y = TIMER_Y + 3;
        const int INPUT_Y = WORD_Y + 2;
        const int MESSAGE_Y = INPUT_Y + 2;

        // UI ���
        gotoxy_boss(0, DIVIDER_Y);
        std::wcout << L"-----------------------------------------------------------";
        gotoxy_boss(0, WORD_Y);
        std::wcout << L">> " << currentWord << L" <<";
        gotoxy_boss(0, INPUT_Y);
        std::wcout << L"�Է�: ";

        // --- ���� ����: ������ �������ų� �÷��̾ ���� ������ �ݺ� ---
        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            float bossAttackTimer = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastBossAttackTime).count();

            // ü�¹�, Ÿ�̸� �� UI ����
            gotoxy_boss(0, BOSS_HP_Y);
            printHpBar(L"BOSS  ", currentBoss.currentHp, currentBoss.maxHp, 40);
            gotoxy_boss(0, PLAYER_HP_Y);
            printHpBar(L"PLAYER", playerCurrentHp, playerMaxHp, 40);
            gotoxy_boss(0, TIMER_Y);
            std::wcout << L"���� ���ݱ���: " << 10 - static_cast<int>(bossAttackTimer) << L"�� (����Ƚ��: " << successfulAttacks << L"/4)      ";
            gotoxy_boss(7, INPUT_Y);
            std::wcout << userInput << L" ";

            // 10�ʰ� ������ ������ ����
            if (bossAttackTimer >= 10.0f) {
                if (successfulAttacks < 4) {
                    playerCurrentHp -= currentBoss.attackPower; // �÷��̾� ü�� ����
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"������ ����! " << currentBoss.attackPower << L"�� �������� �Ծ����ϴ�!        ";
                }
                else {
                    gotoxy_boss(0, MESSAGE_Y);
                    std::wcout << L"������ ������ ���Ƴ½��ϴ�!                      ";
                }
                successfulAttacks = 0; // ���� Ƚ�� �ʱ�ȭ
                lastBossAttackTime = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                gotoxy_boss(0, MESSAGE_Y);
                std::wcout << L"                                                  ";
            }

            // �÷��̾ ������ ���� ����
            if (playerCurrentHp <= 0) {
                clearTypingScreen();
                std::wcout << L"\n\n�й��߽��ϴ�...\n\n";
                goto game_over;
            }

            // Ű �Է� ó�� (�ѱ�, Ư������ ����)
            if (_kbhit()) {
                wchar_t ch = _getwch(); // �����ڵ� ���� �Է�
                if (ch == L'\r' || ch == L'\n' || ch == L' ') { // ���� �Ǵ� �����̽���
                    if (userInput == currentWord) {
                        currentBoss.currentHp -= 10; // ���� ü�� ����
                        successfulAttacks++; // ���� Ƚ�� ����
                        if (currentBoss.currentHp <= 0) break; // ���� óġ �� ���� Ż��

                        // ���� �ܾ�� ��ü
                        std::shuffle(allWords.begin(), allWords.end(), g);
                        currentWord = allWords[0];
                        gotoxy_boss(0, WORD_Y);
                        std::wcout << L">> " << currentWord << L" <<" << L"                    ";
                    }
                    // �Է�â �����
                    gotoxy_boss(7, INPUT_Y);
                    std::wcout << std::wstring(userInput.length() + 1, L' ');
                    userInput = L"";
                }
                else if (ch == L'\b') { // �齺���̽�
                    if (!userInput.empty()) userInput.pop_back();
                }
                else if (iswprint(ch)) { // �Ϲ� ����(�ѱ�, ����, Ư������ ��)
                    userInput += ch;
                }
                else if (ch == 27) { // ESC Ű�� ���� ����
                    goto game_over;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // CPU ��뷮 ����
        }

        // �������� Ŭ���� �޽���
        clearTypingScreen();
        std::wcout << L"\n\n�����մϴ�! STAGE " << stage + 1 << L" Ŭ����!\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // ��� �������� Ŭ���� �޽���
    clearTypingScreen();
    std::wcout << L"\n\n��� ������ �����ƽ��ϴ�! ����� Ÿ���� ���Դϴ�!\n\n";

game_over:
    displayGameOverScreen(); // ���� ���� ȭ�� ���(���� �Լ�)
    std::wcout << L"\nŸ�� �������� ����Ǿ����ϴ�. ��� �� �޴��� ���ư��ϴ�." << std::endl;
    showCursor(true); // Ŀ�� �ٽ� ���̱�
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
