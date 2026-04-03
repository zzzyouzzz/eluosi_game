#ifndef ELUOSI_H
#define ELUOSI_H

#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <pthread.h>

#define BLOCK_SIZE 30
#define BOARD_WIDTH 11
#define BOARD_HEIGHT 21

struct Tetromino {
    int type;
    int rotation = 0;
    int x;
    float y;
};

struct Button {
    int x, y;
    int width, height;
    const TCHAR* text;
};

extern float b_volume, g_volume, c_volume, b_s_volume;
extern IMAGE bgImg;
extern int score;
extern int nextType;
extern bool gameOver;
extern bool isPaused;
extern bool inGame;
extern bool isFull;
extern bool isclicked;
extern COLORREF blockColors[7];
extern int board[BOARD_HEIGHT][BOARD_WIDTH];
extern int shapes[7][4][4][2];
extern Button startBtn;
extern Button restartBtn;
extern Button exitBtn;
extern Button continueBtn;
extern Button pauseRestartBtn;
extern Button pauseExitBtn;
extern pthread_mutex_t lock;
extern pthread_mutex_t lock2;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;
extern sf::Music bgm,gameover_sound,clear_sound,button_sound;
extern Tetromino current;

void drawButton(const Button& btn);
bool isButtonClicked(const Button& btn, MOUSEMSG msg);
void resetGame();
void drawBlock(int x, float y, COLORREF color);
void* drawBoard(void* arg);
void clearFullLines();
bool checkCollision();
void drawStartScreen(Button& startBtn, Button& exitBtn);
void drawEndScreen(const Button& restartBtn, const Button& exitBtn, int score);
void drawPauseScreen(const Button& continueBtn, const Button& restartBtn, const Button& exitBtn);
void* PlaySoundThread(void* unused);
void playButtonSound();
void initGame();
void* operation(void* arg); 

#endif // ELUOSI_H