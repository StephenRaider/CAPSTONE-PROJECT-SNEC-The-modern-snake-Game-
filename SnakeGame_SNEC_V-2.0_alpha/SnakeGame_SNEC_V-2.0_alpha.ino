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
int snake1X[200], snake1Y[200], numSegments1;
int dir1X, dir1Y;
int target1  = 0; // 0 for apple1, 1 for apple2
int score1 = 0;
int totalScore1 = 0;

// Game variables for snake2
int snake2X[200], snake2Y[200], numSegments2;
int dir2X, dir2Y;
int target2 = 1; // 0 for apple1, 1 for apple2
int score2 = 0;
int totalScore2 = 0;

//game counter
int gameCount = 0;

// Variables for two apples
int apple1X, apple1Y;
int apple2X, apple2Y;


// Grid size
int gridSize = 10;

void versionInfo()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,10);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.println("Snek game 2.0 alpha build");
  tft.println(" ");
  tft.println("Split Screen");
  tft.println("More logic improvments");
  tft.println(" ");
  tft.println("By team");
  delay(5000);

}
void setup() 
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(1);
    clearScreen();
    versionInfo();
    clearScreen();
    randomSeed(analogRead(0));
    startGame();
}

void loop() 
{
  int oldTail1X = snake1X[numSegments1 - 1];
  int oldTail1Y = snake1Y[numSegments1 - 1];

  int oldTail2X = snake2X[numSegments2 - 1];
  int oldTail2Y = snake2Y[numSegments2 - 1];

  moveSnake1();
  
  moveSnake2();

  
  if (snake1X[0] == apple1X && snake1Y[0] == apple1Y) {
    eatApple(snake1X, snake1Y, &numSegments1, &target1, &score1);
    spawnApple(&apple1X,&apple1Y, 0, tft.width()/2);
  }

  
  if (snake1X[0] == apple2X && snake1Y[0] == apple2Y) {
    eatApple(snake1X, snake1Y, &numSegments1, &target1, &score1);
    spawnApple(&apple2X,&apple2Y, tft.width()/2, tft.width());
  }

    if (snake2X[0] == apple1X && snake2Y[0] == apple1Y) {
    eatApple(snake2X, snake2Y, &numSegments2, &target2, &score2);
    spawnApple(&apple1X,&apple1Y, 0, tft.width()/2);
  }

  
  if (snake2X[0] == apple2X && snake2Y[0] == apple2Y) {
    eatApple(snake2X, snake2Y, &numSegments2, &target2, &score2);
    spawnApple(&apple2X,&apple2Y, tft.width()/2, tft.width());
  }

  
  if (checkCollision(snake1X, snake1Y,numSegments1)) {
    score1 -= 20;// lose points on dying
    totalScore1 += score1;
    totalScore2 += score2;
    gameOver("GREEN SNAKE", TFT_GREEN, score1, score2, totalScore1, totalScore2, gameCount);
    clearScreen();
    startGame();
  }

  
  if (checkCollision(snake2X,snake2Y,numSegments2)) {
    score2 -= 20;// lose points on dying
    totalScore1 += score1;
    totalScore2 += score2;
    gameOver("PINK SNAKE", TFT_MAGENTA, score1, score2, totalScore1, totalScore2, gameCount);
    clearScreen();
    startGame();
  }

  
  // Erase old tail positions
  tft.fillRect(oldTail1X, oldTail1Y, gridSize, gridSize,TFT_BLACK);
  
  tft.fillRect(oldTail2X, oldTail2Y, gridSize, gridSize,TFT_BLACK);

  
  drawSnake(snake1X,snake1Y,numSegments1,TFT_BLUE,TFT_GREEN);
  
   
  drawSnake(snake2X,snake2Y,numSegments2,TFT_VIOLET,TFT_MAGENTA);

  tft.fillRect(tft.width()/2 -1, 0, 1, tft.height(), TFT_WHITE);

   
  delay(50);
}

void clearScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(tft.width()/2 -1, 0, 1, tft.height(), TFT_WHITE);
}

void startGame() {
  
   numSegments1 = numSegments2 =3;
   
   // Initialize the positions of the snakes to be in the middle of the screen
   for(int i=0;i<numSegments1;i++){
       snake1X[i] = (tft.width() / gridSize) /4 * gridSize;
       snake1Y[i] = ((tft.height() / gridSize) /2 + i) * gridSize;
   }
   for(int i=0;i<numSegments2;i++){    
       snake2X[i] = (tft.width() / gridSize) *3/4 * gridSize;
       snake2Y[i] = ((tft.height() / gridSize) /2 + i) * gridSize;
   }
   
   spawnApple(&apple1X,&apple1Y, 0, tft.width()/2);
   
   spawnApple(&apple2X,&apple2Y, tft.width()/2, tft.width());


   //reset scores for new game
   score1 = score2 = 0;

   //increase game counter
   gameCount++;
}

void spawnApple(int* appleX,int* appleY, int minX, int maxX) {
    *appleX = random(minX / gridSize, maxX / gridSize) * gridSize;
    *appleY = random(tft.height() / gridSize) * gridSize;
    
    // Draw apple
    tft.fillRect(*appleX, *appleY, gridSize, gridSize, TFT_RED);
}
void moveSnake1() {
  // Check if data is available to read
  if (Serial.available() > 0) 
  {
      char key = Serial.read(); // Read the incoming byte

      // Update direction based on key press
      switch (key) {
          case 'w':
          case 'i': // Up arrow
              if (dir1Y != gridSize) { // Prevent moving directly backwards
                  dir1X = 0;
                  dir1Y = -gridSize;
              }
              break;
          case 'a':
          case 'j': // Left arrow
              if (dir1X != gridSize) {
                  dir1X = -gridSize;
                  dir1Y = 0;
              }
              break;
          case 's':
          case 'k': // Down arrow
              if (dir1Y != -gridSize) {
                  dir1X = 0;
                  dir1Y = gridSize;
              }
              break;
          case 'd':
          case 'l': // Right arrow
              if (dir1X != -gridSize) {
                  dir1X = gridSize;
                  dir1Y = 0;
              }
              break;
      }
  }

  // Shift segments down the array
  for (int i=numSegments1-1; i>0; i--) 
  {
      snake1X[i] = snake1X[i-1];
      snake1Y[i] = snake1Y[i-1];
  }

  // Move head
  snake1X[0] += dir1X;
  snake1Y[0] += dir2Y;


}

void moveSnake2() 
{
    // Simple AI: stick to chosen apple until it is eaten
    int newDirX = dir2X;
    int newDirY = dir2Y;
    
    if (apple1X != snake2X[0] && apple1Y != snake2Y[0]) 
    {
            newDirX = (apple2X > snake2X[0]) ? gridSize : -gridSize;
            newDirY = (apple2Y > snake2Y[0]) ? gridSize : -gridSize;
    } 
    else if (apple1X != snake2X[0]) 
    {
            newDirX = (apple2X > snake2X[0]) ? gridSize : -gridSize;
            newDirY = 0;
    } 
    else if (apple1Y != snake2Y[0]) 
    {
            newDirY = (apple2Y > snake2Y[0]) ? gridSize : -gridSize;
            newDirX = 0;
    }    
    


    // Check for collisions in the new direction
    int newX = snake2X[0] + newDirX;
    int newY = snake2Y[0] + newDirY;
    bool collision = false;
    for(int i=0; i<numSegments2; i++) {
        if(newX == (snake2X[0]+i) && newY == (snake2Y[0]+i)) {
            collision = true;
            break;
        }
    }

    // If a collision is detected, try turning left or right
    if (collision) {
        // Try turning right
        int temp = newDirX;
        newDirX = -newDirY;
        newDirY = temp;

        newX = snake2X[0] + newDirX;
        newY = snake2Y[0] + newDirY;

        for(int i=0; i<numSegments2; i++) {
            if(newX == (snake2X[0]+i) && newY == (snake2Y[0]+i)) {
                // If there's still a collision, turn in the other direction
                newDirX = -newDirX;
                newDirY = -newDirY;

                newX = snake2X[0] + newDirX;
                newY = snake2Y[0] + newDirY;
                break;
            }
        }
    }

    // Check if the new position is out of bounds
    if (newX < (tft.width()/2) || newX >= tft.width() || newY < 0 || newY >= tft.height()) {
        // If out of bounds, try turning right
        int temp = newDirX;
        newDirX = -newDirY;
        newDirY = temp;

        newX = snake2X[0] + newDirX;
        newY = snake2Y[0] + newDirY;

        if (newX < (tft.width()/2) || newX >= tft.width() || newY < 0 || newY >= tft.height()) {
            // If still out of bounds, try turning into other direction
            newDirX = -newDirX;
            newDirY = -newDirY;  
      }    
    }

    // Update direction
    dir2X = newDirX;
    dir2Y = newDirY;

    // Shift segments down the array
    for (int i=numSegments2-1; i>0; i--) 
    {
      snake2X[i] = snake2X[i-1];
      snake2Y[i] = snake2Y[i-1];
    }

    // Move head
    snake2X[0] += dir2X;
    snake2Y[0] += dir2Y;
}




bool checkCollision(int* snakeX, int* snakeY, int numSegments) {
    for(int i=1; i<numSegments; i++) {
        if(*snakeX == *(snakeX+i) && *snakeY == *(snakeY+i)) {
            return true;
        }
    }
    return false;
}

void eatApple(int* snakeX, int* snakeY, int* numSegments,int* target, int* score) {
    // Add new segment at position offset from head
    *(snakeX+*numSegments) = *(snakeX+*numSegments-1);
    *(snakeY+*numSegments) = *(snakeY+*numSegments-1);

    (*numSegments)++;

    *score += 10; //gain points by eating apple
    
    // Choose new target apple
    //*target = random(2);
}

void drawSnake(int* snakeX, int* snakeY, int numSegments,uint16_t headColor,uint16_t bodyColor) {
    // Draw new head position
    tft.fillRect(*snakeX,*snakeY,gridSize,gridSize,headColor);
    
    // Draw rest of the body
    for (int i=1; i<numSegments; i++) {
        tft.fillRect(*(snakeX+i),*(snakeY+i),gridSize,gridSize,bodyColor);
    }
}

void gameOver(const char* msg, uint16_t color, int score1, int score2, int totalScore1, int totalScore2, int gameCount) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,10);
    tft.setTextColor(color);
    tft.setTextSize(3);
    tft.println("Game Over");
    tft.setCursor(0,60);
    tft.setTextSize(2);
    tft.print(msg);
    tft.println(" Died");
    tft.setCursor(0,100);
    tft.print("Green Score: ");
    tft.println(score1);
    tft.print("Pink Score: ");
    tft.println(score2);
    tft.print("Green Total Score: ");
    tft.println(totalScore1);
    tft.print("Pink Total Score: ");
    tft.println(totalScore2);
    tft.print("Games Played: ");
    tft.println(gameCount);
    delay(4000); // Wait for 2000 ms
}
