#include "Memory_Game.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream> // ���� ������� ���� �߰�
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <limits> // std::numeric_limits�� ����ϱ� ���� �ʿ�

// Windows���� Ű���� �Է��� �����ϱ� ���� conio.h ����� �����մϴ�.
#ifdef _WIN32
#include <conio.h>
#endif

// �ܼ� ȭ���� ����� ���� ���� �Լ�
void clearConsoleScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ���� ç���� ������ ���� ������ ��� �ִ� �Լ�
void startMemoryChallenge() {
    // words.txt ���Ͽ��� �ܾ� ����� �ҷ��ɴϴ�.
    std::vector<std::string> allWords;
    std::ifstream file("words.txt");
    std::string word;

    if (!file.is_open()) {
        std::cerr << "\n����: words.txt ������ �� �� �����ϴ�." << std::endl;
        std::cerr << "���� ���ϰ� ���� ��ġ�� words.txt ������ �ִ��� Ȯ�����ּ���." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return; // ������ ������ �Լ� ����
    }

    while (file >> word) {
        allWords.push_back(word);
    }
    file.close();

    if (allWords.empty()) {
        std::cerr << "\n����: words.txt ���Ͽ� �ܾ �����ϴ�." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return; // ���� ������ ������ �Լ� ����
    }

    int score = 0;
    int wordsToMemorize = 1;
    std::vector<std::string> currentWords;
    std::string input;

    while (true) {
        // 1. �ܾ� ���� (selectWords ����)
        currentWords.clear();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        for (int i = 0; i < wordsToMemorize && i < allWords.size(); ++i) {
            currentWords.push_back(allWords[i]);
        }

        // 2. �ܾ� ǥ�� �� ī��Ʈ�ٿ� (displayWords ����)
        for (int i = 5; i > 0; --i) {
            clearConsoleScreen();
            std::cout << "--- �ܾ �ܿ��ּ���! ---" << std::endl;
            std::cout << "\n���� �ð�: " << i << "��\n" << std::endl;
            for (const auto& w : currentWords) {
                std::cout << w << std::endl;
            }
            std::cout << "\n------------------------------" << std::endl;

            // 1�� ���� ����ϸ鼭 Ű���� �Է��� �����մϴ� (Windows ����).
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
#ifdef _WIN32
                if (_kbhit()) { // Ű�� ���ȴ��� Ȯ��
                    _getch();   // ���� Ű�� ���ۿ��� ����
                }
#endif
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // CPU ��뷮 ����
            }
        }

        clearConsoleScreen();

        // 3. ����� �Է� Ȯ�� (checkUserInput ����)
        // "Enter�� ����" �κ��� �����ϰ� �ٷ� �Է��� �޽��ϴ�.
        std::cout << "--- ����� �ܾ �Է��ϼ��� (�ܾ� �Է� �� Enter, �޴��� ���ư����� 'q') ---" << std::endl;
        std::vector<std::string> userWords;

        for (int i = 0; i < wordsToMemorize; ++i) {
            std::cout << i + 1 << "��° �ܾ�: ";
            std::cin >> input;
            if (input == "q") break;
            userWords.push_back(input);
        }

        // 'q'�� �Է��ϸ� ������ �ߴ��ϰ� �޴��� ���ư�
        if (input == "q") {
            std::cout << "\n������ �ߴ��մϴ�." << std::endl;
            break;
        }

        int correctCount = 0;
        std::vector<std::string> tempCurrentWords = currentWords; // �ߺ� ���� ������ ���� �ӽ� ����
        for (const auto& uWord : userWords) {
            // ���� ��Ͽ��� ����ڰ� �Է��� �ܾ ã��
            auto it = std::find(tempCurrentWords.begin(), tempCurrentWords.end(), uWord);
            if (it != tempCurrentWords.end()) {
                correctCount++;
                tempCurrentWords.erase(it); // ���� �ܾ�� �ӽ� ��Ͽ��� �����Ͽ� �ߺ� ���� ����
            }
        }

        if (correctCount == 0) {
            std::cout << "\n�ƽ��׿�! �ϳ��� ������ ���߽��ϴ�." << std::endl;
            std::cout << "���� ����: " << score << "��" << std::endl;
            break;
        }

        // 4. ���� ������Ʈ (updateGame ����)
        score += correctCount * 10;
        wordsToMemorize = 1 + (score / 100);

        // ��� ���
        std::cout << "\n--- ��� ---" << std::endl;
        std::cout << "���� ����: " << correctCount << " / " << currentWords.size() << std::endl;
        std::cout << "���� ����: " << score << "��" << std::endl;
        std::cout << "���� ���� �ܾ� ����: " << wordsToMemorize << std::endl;
        std::cout << "--------------" << std::endl;
        std::cout << "����Ϸ��� Enter Ű�� ��������..." << std::endl;

        // ������ �ܾ� �Է� �� ���ۿ� �����ִ� ���� ���ڸ� ���ϴ�.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        // ����ڰ� Enter Ű�� ���� ������ ��ٸ��ϴ�.
        std::cin.get();
    }

    std::cout << "\n���� ç������ ����Ǿ����ϴ�. ��� �� �޴��� ���ư��ϴ�." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 2�� �� �޴���
}
