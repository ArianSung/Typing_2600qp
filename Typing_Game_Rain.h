#pragma once
// TypingGame.h

#pragma once // ��� ������ �ߺ����� ���ԵǴ� ���� �����մϴ�.

// ## �ʿ��� ǥ�� ���̺귯�� ���� ##
// �� ��� ������ ����ϴ� ������ �ٽ� include�� �ʿ䰡 ������ �̸� �������ݴϴ�.
#include <vector>
#include <string>
#include <windows.h> // COORD, gotoxy_raingame ���� ���� �ʿ��մϴ�.

// ## ���� ��� ���� ##
// ���� ������ ũ�⸦ ����� �����մϴ�.
#define BOARD_WIDTH 80
#define BOARD_HEIGHT 25

// ## �Լ� ���� (Function Prototypes) ##
// �ٸ� ���Ͽ��� �� �Լ����� ����� �� �ֵ��� �ܺο� �����մϴ�.

/**
 * @brief ������ ��ǥ�� �ܼ� Ŀ���� �̵���ŵ�ϴ�.
 * @param x ���� ��ǥ (��)
 * @param y ���� ��ǥ (��)
 */
void gotoxy_raingame(int x, int y);

/**
 * @brief �ܼ� â�� ũ�⸦ �����ϰ� Ŀ���� ������ �ʰ� �մϴ�.
 */
void setConsoleSize();

/**
 * @brief ���Ͽ��� �ܾ� ����� �ҷ��� vector�� �����մϴ�.
 * @param words �ܾ ������ vector ��ü (���� ����)
 * @param filename �о�� ������ �̸�
 */
void loadWordsFromFile(std::vector<std::string>& words, const std::string& filename);

/**
 * @brief ���� Ÿ�� ������ �����մϴ�.
 */
void startRainGame();
