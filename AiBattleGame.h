#pragma once
// AiBattleGame.h

#pragma once // ��� ������ �ߺ� ������ �����մϴ�.

#include <string>
#include <vector>

// 8x8 ���ڿ��� �ܾ��� ������ �����ϱ� ���� ����ü
struct GridWord {
    std::string text;
    int x, y;
    bool isActive = true;
};

/**
 * @brief AI�� �ܾ� ����� ��� ������ �����մϴ�.
 */
void startAiBattleGame();
void setConsoleSize(int width, int height);