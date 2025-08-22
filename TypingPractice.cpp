#include "TypingPractice.h" // ��� ���� ��� ������ �����մϴ�.

#include <iostream>     // std::wcout ����� ���� �߰�
#include <string>       // std::wstring ����� ���� �߰�
#include <vector>       // std::vector ����� ���� �߰�
#include <sstream>      // std::wstringstream ����� ���� �߰�
#include <cwctype>      // iswprint ����� ���� �߰�
#include <chrono>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <random>

// Windows-specific headers for console manipulation
#define NOMINMAX
#include <windows.h>
#include <conio.h>

// --- ���ο����� ����� �Լ� �� ���ӽ����̽� ���� ---

// �� �Լ����� �� cpp ���� �ȿ����� ���˴ϴ�.
static void runPracticeSession(const std::wstring& filename, const std::wstring& modeName);
static void displayRealTimeStats(int kpm, double accuracy);
static void displayFinalResults(long long totalTypedChars, int incorrectRounds, int totalRounds, double totalTime);
static std::vector<std::wstring> loadContentFromFile(const std::wstring& filename);
static int calculateKPM(long long charactersTyped, double timeInSeconds);
static void showMessage(const std::wstring& message, int milliseconds);
static std::wstring utf8_to_wstring(const std::string& str);


// Console text color codes
constexpr int COLOR_DEFAULT = 7;
constexpr int COLOR_GREEN = 10;
constexpr int COLOR_RED = 12;

// Namespace for console-related functions
namespace Console {
    void gotoxy(int x, int y) {
        COORD pos = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
    void setColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }
    void clearScreen() {
        system("cls");
    }
    void hideCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }
    void showCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }
}


// --- �ܺο� ������ ������ �Լ� ---

void startTypingPractice() {
    bool isSelecting = true;
    while (isSelecting) {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);
        Console::showCursor();
        std::wcout << L"==========================\n"
            << L"===   Ÿ�� ���� ���   ===\n"
            << L"==========================\n"
            << L"  1. �ܾ� ����\n"
            << L"  2. �� �� ����\n"
            << L"  3. ���� �޴��� ���ư���\n"
            << L"--------------------------\n"
            << L"��带 �����ϼ���: ";

        wchar_t choice = _getwch();
        switch (choice) {
        case L'1':
            runPracticeSession(L"words.txt", L"�ܾ� ����");
            break;
        case L'2':
            runPracticeSession(L"long.txt", L"�� �� ����");
            break;
        case L'3':
            isSelecting = false;
            break;
        default:
            showMessage(L"�߸��� �Է��Դϴ�. �ٽ� �õ��� �ּ���.", 1000);
            break;
        }
    }
}


// --- ���� �Լ����� ���� ���� ---

static void runPracticeSession(const std::wstring& filename, const std::wstring& modeName) {
    std::vector<std::wstring> content = loadContentFromFile(filename);
    if (content.empty()) {
        showMessage(L"'" + filename + L"' ������ �� �� ���ų� ������ ����ֽ��ϴ�.", 2000);
        return;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(content.begin(), content.end(), g);

    long long totalTypedChars_completed = 0;
    int totalRounds = 0;
    int correctRounds = 0;

    auto practiceStartTime = std::chrono::high_resolution_clock::now();

    for (const auto& text : content) {
        std::wstring userInput = L"";

        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);
        std::wcout << L"--- " << modeName << L" --- (ESC: �޴��� ���ư���, Enter: �Է� �Ϸ�)\n\n";
        std::wcout << L"���þ�: " << text << L"\n\n";
        std::wcout << L"�Է�   : ";

        double currentSessionAccuracy = (totalRounds == 0) ? 100.0 : ((double)correctRounds / totalRounds * 100.0);

        while (true) {
            Console::hideCursor();
            auto now = std::chrono::high_resolution_clock::now();
            double cumulativeElapsedTime = std::chrono::duration<double>(now - practiceStartTime).count();
            long long cumulativeTypedChars = totalTypedChars_completed + userInput.length();
            int currentKPM = calculateKPM(cumulativeTypedChars, cumulativeElapsedTime);

            displayRealTimeStats(currentKPM, currentSessionAccuracy);

            Console::gotoxy(8 + userInput.length(), 4);
            Console::showCursor();

            if (_kbhit()) {
                wchar_t ch = _getwch();
                if (ch == 27) { // ESC key
                    Console::showCursor();
                    return;
                }
                else if (ch == L'\r' || ch == L'\n') {
                    break;
                }
                else if (ch == L'\b' || ch == 8) {
                    if (!userInput.empty()) {
                        userInput.pop_back();
                        Console::hideCursor();
                        Console::gotoxy(8 + userInput.length(), 4);
                        std::wcout << L" ";
                        Console::gotoxy(8 + userInput.length(), 4);
                        Console::showCursor();
                    }
                }
                else if (iswprint(ch)) {
                    size_t current_idx = userInput.length();
                    if (current_idx < text.length() && ch == text[current_idx]) {
                        Console::setColor(COLOR_GREEN);
                    }
                    else {
                        Console::setColor(COLOR_RED);
                    }
                    std::wcout << ch;
                    userInput += ch;
                }
            }
            Sleep(50);
        }

        totalRounds++;
        if (userInput == text) {
            correctRounds++;
        }
        totalTypedChars_completed += userInput.length();
    }

    Console::showCursor();
    auto practiceEndTime = std::chrono::high_resolution_clock::now();
    double finalTotalTime = std::chrono::duration<double>(practiceEndTime - practiceStartTime).count();
    int incorrectRounds = totalRounds - correctRounds;

    displayFinalResults(totalTypedChars_completed, incorrectRounds, totalRounds, finalTotalTime);
    std::wcout << L"\n�ƹ� Ű�� ���� �޴��� ���ư�����...";
    _getch();
}

static void displayRealTimeStats(int kpm, double accuracy) {
    Console::gotoxy(0, 8);
    Console::setColor(COLOR_DEFAULT);
    wprintf(L"====================================================\n");
    wprintf(L"  �д� Ÿ��(KPM): %-5d\n", kpm);
    wprintf(L"  ��Ȯ��        : %6.2f %%\n", accuracy);
    wprintf(L"====================================================\n");
}

static void displayFinalResults(long long totalTypedChars, int incorrectRounds, int totalRounds, double totalTime) {
    Console::clearScreen();
    Console::setColor(COLOR_DEFAULT);
    int finalKpm = calculateKPM(totalTypedChars, totalTime);
    int correctRounds = totalRounds - incorrectRounds;
    double finalAccuracy = (totalRounds == 0) ? 0.0 : ((double)correctRounds / totalRounds * 100.0);
    std::wcout << L"===================\n";
    std::wcout << L"===  ���� �Ϸ�  ===\n";
    std::wcout << L"===================\n\n";
    wprintf(L"  �� �ð�        : %.2f ��\n", totalTime);
    wprintf(L"  ��� Ÿ��(KPM) : %d\n", finalKpm);
    wprintf(L"  ���� ��Ȯ��    : %.2f %%\n", finalAccuracy);
    wprintf(L"  Ʋ�� ����      : %d ��\n", incorrectRounds);
}

// Helper function to convert a UTF-8 std::string to std::wstring
static std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// [���� ����] ������ ���������� �б� ���� ���ο� �Լ� ����
static std::vector<std::wstring> loadContentFromFile(const std::wstring& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary); // ������ ���̳ʸ� ���� ���ϴ�.

    if (!file.is_open()) {
        return content; // ���� ���� ����
    }

    // ���� ��ü�� std::string���� �н��ϴ�.
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (file_contents.empty()) {
        return content;
    }

    // UTF-8 BOM(Byte Order Mark) Ȯ�� �� ����
    if (file_contents.size() >= 3 &&
        (unsigned char)file_contents[0] == 0xEF &&
        (unsigned char)file_contents[1] == 0xBB &&
        (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }

    // UTF-8 ���ڿ��� ���̵� ���ڿ�(wstring)���� ��ȯ
    std::wstring w_contents = utf8_to_wstring(file_contents);
    if (w_contents.empty()) {
        return content;
    }

    // ��ȯ�� ���̵� ���ڿ��� �� ������ �����ϴ�.
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


static int calculateKPM(long long charactersTyped, double timeInSeconds) {
    if (timeInSeconds <= 0 || charactersTyped <= 0) return 0;
    double minutes = timeInSeconds / 60.0;
    return static_cast<int>(charactersTyped / minutes);
}

static void showMessage(const std::wstring& message, int milliseconds) {
    Console::clearScreen();
    Console::setColor(COLOR_DEFAULT);
    Console::showCursor();
    std::wcout << message << std::endl;
    Sleep(milliseconds);
}
