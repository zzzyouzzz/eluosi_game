#include "eluosi.h"
//#include "eluosi.cpp"
int main() {
    initGame();
    pthread_t soundThread;
    pthread_create(&soundThread, NULL, PlaySoundThread, NULL);
    bool showStartScreen = true;

    drawStartScreen(startBtn, exitBtn);
    
    while (showStartScreen) {
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (isButtonClicked(startBtn, msg)) {
                playButtonSound();  // 开始按钮点击音效
                showStartScreen = false;
                resetGame();
                nextType = rand() % 7;
                current.type = rand() % 7;
                current.x = BOARD_WIDTH / 2 ;
                current.y = 1;
                current.rotation = 0;
            }
            if (isButtonClicked(exitBtn, msg)) {
                  // 退出按钮点击音效
                showStartScreen = false;
                inGame = false;
                playButtonSound();
            }
        }
        Sleep(10);
    }

    

    while(inGame) {
        int lastTime = clock();
        int coolTime = clock();
        float speed=0.1;
            
        bgm.play();

        pthread_t gameloop;
        pthread_create(&gameloop, NULL, drawBoard, NULL);
        pthread_t op;
        pthread_create(&op, NULL, operation, NULL);
        while (!gameOver&&inGame) {          
            if(!isPaused){

                if (GetAsyncKeyState('Q') & 0x8000) {
                    playButtonSound();  // Q键暂停音效
                    bgm.pause();
                    isPaused = true;
                }
                if(clock()-coolTime>40){
                    current.y+=speed;
                    coolTime=clock();
                }
                if(clock()-lastTime>10000){
                    lastTime=clock();
                    speed+=0.01;
                }

                if (checkCollision()) {
                    while (checkCollision()) current.y-=0.1;
                    for (int i = 0; i < 4; i++) {
                        int dx = shapes[current.type][current.rotation][i][1];
                        int dy = shapes[current.type][current.rotation][i][0];
                        board[(int)ceil(current.y + dy)][(int)ceil(current.x + dx)] = current.type +1;
                    }
                    
                    current.type = nextType;
                    nextType = rand() % 7;
                    current.x = BOARD_WIDTH / 2 ;
                    current.y = 1;
                    current.rotation = 0;
                    clearFullLines();
                    if (checkCollision()) gameOver = true;
                }
                Sleep(10);
            }else{
                drawPauseScreen(continueBtn, pauseRestartBtn, pauseExitBtn);               
                while(isPaused){
                    if (MouseHit()) {
                        MOUSEMSG msg = GetMouseMsg();
                        // 继续游戏：恢复 BGM
                        if (isButtonClicked(continueBtn, msg)) {
                            playButtonSound();  // 暂停界面继续游戏音效
                            isPaused = false;
                            pthread_cond_signal(&cond);
                            // 重新打开并播放 BGM（与键盘暂停恢复逻辑一致）
                            bgm.play();
                        }
                        // 重新开始：resetGame 已包含 BGM 重启逻辑
                        if (isButtonClicked(pauseRestartBtn, msg)) {
                            playButtonSound();  // 暂停界面重新开始音效
                            resetGame();
                            bgm.stop();
                            bgm.play();
                            isPaused = false;
                            pthread_cond_signal(&cond);
                            nextType = rand() % 7;
                            current.type = nextType;
                            current.x = BOARD_WIDTH / 2;
                            current.y = 1;
                        }
                        // 退出游戏：关闭 BGM（原逻辑已包含）
                        if (isButtonClicked(pauseExitBtn, msg)) {
                            isPaused = false;
                            inGame = false;
                            playButtonSound();  // 暂停界面退出音效
                            pthread_cond_signal(&cond);
                        }
                    }
                    Sleep(10); 
                }
            }
        }
        pthread_join(gameloop, NULL);
        pthread_join(op, NULL);
        bgm.stop();
        if (gameOver) {          
            gameover_sound.play();
            drawEndScreen(restartBtn, exitBtn, score);
            while (gameOver) {

                if (MouseHit()) {
                    MOUSEMSG msg = GetMouseMsg();
                    if (isButtonClicked(restartBtn, msg)) {                       
                        playButtonSound();  // 游戏结束界面重新开始音效
                        resetGame();
                        nextType = rand() % 7;
                        current.type = nextType;
                        current.x = BOARD_WIDTH / 2;
                        current.y = 1;                        
                        gameOver = false;
                    }
                    if (isButtonClicked(exitBtn, msg)) {
                          // 游戏结束界面退出音效
                        gameOver = false;
                        inGame = false;
                        playButtonSound();
                    }
                }
                Sleep(10);
            }
            gameover_sound.stop();
        }
    }
    pthread_join(soundThread, NULL);
    closegraph();
    return 0;
}
