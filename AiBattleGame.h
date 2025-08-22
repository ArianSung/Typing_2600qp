// AiBattleGame.h

#pragma once // ��� ������ �ߺ� ������ �����մϴ�.

#include <string>
#include <vector>

// 8x8 ���ڿ��� �ܾ��� ������ �����ϱ� ���� ����ü
struct GridWord {
    std::wstring text; // [����] std::string -> std::wstring
    int x, y;
    bool isActive = true;
};

/**
 * @brief AI�� �ܾ� ����� ��� ������ �����մϴ�.
 */
void startAiBattleGame();
void setConsoleSize(int width, int height);
