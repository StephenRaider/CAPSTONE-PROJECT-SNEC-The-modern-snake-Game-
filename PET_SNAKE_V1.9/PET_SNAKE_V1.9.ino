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

// Game variables
int snakeX[100], snakeY[100], numSegments;
int appleX, appleY;
int dirX, dirY;

// Grid size
int gridSize = 10;

void setup() {
  uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
  startGame();
}

void loop() {
  int oldTailX = snakeX[numSegments - 1];
  int oldTailY = snakeY[numSegments - 1];

  moveSnake();

  if (snakeX[0] == appleX && snakeY[0] == appleY) eatApple();
  
  if (checkCollision()) startGame();

  // Erase old tail position
  tft.fillRect(oldTailX, oldTailY, gridSize, gridSize, TFT_BLACK);

  drawSnake();
  
  delay(100);
}

void startGame() {
  numSegments = 1;
  snakeX[0] = (tft.width() / gridSize) / 2 * gridSize;
  snakeY[0] = (tft.height() / gridSize) / 2 * gridSize;
  
  spawnApple();
}

void spawnApple() {
  appleX = random(tft.width() / gridSize) * gridSize;
  appleY = random(tft.height() / gridSize) * gridSize;
  
  // Draw apple
  tft.fillRect(appleX, appleY, gridSize, gridSize, TFT_RED);
}

void moveSnake() {
  // Simple AI: move in the direction of the apple
  dirX = (appleX > snakeX[0]) ? gridSize : (appleX < snakeX[0]) ? -gridSize : 0;
  dirY = (appleY > snakeY[0]) ? gridSize : (appleY < snakeY[0]) ? -gridSize : 0;

  // Shift segments down the array
  for (int i=numSegments-1; i>0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }

  // Move head
  snakeX[0] += dirX;
  snakeY[0] += dirY;
}

bool checkCollision() {
    for(int i=1; i<numSegments; i++) {
        if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            return true;
        }
    }
    return false;
}

void eatApple() {
    // Add new segment at position offset from head
    snakeX[numSegments] = snakeX[numSegments-1] - dirX;
    snakeY[numSegments] = snakeY[numSegments-1] - dirY;

    numSegments++;
    
    spawnApple();
}

void drawSnake() {
    // Draw new head position
    tft.fillRect(snakeX[0],snakeY[0],gridSize,gridSize,TFT_BLUE);
    
    // Draw rest of the body
    for (int i=1; i<numSegments; i++) {
        tft.fillRect(snakeX[i],snakeY[i],gridSize,gridSize,TFT_GREEN);
    }
}
