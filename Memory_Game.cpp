#include "Memory_Game.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream> // 파일 입출력을 위해 추가
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <limits> // std::numeric_limits를 사용하기 위해 필요

// Windows에서 키보드 입력을 감지하기 위해 conio.h 헤더를 포함합니다.
#ifdef _WIN32
#include <conio.h>
#endif

// 콘솔 화면을 지우는 내부 헬퍼 함수
void clearConsoleScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 기억력 챌린지 게임의 메인 로직을 담고 있는 함수
void startMemoryChallenge() {
    // words.txt 파일에서 단어 목록을 불러옵니다.
    std::vector<std::string> allWords;
    std::ifstream file("words.txt");
    std::string word;

    if (!file.is_open()) {
        std::cerr << "\n오류: words.txt 파일을 열 수 없습니다." << std::endl;
        std::cerr << "실행 파일과 같은 위치에 words.txt 파일이 있는지 확인해주세요." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return; // 파일이 없으면 함수 종료
    }

    while (file >> word) {
        allWords.push_back(word);
    }
    file.close();

    if (allWords.empty()) {
        std::cerr << "\n오류: words.txt 파일에 단어가 없습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return; // 파일 내용이 없으면 함수 종료
    }

    int score = 0;
    int wordsToMemorize = 1;
    std::vector<std::string> currentWords;
    std::string input;

    while (true) {
        // 1. 단어 선택 (selectWords 로직)
        currentWords.clear();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        for (int i = 0; i < wordsToMemorize && i < allWords.size(); ++i) {
            currentWords.push_back(allWords[i]);
        }

        // 2. 단어 표시 및 카운트다운 (displayWords 로직)
        for (int i = 5; i > 0; --i) {
            clearConsoleScreen();
            std::cout << "--- 단어를 외워주세요! ---" << std::endl;
            std::cout << "\n남은 시간: " << i << "초\n" << std::endl;
            for (const auto& w : currentWords) {
                std::cout << w << std::endl;
            }
            std::cout << "\n------------------------------" << std::endl;

            // 1초 동안 대기하면서 키보드 입력을 무시합니다 (Windows 전용).
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
#ifdef _WIN32
                if (_kbhit()) { // 키가 눌렸는지 확인
                    _getch();   // 눌린 키를 버퍼에서 제거
                }
#endif
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // CPU 사용량 감소
            }
        }

        clearConsoleScreen();

        // 3. 사용자 입력 확인 (checkUserInput 로직)
        // "Enter로 시작" 부분을 제거하고 바로 입력을 받습니다.
        std::cout << "--- 기억한 단어를 입력하세요 (단어 입력 후 Enter, 메뉴로 돌아가려면 'q') ---" << std::endl;
        std::vector<std::string> userWords;

        for (int i = 0; i < wordsToMemorize; ++i) {
            std::cout << i + 1 << "번째 단어: ";
            std::cin >> input;
            if (input == "q") break;
            userWords.push_back(input);
        }

        // 'q'를 입력하면 게임을 중단하고 메뉴로 돌아감
        if (input == "q") {
            std::cout << "\n게임을 중단합니다." << std::endl;
            break;
        }

        int correctCount = 0;
        std::vector<std::string> tempCurrentWords = currentWords; // 중복 점수 방지를 위한 임시 벡터
        for (const auto& uWord : userWords) {
            // 정답 목록에서 사용자가 입력한 단어를 찾음
            auto it = std::find(tempCurrentWords.begin(), tempCurrentWords.end(), uWord);
            if (it != tempCurrentWords.end()) {
                correctCount++;
                tempCurrentWords.erase(it); // 맞춘 단어는 임시 목록에서 제거하여 중복 점수 방지
            }
        }

        if (correctCount == 0) {
            std::cout << "\n아쉽네요! 하나도 맞추지 못했습니다." << std::endl;
            std::cout << "최종 점수: " << score << "점" << std::endl;
            break;
        }

        // 4. 게임 업데이트 (updateGame 로직)
        score += correctCount * 10;
        wordsToMemorize = 1 + (score / 100);

        // 결과 출력
        std::cout << "\n--- 결과 ---" << std::endl;
        std::cout << "맞춘 개수: " << correctCount << " / " << currentWords.size() << std::endl;
        std::cout << "현재 점수: " << score << "점" << std::endl;
        std::cout << "다음 라운드 단어 개수: " << wordsToMemorize << std::endl;
        std::cout << "--------------" << std::endl;
        std::cout << "계속하려면 Enter 키를 누르세요..." << std::endl;

        // 마지막 단어 입력 후 버퍼에 남아있는 개행 문자를 비웁니다.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        // 사용자가 Enter 키를 누를 때까지 기다립니다.
        std::cin.get();
    }

    std::cout << "\n기억력 챌린지가 종료되었습니다. 잠시 후 메뉴로 돌아갑니다." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 2초 후 메뉴로
}
