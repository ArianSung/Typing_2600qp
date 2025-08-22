// [해결] Windows의 min/max 매크로와 std::max의 충돌을 방지합니다.
#define NOMINMAX

#include "Memory_Game.h"
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
#include <limits>

#ifdef _WIN32
#include <conio.h>
#include <windows.h> // NOMINMAX는 이 헤더가 포함되기 전에 정의되어야 합니다.
#endif

// [추가] UTF-8 변환 및 파일 로딩 헬퍼 함수
static std::wstring utf8_to_wstring_mem(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

static std::vector<std::wstring> loadWordsToWstring_mem(const std::string& filename) {
    std::vector<std::wstring> content;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return content;
    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    if (file_contents.size() >= 3 && (unsigned char)file_contents[0] == 0xEF && (unsigned char)file_contents[1] == 0xBB && (unsigned char)file_contents[2] == 0xBF) {
        file_contents = file_contents.substr(3);
    }
    std::wstring w_contents = utf8_to_wstring_mem(file_contents);
    std::wstringstream wss(w_contents);
    std::wstring line;
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty()) content.push_back(line);
    }
    return content;
}


void clearConsoleScreen() {
    system("cls");
}

void startMemoryChallenge() {
    // [수정] 유니코드 파일 로딩
    std::vector<std::wstring> allWords = loadWordsToWstring_mem("words.txt");

    if (allWords.empty()) {
        std::wcerr << L"\n오류: words.txt 파일을 열 수 없거나 내용이 비어있습니다." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return;
    }

    int score = 0;
    int wordsToMemorize = 1;
    std::vector<std::wstring> currentWords; // [수정] string -> wstring
    std::wstring input; // [수정] string -> wstring

    while (true) {
        currentWords.clear();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);
        for (int i = 0; i < wordsToMemorize && i < allWords.size(); ++i) {
            currentWords.push_back(allWords[i]);
        }

        for (int i = 5; i > 0; --i) {
            clearConsoleScreen();
            std::wcout << L"--- 단어를 외워주세요! ---" << std::endl;
            std::wcout << L"\n남은 시간: " << i << L"초\n" << std::endl;
            for (const auto& w : currentWords) {
                std::wcout << w << std::endl;
            }
            std::wcout << L"\n------------------------------" << std::endl;

            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(1)) {
#ifdef _WIN32
                if (_kbhit()) { _getch(); }
#endif
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        clearConsoleScreen();

        std::wcout << L"--- 기억한 단어를 입력하세요 (단어 입력 후 Enter, 메뉴로 돌아가려면 'q') ---" << std::endl;
        std::vector<std::wstring> userWords;

        for (int i = 0; i < wordsToMemorize; ++i) {
            std::wcout << i + 1 << L"번째 단어: ";
            std::wcin >> input; // [수정] cin -> wcin
            if (input == L"q") break;
            userWords.push_back(input);
        }

        if (input == L"q") {
            std::wcout << L"\n게임을 중단합니다." << std::endl;
            break;
        }

        int correctCount = 0;
        std::vector<std::wstring> tempCurrentWords = currentWords;
        for (const auto& uWord : userWords) {
            auto it = std::find(tempCurrentWords.begin(), tempCurrentWords.end(), uWord);
            if (it != tempCurrentWords.end()) {
                correctCount++;
                tempCurrentWords.erase(it);
            }
        }

        if (correctCount == 0) {
            std::wcout << L"\n아쉽네요! 하나도 맞추지 못했습니다." << std::endl;
            std::wcout << L"최종 점수: " << score << L"점" << std::endl;
            break;
        }

        score += correctCount * 10;
        wordsToMemorize = 1 + (score / 100);

        std::wcout << L"\n--- 결과 ---" << std::endl;
        std::wcout << L"맞춘 개수: " << correctCount << L" / " << currentWords.size() << std::endl;
        std::wcout << L"현재 점수: " << score << L"점" << std::endl;
        std::wcout << L"다음 라운드 단어 개수: " << wordsToMemorize << std::endl;
        std::wcout << L"--------------" << std::endl;
        std::wcout << L"계속하려면 Enter 키를 누르세요..." << std::endl;

        std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
        std::wcin.get();
    }

    std::wcout << L"\n기억력 챌린지가 종료되었습니다. 잠시 후 메뉴로 돌아갑니다." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
