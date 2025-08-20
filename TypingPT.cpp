#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fcntl.h> // for _O_U16TEXT
#include <io.h>    // for _setmode
#include <random>  // for std::shuffle

// �ܼ� ������ ���� Windows ���� ���
#define NOMINMAX
#include <windows.h>
#include <conio.h>

// �ܼ� �ؽ�Ʈ ���� �ڵ�
constexpr int COLOR_DEFAULT = 7;
constexpr int COLOR_GREEN = 10;
constexpr int COLOR_RED = 12;

// --- ��ƿ��Ƽ �Լ� ---

// UTF-8 std::string�� std::wstring���� ��ȯ�ϴ� ���� �Լ�
std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// �ܼ� ���� �Լ����� ���� ���ӽ����̽�
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

// --- �ٽ� ���� ���� ---

class TypingGame {
public:
    void start() {
        SetConsoleTitle(L"Ÿ�� ���� ����");
        bool isRunning = true;
        while (isRunning) {
            displayMainMenu();
            wchar_t choice = _getwch();
            switch (choice) {
            case L'1':
                selectTypingMode();
                break;
            case L'2':
                showComingSoon();
                break;
            case L'3':
                isRunning = false;
                break;
            default:
                showMessage(L"�߸��� �Է��Դϴ�. �ٽ� �õ��� �ּ���.", 1000);
                break;
            }
        }
        showMessage(L"���α׷��� �����մϴ�.", 1000);
    }

private:
    void displayMainMenu() {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);
        Console::showCursor();
        std::wcout << L"==========================\n"
            << L"===   Ÿ�� ���� ����   ===\n"
            << L"==========================\n"
            << L"  1. Ÿ�� ����\n"
            << L"  2. �꼺�� (�غ� ��)\n"
            << L"  3. ����\n"
            << L"--------------------------\n"
            << L"�޴��� �����ϼ���: ";
    }

    void selectTypingMode() {
        bool isSelecting = true;
        while (isSelecting) {
            Console::clearScreen();
            Console::setColor(COLOR_DEFAULT);
            Console::showCursor();
            std::wcout << L"======================\n"
                << L"=== Ÿ�� ���� ��� ===\n"
                << L"======================\n"
                << L"  1. �ܾ� ����\n"
                << L"  2. �� �� ����\n"
                << L"  3. ���� �޴��� ���ư���\n"
                << L"----------------------\n"
                << L"��带 �����ϼ���: ";

            wchar_t choice = _getwch();
            switch (choice) {
            case L'1':
                runPractice(L"words.txt", L"�ܾ� ����");
                break;
            case L'2':
                runPractice(L"long.txt", L"�� �� ����");
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

    void runPractice(const std::wstring& filename, const std::wstring& modeName) {
        std::vector<std::wstring> content = loadContentFromFile(filename);
        if (content.empty()) {
            showMessage(L"'" + filename + L"' ������ �� �� ���ų� ����ֽ��ϴ�.", 2000);
            return;
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(content.begin(), content.end(), g);

        long long totalPromptChars = 0;
        int totalMistakeCount = 0;
        double totalElapsedTime = 0.0;

        for (const auto& text : content) {
            std::wstring userInput = L"";

            Console::clearScreen();
            Console::setColor(COLOR_DEFAULT);
            std::wcout << L"--- " << modeName << L" --- (ESC: �޴��� ���ư���, Enter: �Է� �Ϸ�)\n\n";
            std::wcout << L"���þ�: " << text << L"\n\n";
            std::wcout << L"�Է�  : ";

            auto startTime = std::chrono::high_resolution_clock::now();

            while (true) {
                Console::hideCursor();
                auto now = std::chrono::high_resolution_clock::now();
                double elapsedTime = std::chrono::duration<double>(now - startTime).count();
                displayRealTimeStats(userInput.length(), elapsedTime);
                Console::gotoxy(8 + userInput.length(), 4);
                Console::showCursor();

                if (_kbhit()) {
                    wchar_t ch = _getwch();
                    if (ch == 27) { // ESC Ű
                        Console::showCursor();
                        return;
                    }
                    else if (ch == L'\r') { // Enter Ű
                        break; // while ���� Ż��
                    }
                    else if (ch == L'\b' || ch == 8) { // �齺���̽�
                        if (!userInput.empty()) {
                            userInput.pop_back();

                            Console::hideCursor();
                            Console::gotoxy(8 + userInput.length(), 4);
                            std::wcout << L" ";
                            Console::gotoxy(8 + userInput.length(), 4);
                            Console::showCursor();
                        }
                    }
                    else if (iswprint(ch)) { // �Ϲ� ���� �Է�
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
            }

            auto endTime = std::chrono::high_resolution_clock::now();

            // --- ���� ��Ÿ ��� ---
            int currentMistakes = calculateMistakes(text, userInput);

            totalElapsedTime += std::chrono::duration<double>(endTime - startTime).count();
            totalPromptChars += text.length();
            totalMistakeCount += currentMistakes;
        }

        Console::showCursor();
        displayFinalResults(totalPromptChars, totalMistakeCount, totalElapsedTime);
        std::wcout << L"\n�ƹ� Ű�� ���� �޴��� ���ư�����...";
        _getch();
    }

    // �ǽð� ���â���� '��Ÿ' �׸� ����
    void displayRealTimeStats(int typedChars, double elapsedTime) {
        Console::gotoxy(0, 8);
        Console::setColor(COLOR_DEFAULT);
        int wpm = calculateWPM(typedChars, elapsedTime);
        double accuracy = 100.0 * (static_cast<double>(typedChars) / (typedChars + 1)); // �ӽ� ��Ȯ��

        wprintf(L"====================================================\n");
        wprintf(L"  Ÿ��   : %-5d Ÿ/��\n", wpm);
        wprintf(L"  ��Ȯ�� : --.---%%\n"); // �ǽð� ��Ȯ�� ����� �����Ƿ� ǥ������ ����
        wprintf(L"====================================================\n");
    }

    void displayFinalResults(long long totalPromptChars, int totalMistakes, double totalTime) {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);

        long long correctChars = totalPromptChars > totalMistakes ? totalPromptChars - totalMistakes : 0;
        int finalWpm = calculateWPM(correctChars, totalTime);
        double finalAccuracy = calculateAccuracy(totalPromptChars, totalMistakes);

        std::wcout << L"=== ���� �Ϸ� ===\n\n";
        wprintf(L"�� �ð�      : %.2f ��\n", totalTime);
        wprintf(L"��� Ÿ��    : %d Ÿ/��\n", finalWpm);
        wprintf(L"��Ȯ��       : %.2f%%\n", finalAccuracy);
        wprintf(L"�� ��Ÿ ��   : %d ��\n", totalMistakes);
    }

    std::vector<std::wstring> loadContentFromFile(const std::wstring& filename) {
        std::vector<std::wstring> content;
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    content.push_back(utf8_to_wstring(line));
                }
            }
            file.close();
        }
        return content;
    }

    // Levenshtein �Ÿ� �˰����� ����� �� ���ڿ��� ����(��Ÿ ��)�� ���
    int calculateMistakes(const std::wstring& s1, const std::wstring& s2) {
        const size_t len1 = s1.size(), len2 = s2.size();
        std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

        d[0][0] = 0;
        for (size_t i = 1; i <= len1; ++i) d[i][0] = i;
        for (size_t i = 1; i <= len2; ++i) d[0][i] = i;

        for (size_t i = 1; i <= len1; ++i)
            for (size_t j = 1; j <= len2; ++j)
                d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });

        return d[len1][len2];
    }

    int calculateWPM(long long charactersTyped, double timeInSeconds) {
        if (timeInSeconds <= 0 || charactersTyped <= 0) return 0;
        return static_cast<int>((static_cast<double>(charactersTyped) / 5.0) / (timeInSeconds / 60.0));
    }

    double calculateAccuracy(long long totalCharacters, int mistakes) {
        if (totalCharacters <= 0) return 0.0;
        double correctChars = totalCharacters > mistakes ? totalCharacters - mistakes : 0;
        double accuracy = 100.0 * (correctChars / totalCharacters);
        return std::max(0.0, accuracy);
    }

    void showMessage(const std::wstring& message, int milliseconds) {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);
        Console::showCursor();
        std::wcout << message << std::endl;
        Sleep(milliseconds);
    }

    void showComingSoon() {
        showMessage(L"�� ����� �� �߰��� �����Դϴ�!", 2000);
    }
};

// ========================================================================================
// ���� �Լ�
// ========================================================================================
int wmain() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    TypingGame game;
    game.start();
    return 0;
}
