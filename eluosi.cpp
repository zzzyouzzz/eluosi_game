#include "eluosi.h"
/*************************音效音量********************************/ 
float b_volume=35.0,g_volume=50.0,c_volume=50.0,b_s_volume=80.0;
/****************************************************************/
int score = 0;
int nextType;
bool gameOver = false;
bool isPaused = false;
bool inGame = true;
bool isFull = false;
bool isclicked = false;
int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };
int shapes[7][4][4][2] = {
    {{{0,0},{1,0},{2,0},{3,0}},{{1,-2},{1,-1},{1,0},{1,1}},{{-1,0},{0,0},{1,0},{2,0}},{{1,-1},{1,0},{1,1},{1,2}}},
    {{{0,1},{1,0},{1,1},{1,2}},{{0,1},{1,1},{1,2},{2,1}},{{1,0},{1,1},{1,2},{2,1}},{{0,1},{1,0},{1,1},{2,1}}},
    {{{0,0},{0,1},{1,0},{1,1}},{{0,0},{0,1},{1,0},{1,1}},{{0,0},{0,1},{1,0},{1,1}},{{0,0},{0,1},{1,0},{1,1}}},
    {{{0,0},{0,1},{0,2},{1,0}},{{0,-1},{0,0},{1,0},{2,0}},{{0,-2},{0,-1},{0,0},{-1,0}},{{-2,0},{-1,0},{0,0},{0,1}}},
    {{{0,0},{1,0},{1,1},{1,2}},{{1,0},{1,1},{2,0},{3,0}},{{1,0},{1,-1},{1,-2},{2,0}},{{1,0},{1,-1},{0,0},{-1,0}}},
    {{{0,1},{0,2},{1,0},{1,1}},{{0,1},{1,1},{1,2},{2,2}},{{1,1},{1,2},{2,0},{2,1}},{{0,0},{1,0},{1,1},{2,1}}},
    {{{0,0},{0,1},{1,1},{1,2}},{{0,2},{1,1},{1,2},{2,1}},{{1,0},{1,1},{2,1},{2,2}},{{0,1},{1,0},{1,1},{2,0}}},
};
int windowWidth = BOARD_WIDTH * BLOCK_SIZE + 200;
Button startBtn = { (windowWidth - 120) / 2, 400, 120, 40, _T("START") };
Button restartBtn = { (windowWidth - 120) / 2, 400, 120, 40, _T("RESTART") };
Button exitBtn = { (windowWidth - 120) / 2, 500, 120, 40, _T("EXIT") };
Button continueBtn = { (windowWidth - 120) / 2, 300, 120, 40, _T("CONTINUE") };
Button pauseRestartBtn = { (windowWidth - 120) / 2, 400, 120, 40, _T("RESTART") };
Button pauseExitBtn = { (windowWidth - 120) / 2, 500, 120, 40, _T("EXIT") };
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
sf::Music bgm,gameover_sound,clear_sound,button_sound;
Tetromino current;
IMAGE bgImg;
COLORREF blockColors[7];

void* operation(void* arg){
    while(!gameOver&&inGame){
        if(!isPaused){
            if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
                playButtonSound();  // 左键或A键点击音�?
                current.x--;
                while (checkCollision()) current.x++;
                Sleep(100);  // 添加延迟防止连续触发
            }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
                playButtonSound();  // 右键或D键点击音�?
                current.x++;
                while (checkCollision()) current.x--;
                Sleep(100);  // 添加延迟防止连续触发
            }
            if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
                current.rotation = (current.rotation + 1) % 4;
                int i=0,x=current.x;
                while(i<2&&checkCollision()){
                    i++;
                    current.x=x+i;
                    //printf("%d\n",i);
                }
                i=0;
                while(i<2&&checkCollision()){
                    i++;
                    current.x=x-i;
                    //printf("%d\n",i);
                }
                if (checkCollision()){
                    current.x=x;
                    current.rotation = (current.rotation + 3) % 4;
                    gameover_sound.play();
                    Sleep(500);
                    gameover_sound.stop();
                }else{
                    playButtonSound();  // 上键或W键点击音�?
                }
                Sleep(200);  // 旋转添加稍长延迟
            }
            if (current.y>1.5&&GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000){
                playButtonSound();  // 下键或S键点击音�?
                current.y+=0.2;
                while (current.y>=1&&checkCollision()){
                    current.y-=0.01;
                    //printf("%f\n",current.y);
                    Sleep(20);
                } 
                Sleep(1);
            }
        }
    }
    return NULL;
}

void initGame(){
    initgraph((BOARD_WIDTH + 1) * BLOCK_SIZE + 200, (BOARD_HEIGHT + 1) * BLOCK_SIZE);
    HWND hwnd = GetHWnd();
    SetWindowTextW(hwnd, L"Tetris");
    SetWindowPos(hwnd, HWND_TOP, 700, 20, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
    setbkmode(TRANSPARENT);
    srand(time(0));
    loadimage(&bgImg, _T("image/bgp.png"));
    bgm.openFromFile("music\\kun.wav");
    gameover_sound.openFromFile("music\\ji.wav");
    clear_sound.openFromFile("music\\clear.wav");
    button_sound.openFromFile("music\\button.wav");
    bgm.setVolume(b_volume);
    gameover_sound.setVolume(g_volume);
    clear_sound.setVolume(c_volume);
    button_sound.setVolume(b_s_volume);
}

void drawButton(const Button& btn) {
    setfillcolor(LIGHTGRAY);
    solidrectangle(btn.x, btn.y, btn.x + btn.width, btn.y + btn.height);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("Arial"));
    int textWidth = textwidth(btn.text);
    int textHeight = textheight(btn.text);
    outtextxy(btn.x + (btn.width - textWidth) / 2, btn.y + (btn.height - textHeight) / 2, btn.text);
}

bool isButtonClicked(const Button& btn, MOUSEMSG msg) {
    return msg.uMsg == WM_LBUTTONDOWN &&
        msg.x >= btn.x && msg.x <= btn.x + btn.width &&
        msg.y >= btn.y && msg.y <= btn.y + btn.height;
}

void resetGame() {
    score = 0;
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            board[y][x] = 0;
        }
    }
    current.type = rand() % 7;
    current.x = BOARD_WIDTH / 2 - 2;
    current.y = 0;
    
    // 新增：初始化随机颜色（排除黑色）
    for (int i = 0; i < 7; i++) {
        COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
        for(int j=i;j>=0;j--){
            if(color==blockColors[j]||color==BLACK){
                color = RGB(rand() % 256, rand() % 256, rand() % 256);
                j=i;
            }
        }
        blockColors[i] = color;
    }
}

void drawBlock(int x, float y, COLORREF color) {
    setfillcolor(color);
    solidrectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, (x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    setlinecolor(WHITE);
    rectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, (x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
}

// 新增：用于下落预测的碰撞检测函�?
bool checkCollisionForGhost(const Tetromino& t) {
    for (int i = 0; i < 4; i++) {
        int dx = shapes[t.type][t.rotation][i][1];
        int dy = shapes[t.type][t.rotation][i][0];
        int nx = t.x + dx;
        int ny = ceil(t.y + dy);

        if (nx <= 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT || board[ny][nx])
            return true;
    }
    return false;
}

void* drawBoard(void* arg) {
    while(!gameOver){
        pthread_mutex_lock(&lock);
        if(isPaused||isFull){
            pthread_cond_wait(&cond, &lock);
        }
        if(!inGame){
            pthread_exit(NULL);
        }
        BeginBatchDraw();
        cleardevice();
        putimage(0, 0, &bgImg);
        setlinecolor(WHITE);
        rectangle(28, 28, BOARD_WIDTH * BLOCK_SIZE+2, BOARD_HEIGHT * BLOCK_SIZE+2);
        setlinecolor(LIGHTGRAY);
        for (int x = BLOCK_SIZE; x <= BOARD_WIDTH * BLOCK_SIZE; x += BLOCK_SIZE) {
            line(x, BLOCK_SIZE, x, BOARD_HEIGHT * BLOCK_SIZE);
        }
        for (int y = BLOCK_SIZE; y <= BOARD_HEIGHT * BLOCK_SIZE; y += BLOCK_SIZE) {
            line(BLOCK_SIZE, y, BOARD_WIDTH * BLOCK_SIZE, y);
        }

        // 新增：计算并绘制下落预测位置
        if (!gameOver && !isPaused) {
            Tetromino ghost = current;
            // 计算方块最终下落位�?
            while (!checkCollisionForGhost(ghost)) {
                ghost.y += 0.01;
            }
            //ghost.y -= 0.1; // 回退到最后有效位�?
            
            // 绘制下落预测（虚线边框）
            for (int i = 0; i < 4; i++) {
                int dx = shapes[ghost.type][ghost.rotation][i][1];
                int dy = shapes[ghost.type][ghost.rotation][i][0];
                int nx = ghost.x + dx;
                float ny = ghost.y + dy;
                
                // 设置虚线样式
                setlinestyle(PS_DASH, 1);
                setlinecolor(WHITE);
                rectangle(nx * BLOCK_SIZE+4, ny * BLOCK_SIZE+4, 
                         (nx + 1) * BLOCK_SIZE-4, (ny + 1) * BLOCK_SIZE-4);
                setlinestyle(PS_SOLID, 1); // 恢复实线
            }
        }

        // 修改：使用随机颜色绘制当前方�?
        for (int i = 0; i < 4; i++) {
            int dx = shapes[current.type][current.rotation][i][1];
            int dy = shapes[current.type][current.rotation][i][0];

            drawBlock(current.x + dx, current.y + dy, blockColors[current.type]);  
        }

        // 修改：使用随机颜色绘制已固定的方�?
        for (int y = 1; y < BOARD_HEIGHT; y++) {
            for (int x = 1; x < BOARD_WIDTH; x++) {
                if (board[y][x]) {
                    drawBlock(x, y, blockColors[board[y][x]-1]);
                }
            }
        }
        // 绘制下一个方�?
        int minDx = 4, maxDx = -1, minDy = 4, maxDy = -1;
        settextstyle(40, 0, _T("Arial"));
        settextcolor(BLACK);
        TCHAR scoreText[20];
        _stprintf(scoreText, _T("Score: %d"), score);
        int windowHeight = BOARD_HEIGHT * BLOCK_SIZE;
        int previewHeight = 200;
        int previewTop = (windowHeight - previewHeight) / 2-40;
        int previewLeft = (BOARD_WIDTH + 1) * BLOCK_SIZE;
        int previewWidth = 200;
        int scoreX = previewLeft + (previewWidth - textwidth(scoreText)) / 2;
        int scoreY = previewTop - 80;
        outtextxy(scoreX, scoreY, scoreText);

        settextstyle(20, 0, _T("Georgia"));  
        TCHAR controls[][50] = {
            _T("Operations:"),
            _T("Q - Pause"),
            _T("A - Left"),
            _T("D - Right"),
            _T("up/W - Rotate"),
            _T("down/S - Speed Down")
        };
        int controlStartY = previewTop + previewHeight + 30;  
        for (int i = 0; i < 6; i++) {
            int textWidth = textwidth(controls[i]);
            int textX = previewLeft + (previewWidth - textWidth) / 2;
            int textY = controlStartY + i * 30;  // 每行间隔30像素
            outtextxy(textX, textY, controls[i]);
        }
 
        int previewRight = previewLeft + previewWidth -1;
        int previewBottom = previewTop + previewHeight;

        settextcolor(BLACK);
        settextstyle(30, 0, _T("Georgia"));
        TCHAR title[20];
        _stprintf_s(title, _T("Next Block"));
        outtextxy(previewLeft + (previewWidth - textwidth(title)) / 2, previewTop - 40, title);

        setlinecolor(WHITE);
        rectangle(previewLeft, previewTop, previewRight, previewBottom);


        for (int i = 0; i < 4; i++) {
            int dx = shapes[nextType][0][i][1];
            int dy = shapes[nextType][0][i][0];
            minDx = std::min(minDx, dx);
            maxDx = std::max(maxDx, dx);
            minDy = std::min(minDy, dy);
            maxDy = std::max(maxDy, dy);
        }
        int blockWidth = (maxDx - minDx + 1) * BLOCK_SIZE;
        int blockHeight = (maxDy - minDy + 1) * BLOCK_SIZE;

        int centerX = previewLeft + (previewWidth - blockWidth) / 2+10;
        int centerY = previewTop + (previewHeight - blockHeight) / 2;

        for (int i = 0; i < 4; i++) {
            int dx = shapes[nextType][0][i][1];
            int dy = shapes[nextType][0][i][0];
            int px = centerX + (dx - minDx) * BLOCK_SIZE;
            int py = centerY + (dy - minDy) * BLOCK_SIZE;
            drawBlock(px / BLOCK_SIZE, py / BLOCK_SIZE, blockColors[nextType]);
        }
        EndBatchDraw();
        pthread_mutex_unlock(&lock);
        Sleep(10);
        
    }
    return NULL;
}

void clearFullLines() {
    int cleared = 0;     
    int Full_lines[BOARD_HEIGHT] = { 0 };
    for (int y = BOARD_HEIGHT-1; y >= 1; y--) {
        for (int x = 1; x < BOARD_WIDTH; x++) {
            Full_lines[y] += (board[y][x] != 0);
        }
        if(Full_lines[y]==10){
            Sleep(20);
            isFull=true; 
        }
    }
    if(isFull){
        clear_sound.stop();
        clear_sound.play();
        for(int y=BOARD_HEIGHT-1;y>=1;y--){
            if(Full_lines[y]==10){
                cleared++;
                for(int x=1;x<BOARD_WIDTH;x++){
                    drawBlock(x,y,WHITE); 
                }
            }
        }
        Sleep(300);
        isFull=false;
        pthread_cond_signal(&cond);
    }
    int p=BOARD_HEIGHT-1,q=2;
    while(p>=1&&Full_lines[p]!=10){
        p--;
    }
    q=p-1;
    while(p>=1){
        while(q>=1&&Full_lines[q]==10){
            q--;
        }
        if(q>=1){        
            for(int i=0;i<BOARD_WIDTH;i++){
                board[p][i]=board[q][i];
            }
            p--;
            q--;
        }else{
            for(int i=0;i<BOARD_WIDTH;i++){
                board[p][i]=0;
            }
            p--;
        }
    }
    if (cleared > 0) {
        switch (cleared) {
        case 1: score += 100; break;
        case 2: score += 300; break;
        case 3: score += 500; break;
        case 4: score += 800; break;
        }
    }

}

bool checkCollision() {
    for (int i = 0; i < 4; i++) {
        int dx = shapes[current.type][current.rotation][i][1];
        int dy = shapes[current.type][current.rotation][i][0];
        int nx = current.x + dx;
        int ny = ceil(current.y + dy);

        if (nx <= 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT || board[ny][nx])
            return true;
    }
    return false;
}

void drawStartScreen(Button& startBtn, Button& exitBtn) {
    cleardevice();
     putimage(0, 0, &bgImg);  
    int windowWidth = BOARD_WIDTH * BLOCK_SIZE + 200;

    settextstyle(70, 0, _T("Lucida Handwriting"));
    settextcolor(CYAN);
    TCHAR title[20];
    _stprintf_s(title, _T("TETRIS"));
    outtextxy((windowWidth - textwidth(title)) / 2, 30, title);

     settextstyle(20, 0, _T("Georgia"));
    settextcolor(WHITE);
    TCHAR controls[][50] = {
        _T("Operations Introduction:"),
        _T("Q-pause"),
        _T("A-left"),
        _T("D-right"),
        _T("up/W-rotate"),
        _T("down/S-speed down")
    };
    int lineHeight = 40;
    int startY = 120;  // 标题下方起始位置
    for (int i = 0; i < 6; i++) {
        int textWidth = textwidth(controls[i]);
        outtextxy((windowWidth - textWidth) / 2, startY + i * lineHeight, controls[i]);
    }

    // 绘制按钮
    drawButton(startBtn);
    drawButton(exitBtn);
}

void drawEndScreen(const Button& restartBtn, const Button& exitBtn, int score) {
    cleardevice();
    putimage(0, 0, &bgImg);
    int windowWidth = BOARD_WIDTH * BLOCK_SIZE + 200;

    settextstyle(60, 0, _T("Lucida Handwriting"));
    settextcolor(RED);
    TCHAR title[20];
    _stprintf_s(title, _T("GAME OVER"));
    outtextxy((windowWidth - textwidth(title)) / 2, 30, title);

    settextstyle(40, 0, _T("Copperplate"));
    settextcolor(WHITE);
    TCHAR scoreText[50];
    _stprintf_s(scoreText, _T("Final Score: %d"), score);
    outtextxy((windowWidth - textwidth(scoreText)) / 2, 200, scoreText);

    drawButton(restartBtn);
    drawButton(exitBtn);
}

void drawPauseScreen(const Button& continueBtn, const Button& restartBtn, const Button& exitBtn) {
    cleardevice();
    putimage(0, 0, &bgImg);
    int windowWidth = BOARD_WIDTH * BLOCK_SIZE + 200;

    settextstyle(60, 0, _T("Lucida Handwriting"));
    settextcolor(YELLOW);
    TCHAR title[20];
    _stprintf_s(title, _T("PAUSED"));
    outtextxy((windowWidth - textwidth(title)) / 2, 100, title);

    // 绘制按钮
    drawButton(continueBtn);
    drawButton(restartBtn);
    drawButton(exitBtn);
}

void* PlaySoundThread(void* unused) {
    while(inGame){
        pthread_mutex_lock(&lock2);
        /*if(isclicked){
            //
            //mciSendString(_T("close anniu"), NULL, 0, NULL);
            
            isclicked=false;
        }*/
        
        // 保持原有音效逻辑
        pthread_cond_wait(&cond2, &lock2);
        button_sound.stop();
        button_sound.play();
        Sleep(100);
        
        pthread_mutex_unlock(&lock2);
    }
    return NULL;
}

void playButtonSound() {
    //isclicked=true;
    pthread_cond_signal(&cond2);
}

