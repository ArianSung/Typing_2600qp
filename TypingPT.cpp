#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdio>

// Windows ���� ���
#define NOMINMAX
#include <windows.h>
#include <conio.h>

// ========================================================================================
// ���� ��� �� Ŭ����/���ӽ����̽� �����
// ========================================================================================

// �ܼ� �ؽ�Ʈ ���� �ڵ� ����
#define COLOR_DEFAULT 7
#define COLOR_GREEN 10
#define COLOR_RED 12

// ���ӽ����̽� ����
namespace ConsoleUtils {
    void gotoxy(int x, int y);
    void setColor(int color);
}

// Ŭ���� ����
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
// ������
// ========================================================================================

// --- ��ƿ��Ƽ �Լ� �� Ŭ���� ���� ---

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
        std::cout << "'" << filename << "' ������ �� �� �����ϴ�. Ȯ�����ּ���." << std::endl;
        Sleep(2000);
    }
    return content;
}

// --- Ÿ�� ���� Ŭ���� ���� ---

void displayRealTimeStats(int wpm, double accuracy, int mistakes) {
    ConsoleUtils::gotoxy(0, 10);
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "====================================================" << std::endl;
    std::cout << "  Ÿ��: " << wpm << " Ÿ/��        " << std::endl;
    std::cout << "  ��Ȯ��: ";
    printf("%.2f%%", accuracy);
    std::cout << "           " << std::endl;
    std::cout << "  ��Ÿ: " << mistakes << " ��          " << std::endl;
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
        std::cout << "--- " << modeName << " --- (ESC: ���� �޴���)" << std::endl;
        ConsoleUtils::gotoxy(0, 2);
        std::cout << "���þ�: " << text << std::endl;
        ConsoleUtils::gotoxy(0, 4);
        std::cout << "�Է�  : ";

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
    std::cout << "=== Ÿ�� ���� ���� ===" << std::endl << std::endl;
    printf("�� �ɸ� �ð�: %.2f��\n", totalElapsedTime);
    int finalWpm = calculateWPM(totalTypedCount - totalMistakeCount, totalElapsedTime);
    std::cout << "��� Ÿ��: " << finalWpm << "Ÿ/��" << std::endl;
    double finalAccuracy = calculateAccuracy(totalTypedCount, totalMistakeCount);
    printf("��Ȯ��: %.2f%%\n", finalAccuracy);
    std::cout << "�� ��Ÿ ��: " << totalMistakeCount << std::endl;

    std::cout << "\n���� �޴��� ���ư����� �ƹ� Ű�� ��������..." << std::endl;
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

// --- ���� �Ŵ��� Ŭ���� ���� ---

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
            std::cout << "�꼺�� ������ ���� �غ� ���Դϴ�." << std::endl;
            Sleep(2000);
            break;
        case '3':
            exitGame();
            running = false;
            break;
        default:
            Utility::clearScreen();
            ConsoleUtils::setColor(COLOR_DEFAULT);
            std::cout << "�߸��� �Է��Դϴ�. �ٽ� �õ��� �ּ���." << std::endl;
            Sleep(1000);
            break;
        }
    }
}

void GameManager::displayMainMenu() {
    Utility::clearScreen();
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "==========================" << std::endl;
    std::cout << "=== Ÿ�� ���� ���α׷� ===" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "  1. Ÿ�� ����" << std::endl;
    std::cout << "  2. �꼺�� ���� (�غ� ��)" << std::endl;
    std::cout << "  3. ����" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << "�޴��� �����ϼ���: ";
}

void GameManager::selectTypingMode() {
    bool running = true;
    while (running) {
        Utility::clearScreen();
        ConsoleUtils::setColor(COLOR_DEFAULT);
        std::cout << "======================" << std::endl;
        std::cout << "=== Ÿ�� ���� ��� ===" << std::endl;
        std::cout << "======================" << std::endl;
        std::cout << "  1. �ܾ� ����" << std::endl;
        std::cout << "  2. �� �� ����" << std::endl;
        std::cout << "  3. �ڷ� ����" << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << "��带 �����ϼ���: ";

        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == '1') {
            std::vector<std::string> words = Utility::loadContentFromFile("words.txt");
            typingPractice.run(words, "�ܾ�");
        }
        else if (choice == '2') {
            std::vector<std::string> longTexts = Utility::loadContentFromFile("long.txt");
            typingPractice.run(longTexts, "�� ��");
        }
        else if (choice == '3') {
            running = false;
        }
        else {
            Utility::clearScreen();
            ConsoleUtils::setColor(COLOR_DEFAULT);
            std::cout << "�߸��� �Է��Դϴ�. �ٽ� �õ��� �ּ���." << std::endl;
            Sleep(1000);
        }
    }
}

void GameManager::exitGame() {
    Utility::clearScreen();
    ConsoleUtils::setColor(COLOR_DEFAULT);
    std::cout << "���α׷��� �����մϴ�." << std::endl;
    Sleep(1000);
}

// ========================================================================================
// ���� �Լ�
// ========================================================================================
int main() {
    // �ܼ� â ���� ����
    SetConsoleTitle(L"Typing Practice Game");
    GameManager manager;
    manager.start();
    return 0;
}