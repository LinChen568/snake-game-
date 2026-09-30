#include <stdio.h>
#include <stdlib.h>   //主要用rand()
#include <time.h>
#include <conio.h>    //主要用_kbhit() _getch()
#include <windows.h>  //主要用Sleep() 函數

#define maxlength 100

// 定義新的類型  蛇身
typedef struct {
    int x, y;
} body;

// 全域變數
body snake[maxlength];
int snakelength = 1; 
char direction = 'd'; // 初始方向定為向右
int width, height;
int foodx, foody, lastfoodx = -1, lastfoody = -1;
int bigfood, bigfoodx, bigfoody, lastbigfoodx= -1, lastbigfoody = -1;
int life = 1;    
int lifex, lifey, lastlifex = -1, lastlifey = -1;;
int reducelength;     
int reducelengthx, reducelengthy, lastreducelengthx = -1, lastreducelengthy = -1;
int boom[100][100] = {0}; // 記錄x炸彈的位置 初始為0 0沒炸彈
int speed;
int score = 0, level = 1 ;  

void information() {
    system("chcp 65001"); // 設置編碼為 UTF-8  //AI說編碼不匹配 用UTF-8才支援中文
    system("cls");      //清空畫面

    printf("遊戲介紹\n\n");
    printf("規則\n\n");
    printf("空白鍵 可加速\n\n");
    printf("F 6分 蛇長度+3\n\n");
    printf("f 4分 蛇長度+1\n\n");
    printf("+     多一命\n\n");
    printf("#     蛇長度減半\n\n");
    printf("吃掉F後地上會留下x\n\n");
    printf("碰到x會扣血但不會死\n\n");
    printf("得十分後進入下一關\n\n");
    printf("禁止上下或左右直接反向移動\n\n");
    printf("輸入地圖大小 (寬 高) 開始遊戲\n\n");

    scanf("%d %d", &width, &height);
}

int repeat(int x, int y) {               //檢查重疊
    for (int i = 0; i < snakelength; i++) {             // 檢查是否與蛇重疊
        if (snake[i].x == x && snake[i].y == y) {
            return 0;      // 位置無效
        }
    }
     
    if (x == foodx && y == foody) {         //// 檢查東西是否與其他物件重疊
        if((x == bigfoodx && y == bigfoody) || (x == lifex && y == lifey) || (x == reducelengthx && y == reducelengthy) || (boom[y][x] == 1)) {
            return 0; // 位置無效
        } else return 1;   // 位置有效
    } else if (x == bigfoodx && y == bigfoody) {
        if((x == foodx && y == foody) || (x == lifex && y == lifey) || (x == reducelengthx && y == reducelengthy || (boom[y][x] == 1))) {
            return 0;// 位置無效
        } else return 1;// 位置有效
    } else if (x == lifex && y == lifey) {
        if ((x == foodx && y == foody) || (x == bigfoodx && y == bigfoody) || (x == reducelengthx && y == reducelengthy) || (boom[y][x] == 1)){
            return 0;// 位置無效
        } else return 1;// 位置有效
    } else if (x == reducelengthx && y == reducelengthy) {
        if ((x == foodx && y == foody) || (x == bigfoodx && y == bigfoody) || (x == lifex && y == lifey) || (boom[y][x] == 1)){
            return 0;// 位置無效
        } else return 1;// 位置有效
    }
}

void newfood() {  // 小食物f出生點
    do {                                        //隨機在地圖內生成小食物f 如過位置重複或和其他東西重疊則重新生成
        foodx = rand() % (width) + 1;      
        foody = rand() % (height) + 1;
    } while (!repeat(foodx, foody) || (foodx == lastfoodx && foody == lastfoody));   

    lastfoodx = foodx;      // 更新上次小食物位置  避免位置重複用
    lastfoody = foody;
}

void newbigfood() {  // 大食物出生點 在地圖邊緣處
    do {                                            //隨機在地圖邊緣五格內生成大食物F如過位置重複或和其他東西重疊則重新生成
        if(rand() % 4 == 0) {
            bigfoodx = (rand() % 5) + 1;
            bigfoody = rand() % (height) + 1;  //左半部五格內          //因為每次rand()都不一樣所以可能沒有生成 那麼那關就不會有F 要等下一關
        } else if (rand() % 4 == 1) {
            bigfoodx = width + 1- rand() % 5;
            bigfoody = rand() % (height) + 1;  //右半部五格內 
        } else if (rand() % 4 == 2) {
            bigfoodx = rand() % (width) + 1;  //上半部五格內   
            bigfoody = (rand() % 5) + 1;
        } else if (rand() % 4 == 3) {
            bigfoodx = rand() % (width) + 1;    //下半部五格內 
            bigfoody = height + 1 - rand() % 5;
        } 
    } while(!repeat(bigfoodx, bigfoody) || (bigfoodx == lastbigfoodx && bigfoody == lastbigfoody));   
    
    lastbigfoodx = bigfoodx;   // 更新上次大食物位置  避免位置重複用
    lastbigfoody = bigfoody;
}

void newlife() {        //生命果實出生點   只出現在左右牆壁邊緣1格
    do {                                                        //隨機在地圖左右邊緣一格內生成生命果實+如過位置重複或和其他東西重疊則重新生成
        if(rand() % 2 == 0) {
            lifex = 1;                          //左半邊
            lifey = rand() % (height) + 1;
        } else {
            lifex = width;                      //右半邊
            lifey = rand() % (height) + 1;
        }

    } while(!repeat(lifex, lifey) || (lifex == lastlifex && lifey== lastlifey));

    lastlifex = lifex;   // 更新上次生命果實位置 避免位置重複用
    lastlifey = lifey;
}

void newreduce() {     //長度縮減器出生點 只出現在四角落
    do {                                                        //隨機在地圖四角落生成長度縮減器 # 如過位置重複或和其他東西重疊則重新生成
         if(rand() % 4 == 0) {   //左上角
            reducelengthx = 1;                           ////因為每次rand()都不一樣所以可能沒有生成 那麼那關就不會有 # 要等下一關
            reducelengthy = 1;
        } else if (rand() % 4 == 1) {    //左下角
            reducelengthx = 1;
            reducelengthy = height;
        } else if (rand() % 4 == 2) {   //右上角
            reducelengthx = width;
            reducelengthy = 1;
        } else if (rand() % 4 == 3) {    //右下角
            reducelengthx = width;
            reducelengthy = height;
        }
    } while(!repeat(reducelengthx, reducelengthy) || (reducelengthx == lastreducelengthx &&  reducelengthy == lastreducelengthy));

    lastreducelengthx = reducelengthx;    // 更新上次長度縮減器位置 避免位置重複用
    lastreducelengthy = reducelengthy;
}

void initializegame() {         // 遊戲初始設定

    snake[0].x = 3;             // 蛇出生點
    snake[0].y = 3;

    srand(time(NULL));         //將當前時間用作隨機數生成器的種子

    newfood();
    newbigfood();
    newlife();
    newreduce();
}

// 地圖設定
void map() {
    system("cls"); // 清空終端
    for (int i = 0; i < height + 2; i++) {
        for (int j = 0; j < width + 2; j++) {
            if (i == 0 || i == height + 1 || j == 0 || j == width + 1) {
                printf("*"); // 牆壁
            } else if (i == foody && j == foodx) {
                printf("f"); // food  
            } else if (i == bigfoody && j == bigfoodx){
                printf("F"); //FOOD
            } else if (i == lifey && j == lifex) {
                printf("+"); //生命果實 +
            } else if (i ==reducelengthy && j == reducelengthx ) {
                printf("#"); //長度縮減器 #
            } else if (boom[i][j] == 1) {
                printf("x"); //  炸彈 x
            } else {
                int issnake = 0;
                for (int k = 0; k < snakelength; k++) {
                    if (snake[k].x == j && snake[k].y == i) {
                        printf("O"); // 蛇的身體
                        issnake = 1;
                        break;
                    }
                }
                if (issnake == 0) {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

// 蛇的移動
void move() {
    for (int i = snakelength - 1; i > 0; i--) {     //蛇把前面位置給後面位置
        snake[i] = snake[i - 1];
    }

    //蛇頭位置移動
    switch (direction) {
        case 'w': snake[0].y--; break;   //螢幕的左上角是 (0, 0)，這是座標的原點 所以W是Y--所以W是Y--
        case 's': snake[0].y++; break;
        case 'a': snake[0].x--; break;
        case 'd': snake[0].x++; break;
    }

    if (snake[0].x == foodx && snake[0].y == foody) {               // 蛇吃到f
        snakelength++;
        score+=4;
        if (snakelength > maxlength) {
             snakelength = maxlength; //蛇最長100 不能超過
        } 
       
        newfood();

    } else if (snake[0].x == bigfoodx && snake[0].y == bigfoody){       // 蛇吃到F
        snakelength+=3;
        score+=6;
        if (snakelength > maxlength) {
             snakelength = maxlength; //蛇最長100 不能超過
        }

          boom[bigfoody][bigfoodx] = 1;         //// 蛇吃到F後原地留下炸彈

        newbigfood();
        
    } else if (snake[0].x == lifex && snake[0].y == lifey) {      //蛇吃到生命果實
        life++;
       
        newlife();

    } else if (snake[0].x == reducelengthx && snake[0].y == reducelengthy) {         //蛇吃到長度縮減器
        snakelength = snakelength/2;  //蛇變一半  無條件捨去因為都是int
        if (snakelength < 1) {
            snakelength = 1; //蛇最短1 不能少於1
        }

         newreduce();

    } else if (boom[snake[0].y][snake[0].x] == 1) {     ///蛇吃到炸彈x 扣一條命但不會死
        if (life >0 ){
            life--; // 減少生命值
        }
    }
}

void secondinformation() {            //遊戲過程中的資訊顯示
    printf("    life = %d\n",life); 
    printf("    score = %d\n",score);
}


int gameover() {                // 遊戲結束判定
    // 檢查是否撞牆
    if (snake[0].x <= 0 || snake[0].x >= width + 1 || snake[0].y <= 0 || snake[0].y >= height + 1) {
        return 1;
    }

    // 檢查是否自撞
    for (int i = 1; i < snakelength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1;
        }
    }
    return 0;
}

void new () {
     // 死後蛇新出生點
    system("cls");         //清空畫面

    printf("生命值扣1 左上初始點重生");

    direction = 'd';    //定義新出來的方向固定向右

     Sleep(2000); // 暫停 2 秒
     
    snake[0].x = 3;   //復活後重(3,3)開始
    snake[0].y = 3;
}

void nextlevel() {
    level++;

    system("cls");

    printf("進入第 %d 關\n 速度提升",level);

    newbigfood();       //關卡重製時重新生成 F #   因為不一定會生成成功 
    newreduce();        //這樣可以讓有些關卡有 有些關卡沒有

    Sleep(2000); // 暫停 2 秒

}

int main() {

    information();
    
    if (width < 5) {
        printf("寬度太小要大於5\n");
        return 1;
    }
    
    if (height < 5) {
        printf("高度太小要大於5\n");
        return 1;
    }

    initializegame();

    while (1) {
        map();

        // 輸入方向
        if (_kbhit()) {    //_kbhit()檢查鍵盤輸入
            char newdirection = _getch();    // _getch()從鍵盤中讀取一個字符不顯示在螢幕上
            if ((newdirection == 'w' || newdirection == 'a' || newdirection == 's' || newdirection == 'd') &&
                (abs(direction - newdirection) != 2)) { // 禁止直接反方向    //abs()計算絕對值
                direction = newdirection;
            }else if (newdirection == ' ') {   //輸入空白間改變speed模式
                speed = 1;
            }
        }

        move();

        secondinformation();

        if (gameover()) {
            if(life > 0) {
                life--;
                new(); // 重置蛇的位置，但保持生命值
                continue;
            } else {
                printf("Game Over!\n");
                break;
            }
        }
        if(score >= level * 10) {    //得十分進入下一關
            nextlevel();
        }
        if (speed) {        //按空白鍵後速度變2倍
            Sleep(200 / level);
            speed = 0;
        }else   Sleep(400 / level);
        
    }

    return 0;
}
