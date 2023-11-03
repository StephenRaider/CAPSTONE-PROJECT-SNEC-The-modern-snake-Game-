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

// delay variable
int dly = 300;

// Here we  are definig the game variables
int snakeX[100], snakeY[100], numSegments;
int appleX, appleY;
int dirX, dirY;
int score = 0; //We add a score counter for noting score
int highScore = 0; //We add a high score tracker for checking the change of high score

//We are defining  the game variables 2
int snake2X[100], snake2Y[100], numSegments2;
int apple2X, apple2Y;
int dir2X, dir2Y;
int score2 = 0; // We add a score counter for noting score2
int highScore2 = 0; //We add a high score tracker for checking the change of high score

bool gameStarted = false; //we added a game started flag


//Defining the grid size of our game
int gridSize = 10;

void setup() 
{
  Serial.begin(9600); // Start serial communication at 9600 baud
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  clearScreen();
  showLevelScreen(); // Show the start screen instead of starting the game
}

void loop() 
{
  if (gameStarted) 
  {
      int oldTailX = snakeX[numSegments - 1];
      int oldTailY = snakeY[numSegments - 1];

      int oldTail2X = snake2X[numSegments2 - 1];
      int oldTail2Y = snake2Y[numSegments2 - 1];

      moveSnake();
      moveSnake2();

      if (snakeX[0] == appleX && snakeY[0] == appleY) eatApple();
      if (snake2X[0] == apple2X && snake2Y[0] == apple2Y) eatApple2();
      
      if (checkCollision()) 
      {
          startGame();
      }

      if (checkCollision2()) 
      {
          startGame();
      }

      //The old tail position must be erased 
      tft.fillRect(oldTailX, oldTailY, gridSize, gridSize, TFT_BLACK);
      tft.fillRect(oldTail2X, oldTail2Y, gridSize, gridSize, TFT_BLACK);

      drawSnake();
      drawSnake2();

      tft.fillRect(tft.width()/2 -1, 0, 1, tft.height(), TFT_WHITE);
      
      delay(dly);
  }
 
  else 
  {
      if (Serial.available() > 0) 
      {
          char key = Serial.read(); // Read the incoming byte
          if (key == 'a')
          {
              dly = 300;
              gameStarted = true;
              startGame();
          }
          else if (key == 'b')
          {
              dly = 250;
              gameStarted = true;
              startGame();
          }
          else if (key == 'c')
          {
              dly = 200;
              gameStarted = true;
              startGame();
          }
          else if (key == 'd')
          {
              dly = 150;
              gameStarted = true;
              startGame();
          }
          else if (key == 'e')
          {
              dly = 100;
              gameStarted = true;
              startGame();
          }
      }
  }
}

void clearScreen() 
{
  tft.fillScreen(TFT_BLACK);
}

void startGame() 
{
  clearScreen();
  numSegments = 3;
  numSegments2 = 3;

  for (int i = 0; i < numSegments; i++) 
  {
      snakeX[i] = (tft.width() / gridSize) / 4 * gridSize - i * gridSize;
      snakeY[i] = (tft.height() / gridSize) / 2 * gridSize;
  }

  for (int i = 0; i < numSegments2; i++) 
  {
    snake2X[i] = (((tft.width() / gridSize) / 4)*3) * gridSize - i * gridSize;
    snake2Y[i] = (tft.height() / gridSize) / 2 * gridSize;
  }

  score = 0; // Reseting the score when a new game starts each time
  score2 = 0;

  dirX = gridSize; // Set initial direction of motion to right always
  dirY = 0;

  dir2X = gridSize; // Set initial direction of motion to right
  dir2Y = 0;

  spawnApple();
  spawnApple2();
}

void spawnApple() 
{
  bool appleInSnake;
  do 
  {
    appleInSnake = false;
    appleX = random((tft.width()/2) / gridSize) * gridSize;
    appleY = random(tft.height() / gridSize) * gridSize;

    // Check if the apple is inside the snake's body
    for (int i = 0; i < numSegments; i++) 
    {
      if (snakeX[i] == appleX && snakeY[i] == appleY) 
      {
        appleInSnake = true;
        break;
      }
    }
  } while (appleInSnake);  // Keep looping until the apple is not in the snake

  // Draw apple
  tft.fillRect(appleX, appleY, gridSize, gridSize, TFT_RED);
}

void spawnApple2() 
{
  bool appleInSnake;
  do 
  {
    appleInSnake = false;
    apple2X = ((random((tft.width()/2) / gridSize))+(tft.width()/2/gridSize)) * gridSize;
    apple2Y = random(tft.height() / gridSize) * gridSize;

    // Check if the apple is inside the snake's body
    for (int i = 0; i < numSegments2; i++) 
    {
      if (snake2X[i] == apple2X && snake2Y[i] == apple2Y) 
      {
        appleInSnake = true;
        break;
      }
    }
  } while (appleInSnake);  // Keep looping until the apple is not in the snake

  // Draw apple
  tft.fillRect(apple2X, apple2Y, gridSize, gridSize, TFT_RED);
}


void moveSnake() 
{
// Check if data is available to read
  if (Serial.available() > 0) 
  {
      char key = Serial.read(); // Read the input to decide motion of our controlled snake

      // Update direction based on key press each time
      switch (key)
      {
          case 'w':
          case 'i': // Up arrow
              if (dirY != gridSize) 
              { // Prevent moving directly backwards
                  dirX = 0;
                  dirY = -gridSize;
              }
              break;
          case 'a':
          case 'j': // Left arrow
              if (dirX != gridSize) 
              {
                  dirX = -gridSize;
                  dirY = 0;
              }
              break;
          case 's':
          case 'k': // Down arrow
              if (dirY != -gridSize) 
              {
                  dirX = 0;
                  dirY = gridSize;
              }
              break;
          case 'd':
          case 'l': // Right arrow
              if (dirX != -gridSize) 
              {
                  dirX = gridSize;
                  dirY = 0;
              }
              break;
      }
  }

  // Here we are shifting the segments down the array
  for (int i=numSegments-1; i>0; i--)
  {
      snakeX[i] = snakeX[i-1];
      snakeY[i] = snakeY[i-1];
  }

  // Moving the head
  snakeX[0] += dirX;
  snakeY[0] += dirY;

  // Check if out of bounds
  if (snakeX[0] < 0 || snakeX[0] >= tft.width()/2 || snakeY[0] < 0 || snakeY[0] >= tft.height()) 
  {
      gameOver();
      startGame();
  }
}

void moveSnake2() 
{
    // Simple AI: stick to chosen apple until it is eaten
    int newDirX = dir2X;
    int newDirY = dir2Y;


        if (apple2X != snake2X[0] && apple2Y != snake2Y[0]) 
        {
            newDirX = (apple2X > snake2X[0]) ? gridSize : -gridSize;
            newDirY = (apple2Y > snake2Y[0]) ? gridSize : -gridSize;
        } 
        else if (apple2X != snake2X[0]) 
        {
            newDirX = (apple2X > snake2X[0]) ? gridSize : -gridSize;
            newDirY = 0;
        } 
        else if (apple2Y != snake2Y[0]) 
        {
            newDirY = (apple2Y > snake2Y[0]) ? gridSize : -gridSize;
            newDirX = 0;
        }

    // Check for collisions in the new direction
    int newX = snake2X[0] + newDirX;
    int newY = snake2Y[0] + newDirY;
    bool collision = false;
    for(int i=0; i<numSegments2; i++) 
    {
        if(newX == (snake2X[0]+i) && newY == (snake2Y[0]+i)) 
        {
            collision = true;
            break;
        }
    }

    // If a collision is detected, try turning left or right
    if (collision) 
    {
        // Try turning right
        int temp = newDirX;
        newDirX = -newDirY;
        newDirY = temp;

        newX = snake2X[0] + newDirX;
        newY = snake2Y[0] + newDirY;

        for(int i=0; i<numSegments2; i++) 
        {
            if(newX == (snake2X[0]+i) && newY == (snake2Y[0]+i)) 
            {
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
    if (newX < tft.width()/2 || newX >= tft.width() || newY < 0 || newY >= tft.height()) 
    {
        // If out of bounds, try turning right
        int temp = newDirX;
        newDirX = -newDirY;
        newDirY = temp;

        newX = snake2X[0] + newDirX;
        newY = snake2Y[0] + newDirY;

        if (newX < tft.width()/2 || newX >= tft.width() || newY < 0 || newY >= tft.height()) 
        {
            // If still out of bounds, try turning into other direction
            newDirX = -newDirX;
            newDirY = -newDirY;  
        }    
    }

    // Update direction
    dir2X = newDirX;
    dir2Y = newDirY;

    // Shift segments down the array
    for (int i=numSegments2-1; i>0; i--) {
      snake2X[i] = snake2X[i-1];
      snake2Y[i] = snake2Y[i-1];
  }

    // Move head
    snake2X[0] += dir2X;
    snake2Y[0] += dir2Y;

    // Check if out of bounds
  if (snake2X[0] < tft.width()/2 || snake2X[0] >= tft.width() || snake2Y[0] < 0 || snake2Y[0] >= tft.height()) 
  {
      gameOver();
      startGame();
  }
}

bool checkCollision() 
{
  for(int i=1; i<numSegments; i++) {
      if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
          gameOver();
          return true;
      }
  }
  return false;
}

bool checkCollision2() 
{
  for(int i=1; i<numSegments2; i++) {
      if(snake2X[0] == snake2X[i] && snake2Y[0] == snake2Y[i]) {
          gameOver();
          return true;
      }
  }
  return false;
}

void eatApple() 
{
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

void eatApple2() 
{
  // Add new segment at position offset from head
  snake2X[numSegments2] = snake2X[numSegments2-1] - dirX;
  snake2Y[numSegments2] = snake2Y[numSegments2-1] - dirY;

  numSegments2++;

  score2++; // Increase the score when an apple is eaten
  if (score2 > highScore2) { // Update the high score if necessary
      highScore2 = score2;
  }

  spawnApple2();
}

void drawSnake() 
{
  // Draw new head position
  tft.fillRect(snakeX[0],snakeY[0],gridSize,gridSize,TFT_BLUE);

  // Draw rest of the body
  for (int i=1; i<numSegments; i++) {
      tft.fillRect(snakeX[i],snakeY[i],gridSize,gridSize,TFT_GREEN);
  }
}

void drawSnake2() 
{
  // Draw new head position
  tft.fillRect(snake2X[0],snake2Y[0],gridSize,gridSize,TFT_CYAN);

  // Draw rest of the body
  for (int i=1; i<numSegments2; i++) 
  {
      tft.fillRect(snake2X[i],snake2Y[i],gridSize,gridSize,TFT_MAGENTA);
  }
}

void gameOver() 
{
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

/*void showStartScreen() 
{
    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.println("SNEC Ver. 2.1");
    tft.println("By team SNEC");
    tft.println(" ");
    tft.setCursor(0, tft.height()/2);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.println("Press 'w' to start");
}*/

void showLevelScreen()
{
    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.println("press any number key from A to E");
    tft.println("for level selection");
    tft.println(" ");
    tft.println("SNEC ver 2.1");
    tft.println("Add: level selector");
    tft.println("Team SNEC");
}
