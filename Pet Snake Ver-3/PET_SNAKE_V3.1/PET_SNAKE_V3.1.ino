#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_CYAN    0x07FF
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW  0xFFE0  
#define TFT_WHITE   0xFFFF
#define TFT_VIOLET  0x780F

// Game variables for snake1
int snake1X[100], snake1Y[100], numSegments1;
int dir1X, dir1Y;

// Game variables for snake2
int snake2X[100], snake2Y[100], numSegments2;
int dir2X, dir2Y;

// Variables for two apples
int apple1X, apple1Y;
int apple2X, apple2Y;

// Grid size
int gridSize = 10;

void setup() {
  uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(1);
    clearScreen();
    startGame();
}

void loop() {
  int oldTail1X = snake1X[numSegments1 - 1];
  int oldTail1Y = snake1Y[numSegments1 - 1];

  int oldTail2X = snake2X[numSegments2 - 1];
  int oldTail2Y = snake2Y[numSegments2 - 1];

  moveSnake(snake1X, snake1Y, &dir1X, &dir1Y, &numSegments1, apple1X, apple1Y, apple2X, apple2Y);
  moveSnake(snake2X, snake2Y, &dir2X, &dir2Y, &numSegments2, apple1X, apple1Y, apple2X, apple2Y);

  if ((snake1X[0] == apple1X && snake1Y[0] == apple1Y) || (snake2X[0] == apple1X && snake2Y[0] == apple1Y)) {
    eatApple(snake1X, snake1Y, &numSegments1);
    eatApple(snake2X, snake2Y, &numSegments2);
    spawnApple(&apple1X,&apple1Y);
  }

  if ((snake1X[0] == apple2X && snake1Y[0] == apple2Y) || (snake2X[0] == apple2X && snake2Y[0] == apple2Y)) {
    eatApple(snake1X, snake1Y, &numSegments1);
    eatApple(snake2X, snake2Y, &numSegments2);
    spawnApple(&apple2X,&apple2Y);
  }

  
  if (checkCollision(snake1X, snake1Y, numSegments1)) {
    clearScreen();
    startGame();
  }

  if (checkCollision(snake2X, snake2Y, numSegments2)) {
    clearScreen();
    startGame();
  }

  // Erase old tail positions
  tft.fillRect(oldTail1X, oldTail1Y, gridSize, gridSize, TFT_BLACK);
  tft.fillRect(oldTail2X, oldTail2Y, gridSize, gridSize, TFT_BLACK);

  
  drawSnake(snake1X, snake1Y, numSegments1,TFT_BLUE,TFT_GREEN);
  
  drawSnake(snake2X,snake2Y,numSegments2,TFT_VIOLET,TFT_MAGENTA);

  
  delay(100);
}

void clearScreen() {
    tft.fillScreen(TFT_BLACK);
}

void startGame() {
  
   numSegments1 = numSegments2 = 3;
  
   // Initialize the positions of the snakes to be in the middle of the screen
   for(int i=0;i<numSegments1;i++){
       snake1X[i] = (tft.width() / gridSize) /4 * gridSize;
       snake1Y[i] = ((tft.height() / gridSize) /4 + i) * gridSize;
   }
   for(int i=0;i<numSegments2;i++){    
       snake2X[i] = (tft.width() / gridSize) *3/4 * gridSize;
       snake2Y[i] = ((tft.height() / gridSize) *3/4 + i) * gridSize;
   }
   
   spawnApple(&apple1X,&apple1Y);
   spawnApple(&apple2X,&apple2Y);
}

void spawnApple(int* appleX,int* appleY) {
    *appleX = random(tft.width() / gridSize) * gridSize;
    *appleY = random(tft.height() / gridSize) * gridSize;
    
    // Draw apple
    tft.fillRect(*appleX, *appleY, gridSize, gridSize, TFT_RED);
}

void moveSnake(int* snakeX,int* snakeY,int* dirX,int* dirY,int* numSegments, int apple1X, int apple1Y, int apple2X, int apple2Y) {
    // Simple AI: move in the direction of the closest apple
    int dist1 = abs(apple1X - *snakeX) + abs(apple1Y - *snakeY);
    int dist2 = abs(apple2X - *snakeX) + abs(apple2Y - *snakeY);
    
    if (dist1 < dist2) {
        *dirX = (apple1X > *snakeX) ? gridSize : (apple1X < *snakeX) ? -gridSize : 0;
        *dirY = (apple1Y > *snakeY) ? gridSize : (apple1Y < *snakeY) ? -gridSize : 0;
    } else {
        *dirX = (apple2X > *snakeX) ? gridSize : (apple2X < *snakeX) ? -gridSize : 0;
        *dirY = (apple2Y > *snakeY) ? gridSize : (apple2Y < *snakeY) ? -gridSize : 0;
    }

    // Shift segments down the array
    for (int i=*numSegments-1; i>0; i--) {
        *(snakeX+i) = *(snakeX+i-1);
        *(snakeY+i) = *(snakeY+i-1);
    }

    // Move head
    *snakeX += *dirX;
    *snakeY += *dirY;
}

bool checkCollision(int* snakeX, int* snakeY, int numSegments) {
    for(int i=1; i<numSegments; i++) {
        if(*snakeX == *(snakeX+i) && *snakeY == *(snakeY+i)) {
            return true;
        }
    }
    return false;
}

void eatApple(int* snakeX, int* snakeY, int* numSegments) {
    // Add new segment at position offset from head
    *(snakeX+*numSegments) = *(snakeX+*numSegments-1);
    *(snakeY+*numSegments) = *(snakeY+*numSegments-1);

    (*numSegments)++;
}

void drawSnake(int* snakeX, int* snakeY, int numSegments,uint16_t headColor,uint16_t bodyColor) {
    // Draw new head position
    tft.fillRect(*snakeX,*snakeY,gridSize,gridSize,headColor);
    
    // Draw rest of the body
    for (int i=1; i<numSegments; i++) {
        tft.fillRect(*(snakeX+i),*(snakeY+i),gridSize,gridSize,bodyColor);
    }
}
