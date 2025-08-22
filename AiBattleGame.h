// AiBattleGame.h

#pragma once // 헤더 파일의 중복 포함을 방지합니다.

#include <string>
#include <vector>

// 8x8 격자에서 단어의 정보를 관리하기 위한 구조체
struct GridWord {
    std::wstring text; // [수정] std::string -> std::wstring
    int x, y;
    bool isActive = true;
};

/**
 * @brief AI와 단어 지우기 대결 게임을 시작합니다.
 */
void startAiBattleGame();
void setConsoleSize(int width, int height);
