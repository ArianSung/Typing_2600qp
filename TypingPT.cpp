#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdio>

// Windows 전용 헤더
#define NOMINMAX
#include <windows.h>
#include <conio.h>

// ========================================================================================
// 전역 상수 및 클래스/네임스페이스 선언부
// ========================================================================================

// 콘솔 텍스트 색상 코드 정의
#define COLOR_DEFAULT 7
#define COLOR_GREEN 10
#define COLOR_RED 12

// 네임스페이스 선언
namespace ConsoleUtils {
    void gotoxy(int x, int y);
    void setColor(int color);
}

// 클래스 선언
class Utility {
public:
    static void clearScreen();
    static std::vector<std::string> loadContentFromFile(const std::string& filename);
};

class TypingPractice {
public:
    void run(const std::vector<std::string>& words, const std::string& modeName);
private:
    int calculateWPM(int charactersTyped, double timeInSeconds);
    double calculateAccuracy(int totalCharacters, int mistakes);
};

class GameManager {
public:
    void start();
private:
    void displayMainMenu();
    void selectTypingMode();
    void exitGame();
    TypingPractice typingPractice;
};


// ========================================================================================
// 구현부
// ========================================================================================

// --- 유틸리티 함수 및 클래스 구현 ---

void ConsoleUtils::gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void ConsoleUtils::setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Utility::clearScreen() {
    system("cls");
}

std::vector<std::string> Utility::loadContentFromFile(const std::string& filename) {
    std::vector<std::string> content;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                content.push_back(line);
            }
        }
        file.close();
    }
    else {
        ConsoleUtils::gotoxy(0, 5);
        std::cout << "'" << filename << "' 파일을 열 수 없습니다. 확인해주세요." << std::endl;
        Sleep(2000);
    }
    return content;
}

// --- 타자 연습 클래스 구현 ---

void displayRealTimeStats(int wpm, double accuracy, int mistakes) {
    ConsoleUtils::gotoxy(0, 10);
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "====================================================" << std::endl;
    std::cout << "  타수: " << wpm << " 타/분        " << std::endl;
    std::cout << "  정확도: ";
    printf("%.2f%%", accuracy);
    std::cout << "           " << std::endl;
    std::cout << "  오타: " << mistakes << " 개          " << std::endl;
    std::cout << "====================================================" << std::endl;
}

void TypingPractice::run(const std::vector<std::string>& content, const std::string& modeName) {
    if (content.empty()) {
        return;
    }

    int totalTypedCount = 0;
    int totalMistakeCount = 0;
    double totalElapsedTime = 0.0;

    for (const auto& text : content) {
        std::string userInput = "";
        int currentMistakes = 0;

        Utility::clearScreen();
        ConsoleUtils::gotoxy(0, 0);
        ConsoleUtils::setColor(COLOR_DEFAULT);
        std::cout << "--- " << modeName << " --- (ESC: 메인 메뉴로)" << std::endl;
        ConsoleUtils::gotoxy(0, 2);
        std::cout << "제시어: " << text << std::endl;
        ConsoleUtils::gotoxy(0, 4);
        std::cout << "입력  : ";

        auto startTime = std::chrono::high_resolution_clock::now();

        while (userInput.length() < text.length()) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = now - startTime;
            double elapsedTime = duration.count();
            int wpm = calculateWPM(userInput.length() - currentMistakes, elapsedTime);
            double accuracy = calculateAccuracy(userInput.length(), currentMistakes);
            displayRealTimeStats(wpm, accuracy, currentMistakes);

            ConsoleUtils::gotoxy(8 + userInput.length(), 4);

            if (_kbhit()) {
                char ch = _getch();

                if (ch == 27) { // ESC
                    ConsoleUtils::setColor(COLOR_DEFAULT);
                    return;
                }
                else if (ch == '\b' || ch == 8) { // Backspace
                    if (!userInput.empty()) {
                        if (userInput.back() != text[userInput.length() - 1]) {
                            currentMistakes--;
                        }
                        userInput.pop_back();
                        ConsoleUtils::gotoxy(8 + userInput.length(), 4);
                        std::cout << " ";
                    }
                }
                else if (isprint(ch)) {
                    if (ch == text[userInput.length()]) {
                        ConsoleUtils::setColor(COLOR_GREEN);
                    }
                    else {
                        ConsoleUtils::setColor(COLOR_RED);
                        currentMistakes++;
                    }
                    std::cout << ch;
                    userInput += ch;
                }
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        totalElapsedTime += duration.count();
        totalTypedCount += userInput.length();
        totalMistakeCount += currentMistakes;
    }

    Utility::clearScreen();
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "=== 타자 연습 종료 ===" << std::endl << std::endl;
    printf("총 걸린 시간: %.2f초\n", totalElapsedTime);
    int finalWpm = calculateWPM(totalTypedCount - totalMistakeCount, totalElapsedTime);
    std::cout << "평균 타수: " << finalWpm << "타/분" << std::endl;
    double finalAccuracy = calculateAccuracy(totalTypedCount, totalMistakeCount);
    printf("정확도: %.2f%%\n", finalAccuracy);
    std::cout << "총 오타 수: " << totalMistakeCount << std::endl;

    std::cout << "\n메인 메뉴로 돌아가려면 아무 키나 누르세요..." << std::endl;
    _getch();
}

int TypingPractice::calculateWPM(int charactersTyped, double timeInSeconds) {
    if (timeInSeconds <= 0 || charactersTyped <= 0) return 0;
    return static_cast<int>((static_cast<double>(charactersTyped) / 5.0) / (timeInSeconds / 60.0));
}

double TypingPractice::calculateAccuracy(int totalCharacters, int mistakes) {
    if (totalCharacters <= 0) return 100.0;
    double accuracy = 100.0 * (static_cast<double>(totalCharacters - mistakes) / totalCharacters);
    return std::max(0.0, accuracy);
}

// --- 게임 매니저 클래스 구현 ---

void GameManager::start() {
    bool running = true;
    while (running) {
        displayMainMenu();
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case '1':
            selectTypingMode();
            break;
        case '2':
            Utility::clearScreen();
            ConsoleUtils::setColor(COLOR_DEFAULT);
            std::cout << "산성비 게임은 아직 준비 중입니다." << std::endl;
            Sleep(2000);
            break;
        case '3':
            exitGame();
            running = false;
            break;
        default:
            Utility::clearScreen();
            ConsoleUtils::setColor(COLOR_DEFAULT);
            std::cout << "잘못된 입력입니다. 다시 시도해 주세요." << std::endl;
            Sleep(1000);
            break;
        }
    }
}

void GameManager::displayMainMenu() {
    Utility::clearScreen();
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "==========================" << std::endl;
    std::cout << "=== 타자 연습 프로그램 ===" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "  1. 타자 연습" << std::endl;
    std::cout << "  2. 산성비 게임 (준비 중)" << std::endl;
    std::cout << "  3. 종료" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << "메뉴를 선택하세요: ";
}

void GameManager::selectTypingMode() {
    bool running = true;
    while (running) {
        Utility::clearScreen();
        ConsoleUtils::setColor(COLOR_DEFAULT);
        std::cout << "======================" << std::endl;
        std::cout << "=== 타자 연습 모드 ===" << std::endl;
        std::cout << "======================" << std::endl;
        std::cout << "  1. 단어 연습" << std::endl;
        std::cout << "  2. 긴 글 연습" << std::endl;
        std::cout << "  3. 뒤로 가기" << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << "모드를 선택하세요: ";

        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == '1') {
            std::vector<std::string> words = Utility::loadContentFromFile("words.txt");
            typingPractice.run(words, "단어");
        }
        else if (choice == '2') {
            std::vector<std::string> longTexts = Utility::loadContentFromFile("long.txt");
            typingPractice.run(longTexts, "긴 글");
        }
        else if (choice == '3') {
            running = false;
        }
        else {
            Utility::clearScreen();
            ConsoleUtils::setColor(COLOR_DEFAULT);
            std::cout << "잘못된 입력입니다. 다시 시도해 주세요." << std::endl;
            Sleep(1000);
        }
    }
}

void GameManager::exitGame() {
    Utility::clearScreen();
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "프로그램을 종료합니다." << std::endl;
    Sleep(1000);
}

// ========================================================================================
// 메인 함수
// ========================================================================================
int main() {
    // 콘솔 창 제목 설정
    SetConsoleTitle(L"Typing Practice Game");
    GameManager manager;
    manager.start();
    return 0;
}