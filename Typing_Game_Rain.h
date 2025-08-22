#pragma once
// TypingGame.h

#pragma once // 헤더 파일이 중복으로 포함되는 것을 방지합니다.

// ## 필요한 표준 라이브러리 포함 ##
// 이 헤더 파일을 사용하는 곳에서 다시 include할 필요가 없도록 미리 포함해줍니다.
#include <vector>
#include <string>
#include <windows.h> // COORD, gotoxy_raingame 등을 위해 필요합니다.
//
// ## 전역 상수 선언 ##
// 게임 보드의 크기를 상수로 정의합니다.
#define BOARD_WIDTH 80
#define BOARD_HEIGHT 40

// ## 함수 선언 (Function Prototypes) ##
// 다른 파일에서 이 함수들을 사용할 수 있도록 외부에 공개합니다.

/**
 * @brief 지정된 좌표로 콘솔 커서를 이동시킵니다.
 * @param x 가로 좌표 (열)
 * @param y 세로 좌표 (행)
 */
void gotoxy_raingame(int x, int y);

/**
 * @brief 콘솔 창의 크기를 설정하고 커서를 보이지 않게 합니다.
 */
void setConsoleSize();

/**
 * @brief 파일에서 단어 목록을 불러와 vector에 저장합니다.
 * @param words 단어를 저장할 vector 객체 (참조 전달)
 * @param filename 읽어올 파일의 이름
 */
void loadWordsFromFile(std::vector<std::string>& words, const std::string& filename);

/**
 * @brief 메인 타자 게임을 실행합니다.
 */
void startRainGame();
