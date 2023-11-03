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
int snakeX1[100], snakeY1[100], numSegments1;
int snakeX2[100], snakeY2[100], numSegments2;
int appleX1, appleY1, appleX2, appleY2;
int dirX1, dirY1, dirX2, dirY2;
int score1 = 0, score2 = 0; // Add a score counter for each player
int highScore1 = 0, highScore2 = 0; // Add a high score tracker for each player
bool gameStarted1 = false, gameStarted2 = false; // Add a game started flag for each player

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
    if (gameStarted1) {
        int oldTailX1 = snakeX1[numSegments1 - 1];
        int oldTailY1 = snakeY1[numSegments1 - 1];

        moveSnake1();

        if (snakeX1[0] == appleX1 && snakeY1[0] == appleY1) eatApple1(score1, highScore1);
        
        if (checkCollision(snakeX1, snakeY1, numSegments1)) {
            gameOver(1);
            startGame();
        }

        // Erase old tail position
        tft.fillRect(oldTailX1, oldTailY1, gridSize, gridSize, TFT_BLACK);

        drawSnake(snakeX1, snakeY1, numSegments1, TFT_BLUE, TFT_GREEN);
        
        delay(150);
    } else {
        if (Serial.available() > 0) {
            char key = Serial.read(); // Read the incoming byte
            if (key == 'w') {
                gameStarted1 = true;
                startGame();
            }
        }
    }

    if (gameStarted2) {
        int oldTailX2 = snakeX2[numSegments2 - 1];
        int oldTailY2 = snakeY2[numSegments2 - 1];

        moveSnake2();

        if (snakeX2[0] == appleX2 && snakeY2[0] == appleY2) eatApple2(score2, highScore2);
        
        if (checkCollision(snakeX2, snakeY2, numSegments2)) {
            gameOver(2);
            startGame();
        }

        // Erase old tail position
        tft.fillRect(oldTailX2, oldTailY2, gridSize, gridSize, TFT_BLACK);

        drawSnake(snakeX2, snakeY2, numSegments2, TFT_PURPLE, TFT_PINK);
        
        delay(150);
    } else {
        gameStarted2 = true;
        startGame();
    }
}

void clearScreen() {
    tft.fillScreen(TFT_BLACK);
}

void startGame() {
    clearScreen();
    numSegments1 = 3;

    for (int i = 0; i < numSegments1; i++) {
        snakeX1[i] = (tft.width() / gridSize) / 4 * gridSize - i * gridSize;
        snakeY1[i] = (tft.height() / gridSize) / 4 * gridSize;
    }

    score1 = 0; // Reset the score when a new game starts

    dirX1 = gridSize; // Set initial direction of motion to right
    dirY1 = 0;

    numSegments2 = 3;

    for (int i = 0; i < numSegments2; i++) {
        
        snakeX2[i] = (tft.width() / gridSize) * 3/4 * gridSize - i * gridSize;
        snakeY2[i] = (tft.height() / gridSize) / 4 * gridSize;
    }

    score2 = 0; // Reset the score when a new game starts

    dirX2 = gridSize; // Set initial direction of motion to right
    dirY2 = 0;

    spawnApple(appleX1, appleY1);
    spawnApple(appleX2, appleY2);
}

void spawnApple(int &appleX, int &appleY) {
    bool appleInSnake;
    do {
      appleInSnake = false;
      appleX = random(tft.width() / gridSize) * gridSize;
      appleY = random(tft.height() / gridSize) * gridSize;

      // Check if the apple is inside the snake's body
      for (int i = 0; i < numSegments1; i++) {
          if ((snakeX1[i] == appleX && snakeY1[i] == appleY) || (snakeX2[i] == appleX && snakeY2[i] == appleY)) {
              appleInSnake = true;
              break;
          }
      }

      for (int i = 0; i < numSegments2; i++) {
          if ((snakeX1[i] == appleX && snakeY1[i] == appleY) || (snakeX2[i] == appleX && snakeY2[i] == appleY)) {
              appleInSnake = true;
              break;
          }
      }
    } while (appleInSnake); 

    // Draw apple
    tft.fillRect(appleX, appleY, gridSize, gridSize, TFT_RED);
}

void moveSnake1() {
    // Check if data is available to read
    if (Serial.available() > 0) {
        char key = Serial.read(); 

        // Update direction based on key press
        switch (key) {
            case 'w':
            case 'i': 
                if (dirY1 != gridSize) { 
                    dirX1 = 0;
                    dirY1 = -gridSize;
                }
                break;
            case 'a':
            case 'j': 
                if (dirX1 != gridSize) {
                    dirX1 = -gridSize;
                    dirY1 = 0;
                }
                break;
            case 's':
            case 'k': 
                if (dirY1 != -gridSize) {
                    dirX1 = 0;
                    dirY1 = gridSize;
                }
                break;
            case 'd':
            case 'l': 
                if (dirX1 != -gridSize) {
                    dirX1 = gridSize;
                    dirY1 = 0;
                }
                break;
        }
    }

    // Shift segments down the array
    for (int i=numSegments1-1; i>0; i--) {
        snakeX1[i] = snakeX1[i-1];
        snakeY1[i] = snakeY1[i-1];
    }

    // Move head
    snakeX1[0] += dirX1;
    snakeY1[0] += dirY1;

    // Check if out of bounds
    if (snakeX1[0] < 0 || snakeX1[0] >= tft.width()/2 || snakeY1[0] < 0 || snakeY1[0] >= tft.height()) {
        gameOver(1);
        startGame();
    }
}

void moveSnake2() {
    // Calculate direction towards the apple
    int targetDirX = 0;
    int targetDirY = 0;
    if (appleX2 > snakeX2[0]) targetDirX = gridSize;
    else if (appleX2 < snakeX2[0]) targetDirX = -gridSize;
    else if (appleY2 > snakeY2[0]) targetDirY = gridSize;
    else if (appleY2 < snakeY2[0]) targetDirY = -gridSize;

    // Try to move in the calculated direction
    if (targetDirX != 0 && dirX2 != -targetDirX) {
        dirX2 = targetDirX;
        dirY2 = 0;
    } else if (targetDirY != 0 && dirY2 != -targetDirY) {
        dirX2 = 0;
        dirY2 = targetDirY;
    }

    // Shift segments down the array
    for (int i=numSegments2-1; i>0; i--) {
        snakeX2[i] = snakeX2[i-1];
        snakeY2[i] = snakeY2[i-1];
    }

    // Move head
    snakeX2[0] += dirX2;
    snakeY2[0] += dirY2;

    // Check if out of bounds
    if (snakeX2[0] < tft.width()/2 || snakeX2[0] >= tft.width() || snakeY2[0] < 0 || snakeY2[0] >= tft.height()) {
        gameOver(2);
        startGame();
    }
}

bool checkCollision(int snakeX[], int snakeY[], int numSegments) {
    for(int i=1; i<numSegments; i++) {
        if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            return true;
        }
    }
    return false;
}

void eatApple1(int &score, int &highScore) {
  // Add new segment at position offset from head
  snakeX1[numSegments1] = snakeX1[numSegments1-1] - dirX1;
  snakeY1[numSegments1] = snakeY1[numSegments1-1] - dirY1;

  numSegments1++;

  score++; // Increase the score when an apple is eaten
  if (score > highScore) { // Update the high score if necessary
  highScore = score;
  }

  spawnApple(appleX1, appleY1);
}

void eatApple2(int &score, int &highScore) {
  // Add new segment at position offset from head
  snakeX2[numSegments2] = snakeX2[numSegments2-1] - dirX2;
  snakeY2[numSegments2] = snakeY2[numSegments2-1] - dirY2;

  numSegments2++;

  score++; // Increase the score when an apple is eaten
  if (score > highScore) { // Update the high score if necessary
  highScore = score;
  }

  spawnApple(appleX2, appleY2);
}

void drawSnake(int snakeX[], int snakeY[], int numSegments, uint16_t headColor, uint16_t bodyColor) {
    // Draw new head position
    tft.fillRect(snakeX[0],snakeY[0],gridSize,gridSize,headColor);

    // Draw rest of the body
    for (int i=1; i<numSegments; i++) {
        tft.fillRect(snakeX[i],snakeY[i],gridSize,gridSize,bodyColor);
    }
}

void gameOver(int player) {
    // Display game over screen
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(tft.width()/2 - 50, tft.height()/2);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.println("GAME OVER");

    // Display the score and high score
    tft.setCursor(tft.width()/2 - 50, tft.height()/2 + 20);
    
    if(player == 1){
        tft.println("Player 1 Score: " + String(score1));
        tft.setCursor(tft.width()/2 - 50, tft.height()/2 + 40);
        tft.println("Player 1 High Score: " + String(highScore1));
        
        // Turn player 1's half of the screen white
        tft.fillRect(0, 0, tft.width()/2, tft.height(), TFT_WHITE);
        
        // Display that player 1 died
        tft.setCursor(tft.width()/4 - 50, tft.height()/2);
        tft.setTextColor(TFT_RED);
        tft.setTextSize(2);
        tft.println("Player 1 Died");
        
        gameStarted1 = false; // Set game started flag to false for player 1
     } else{
         tft.println("Player 2 Score: " + String(score2));
         tft.setCursor(tft.width()/2 - 50, tft.height()/2 + 40);
         tft.println("Player 2 High Score: " + String(highScore2));
         
         // Turn player 2's half of the screen white
         tft.fillRect(tft.width()/2, 0, tft.width(), tft.height(), TFT_WHITE);
         
         // Display that player 2 died
         tft.setCursor(3*tft.width()/4 - 50, tft.height()/2);
         tft.setTextColor(TFT_RED);
         tft.setTextSize(2);
         tft.println("Player 2 Died");
         
         gameStarted2 = xfalse; // Set game started flag to false for player 2
     }

     delay(3000);
}

void showStartScreen() {
tft.fillScreen(TFT_WHITE);
tft.setCursor(0, 0);
tft.setTextColor(TFT_BLACK);
tft.setTextSize(2);
tft.println("SNEC ver-2 alpha 1.0");
tft.println("By Jyotiraditya Bag");
tft.setCursor(0, tft.height()/2);
tft.setTextColor(TFT_BLACK);
tft.setTextSize(2);
tft.println("Press 'w' to start");
}
