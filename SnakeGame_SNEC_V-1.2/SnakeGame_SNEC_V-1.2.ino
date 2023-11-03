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
int score = 0; // Add a score counter
int highScore = 0; // Add a high score tracker

bool gameStarted = false; // Add a game started flag

// Grid size
int gridSize = 10;

void setup() {
Serial.begin(9600); // Start serial communication at 9600 baud
uint16_t ID = tft.readID();
tft.begin(ID);
tft.setRotation(1);
clearScreen();
showStartScreen(); // Show the start screen instead of starting the game
}

void loop() {
if (gameStarted) {
    int oldTailX = snakeX[numSegments - 1];
    int oldTailY = snakeY[numSegments - 1];

    moveSnake();

    if (snakeX[0] == appleX && snakeY[0] == appleY) eatApple();
    
    if (checkCollision()) {
        startGame();
    }

    // Erase old tail position
    tft.fillRect(oldTailX, oldTailY, gridSize, gridSize, TFT_BLACK);

    drawSnake();
    
    delay(150);
} else {
    if (Serial.available() > 0) {
        char key = Serial.read(); // Read the incoming byte
        if (key == 'w') {
            gameStarted = true;
            startGame();
        }
    }
}
}

void clearScreen() {
tft.fillScreen(TFT_BLACK);
}

void startGame() {
  clearScreen();
  numSegments = 3;

  for (int i = 0; i < numSegments; i++) {
      snakeX[i] = (tft.width() / gridSize) / 2 * gridSize - i * gridSize;
      snakeY[i] = (tft.height() / gridSize) / 2 * gridSize;
  }

  score = 0; // Reset the score when a new game starts

  dirX = gridSize; // Set initial direction of motion to right
  dirY = 0;

  spawnApple();
}

void spawnApple() {
appleX = random(tft.width() / gridSize) * gridSize;
appleY = random(tft.height() / gridSize) * gridSize;

// Draw apple
tft.fillRect(appleX, appleY, gridSize, gridSize, TFT_RED);
}

void moveSnake() {
// Check if data is available to read
if (Serial.available() > 0) {
    char key = Serial.read(); // Read the incoming byte

    // Update direction based on key press
    switch (key) {
        case 'w':
        case 'i': // Up arrow
            if (dirY != gridSize) { // Prevent moving directly backwards
                dirX = 0;
                dirY = -gridSize;
            }
            break;
        case 'a':
        case 'j': // Left arrow
            if (dirX != gridSize) {
                dirX = -gridSize;
                dirY = 0;
            }
            break;
        case 's':
        case 'k': // Down arrow
            if (dirY != -gridSize) {
                dirX = 0;
                dirY = gridSize;
            }
            break;
        case 'd':
        case 'l': // Right arrow
            if (dirX != -gridSize) {
                dirX = gridSize;
                dirY = 0;
            }
            break;
    }
}

// Shift segments down the array
for (int i=numSegments-1; i>0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
}

// Move head
snakeX[0] += dirX;
snakeY[0] += dirY;

// Check if out of bounds
if (snakeX[0] < 0 || snakeX[0] >= tft.width() || snakeY[0] < 0 || snakeY[0] >= tft.height()) {
    gameOver();
    startGame();
}
}

bool checkCollision() {
for(int i=1; i<numSegments; i++) {
    if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
        gameOver();
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

score++; // Increase the score when an apple is eaten
if (score > highScore) { // Update the high score if necessary
    highScore = score;
}

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

void gameOver() {
// Display game over screen
tft.fillScreen(TFT_BLACK);
tft.setCursor(tft.width()/2 - 50, tft.height()/2);
tft.setTextColor(TFT_RED);
tft.setTextSize(2);
tft.println("GAME OVER");

// Display the score and high score
tft.setCursor(tft.width()/2 - 50, tft.height()/2 + 20);
tft.println("Score: " + String(score));
tft.setCursor(tft.width()/2 - 50, tft.height()/2 + 40);
tft.println("High Score: " + String(highScore));

delay(3000);

clearScreen();
}

void showStartScreen() {
tft.fillScreen(TFT_WHITE);
tft.setCursor(0, 0);
tft.setTextColor(TFT_BLACK);
tft.setTextSize(2);
tft.println("SNEC Ver. 1.2");
tft.println("By Jyotiraditya Bag");
tft.setCursor(0, tft.height()/2);
tft.setTextColor(TFT_BLACK);
tft.setTextSize(2);
tft.println("Press 'w' to start");
}
