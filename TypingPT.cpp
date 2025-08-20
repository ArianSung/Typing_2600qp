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

// 콘솔 조작을 위한 Windows 전용 헤더
#define NOMINMAX
#include <windows.h>
#include <conio.h>

// 콘솔 텍스트 색상 코드
constexpr int COLOR_DEFAULT = 7;
constexpr int COLOR_GREEN = 10;
constexpr int COLOR_RED = 12;

// --- 유틸리티 함수 ---

// UTF-8 std::string을 std::wstring으로 변환하는 헬퍼 함수
std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// 콘솔 관련 함수들을 위한 네임스페이스
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

// --- 핵심 게임 로직 ---

class TypingGame {
public:
    void start() {
        SetConsoleTitle(L"타자 연습 게임");
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
                showMessage(L"잘못된 입력입니다. 다시 시도해 주세요.", 1000);
                break;
            }
        }
        showMessage(L"프로그램을 종료합니다.", 1000);
    }

private:
    void displayMainMenu() {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);
        Console::showCursor();
        std::wcout << L"==========================\n"
            << L"===   타자 연습 게임   ===\n"
            << L"==========================\n"
            << L"  1. 타자 연습\n"
            << L"  2. 산성비 (준비 중)\n"
            << L"  3. 종료\n"
            << L"--------------------------\n"
            << L"메뉴를 선택하세요: ";
    }

    void selectTypingMode() {
        bool isSelecting = true;
        while (isSelecting) {
            Console::clearScreen();
            Console::setColor(COLOR_DEFAULT);
            Console::showCursor();
            std::wcout << L"======================\n"
                << L"=== 타자 연습 모드 ===\n"
                << L"======================\n"
                << L"  1. 단어 연습\n"
                << L"  2. 긴 글 연습\n"
                << L"  3. 메인 메뉴로 돌아가기\n"
                << L"----------------------\n"
                << L"모드를 선택하세요: ";

            wchar_t choice = _getwch();
            switch (choice) {
            case L'1':
                runPractice(L"words.txt", L"단어 연습");
                break;
            case L'2':
                runPractice(L"long.txt", L"긴 글 연습");
                break;
            case L'3':
                isSelecting = false;
                break;
            default:
                showMessage(L"잘못된 입력입니다. 다시 시도해 주세요.", 1000);
                break;
            }
        }
    }

    void runPractice(const std::wstring& filename, const std::wstring& modeName) {
        std::vector<std::wstring> content = loadContentFromFile(filename);
        if (content.empty()) {
            showMessage(L"'" + filename + L"' 파일을 열 수 없거나 비어있습니다.", 2000);
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
            std::wcout << L"--- " << modeName << L" --- (ESC: 메뉴로 돌아가기, Enter: 입력 완료)\n\n";
            std::wcout << L"제시어: " << text << L"\n\n";
            std::wcout << L"입력  : ";

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
                    if (ch == 27) { // ESC 키
                        Console::showCursor();
                        return;
                    }
                    else if (ch == L'\r') { // Enter 키
                        break; // while 루프 탈출
                    }
                    else if (ch == L'\b' || ch == 8) { // 백스페이스
                        if (!userInput.empty()) {
                            userInput.pop_back();

                            Console::hideCursor();
                            Console::gotoxy(8 + userInput.length(), 4);
                            std::wcout << L" ";
                            Console::gotoxy(8 + userInput.length(), 4);
                            Console::showCursor();
                        }
                    }
                    else if (iswprint(ch)) { // 일반 문자 입력
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

            // --- 최종 오타 계산 ---
            int currentMistakes = calculateMistakes(text, userInput);

            totalElapsedTime += std::chrono::duration<double>(endTime - startTime).count();
            totalPromptChars += text.length();
            totalMistakeCount += currentMistakes;
        }

        Console::showCursor();
        displayFinalResults(totalPromptChars, totalMistakeCount, totalElapsedTime);
        std::wcout << L"\n아무 키나 눌러 메뉴로 돌아가세요...";
        _getch();
    }

    // 실시간 통계창에서 '오타' 항목 제거
    void displayRealTimeStats(int typedChars, double elapsedTime) {
        Console::gotoxy(0, 8);
        Console::setColor(COLOR_DEFAULT);
        int wpm = calculateWPM(typedChars, elapsedTime);
        double accuracy = 100.0 * (static_cast<double>(typedChars) / (typedChars + 1)); // 임시 정확도

        wprintf(L"====================================================\n");
        wprintf(L"  타수   : %-5d 타/분\n", wpm);
        wprintf(L"  정확도 : --.---%%\n"); // 실시간 정확도 계산이 어려우므로 표시하지 않음
        wprintf(L"====================================================\n");
    }

    void displayFinalResults(long long totalPromptChars, int totalMistakes, double totalTime) {
        Console::clearScreen();
        Console::setColor(COLOR_DEFAULT);

        long long correctChars = totalPromptChars > totalMistakes ? totalPromptChars - totalMistakes : 0;
        int finalWpm = calculateWPM(correctChars, totalTime);
        double finalAccuracy = calculateAccuracy(totalPromptChars, totalMistakes);

        std::wcout << L"=== 연습 완료 ===\n\n";
        wprintf(L"총 시간      : %.2f 초\n", totalTime);
        wprintf(L"평균 타수    : %d 타/분\n", finalWpm);
        wprintf(L"정확도       : %.2f%%\n", finalAccuracy);
        wprintf(L"총 오타 수   : %d 개\n", totalMistakes);
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

    // Levenshtein 거리 알고리즘을 사용해 두 문자열의 차이(오타 수)를 계산
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
        showMessage(L"이 기능은 곧 추가될 예정입니다!", 2000);
    }
};

// ========================================================================================
// 메인 함수
// ========================================================================================
int wmain() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    TypingGame game;
    game.start();
    return 0;
}
