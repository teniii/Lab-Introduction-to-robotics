//for the Matrix and the game
#include "LedControl.h"
#include <EEPROM.h>
LedControl lc = LedControl(12, 11, 10, 1);
const int pinX1 = A0, pinY1 = A1, pinX2 = A2, pinY2 = A3;

int memoryAddress = 0;
int pos1 = 0, pos2 = 0;
bool turn = 0, fullMatrix = 0;
int x1Value, y1Value, x2Value, y2Value;
bool joyMoved = false;
bool state = true;

bool joyMovedX = false;
bool joyMovedY = false;
int minThreshold = 300;
int maxThreshold = 700;

int level = 1;
int brightnessLevel = 1;
long lastUpdate = 0;
long scorePlayer1 = 0, scorePlayer2 = 0, highScore = 0, score = 0;
int i = 0;
int x1Win = 0, y1Win = 0, x2Win = 3, y2Win = 3; //the coordinates of the winning move (start + end)

bool difficulty = false;
unsigned long startPulsing = 0;

int matrix[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

//for the lcd

#include <LiquidCrystal.h>
const int RS = 9;
const int enable = 8;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);



void mainM();
void welcome();

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();

  // the zero refers to the MAX7219 number, it is zero for 1 chip
  pinMode(pinX1, INPUT);
  pinMode(pinY1, INPUT);
  pinMode(pinX2, INPUT);
  pinMode(pinY2, INPUT);
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 1); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  Serial.begin(9600);

  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
  //welcome();
  highScore = EEPROM.read(memoryAddress);
}


void welcome()
{
  long startWelcoming = millis();
  while ( millis() - startWelcoming < 5000) {
    lcd.setCursor(0, 0);
    lcd.print("Welcome to the");
    lcd.setCursor(0, 1);
    lcd.print("Connect4 Game!");
  }
  lcd.clear();
}

void showCongrats()
{

  long startCongr = millis();
  //lcd.clear();
  while (joyMovedY == false) { //(y1Value <= maxThreshold && y1Value >= minThreshold){

    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);


    if ((millis() - startCongr) % 6000 < 3000) { //we are showing alternatively the messages
      lcd.setCursor(0, 0);
      lcd.print("Congratulations ");
      lcd.setCursor(0, 1);
      if (turn == 0)
        lcd.print("P2, ");
      else
        lcd.print("P1, ");
      lcd.print("you won!             ");
    }

    if ((millis() - startCongr) % 6000 > 3000)
    {

      lcd.setCursor(0, 0);
      lcd.print("Move joystick up       ");
      lcd.setCursor(0, 1);
      lcd.print("or down to exit!            ");
    }

    if (y1Value > maxThreshold || y1Value < minThreshold) {
      joyMovedY = true;
      lcd.clear();
      return;
    }
  }

}

void startGame() {

  x1Value = analogRead(pinX1);
  y1Value = analogRead(pinY1);


  long start = millis();
  score = level * 3;
  lastUpdate = start;
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lives:3");
  lcd.setCursor(9, 0);
  lcd.print("Lvl:");
  lcd.setCursor(13, 0);
  lcd.print(level);
  lcd.setCursor(3, 1);
  lcd.print("Score:");
  lcd.setCursor(9, 1);
  lcd.print(score);


  while (millis() - start < 10000) {

    lcd.setCursor(0, 0);
    lcd.print("Lives:3");
    lcd.setCursor(9, 0);
    lcd.print("Lvl:");
    lcd.setCursor(13, 0);
    lcd.print(level);
    lcd.setCursor(3, 1);
    lcd.print("Score:");
    lcd.setCursor(9, 1);
    lcd.print(score);

    if (millis() - lastUpdate > 2000) { //we update the score and the level
      level++;
      score = 3 * level;
      lastUpdate = millis();
    }
  }
  showCongrats();
  if (score > highScore)
    highScore = score;
  return;

}

void high() {
  joyMovedY = false;

  long startH = millis();
  while ( joyMovedY == false) {

    y1Value = analogRead(pinY1);

    lcd.setCursor(1, 0);
    lcd.print("HighScore:                ");
    lcd.setCursor(1, 1);
    lcd.print(highScore);
    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startH > 500) {
      joyMovedY = true;
      lcd.clear();
      return;
    }
  }

}

void setDifficulty()
{
  joyMovedY = false;
  long startS = millis();
  lcd.clear();
  while ( joyMovedY == false) {
    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);
    lcd.setCursor(0, 0);
    lcd.print("Level:");


    if (x1Value > maxThreshold && joyMovedX == false) {
      difficulty = !difficulty;
      joyMovedX = true;
    }
    else if (x1Value < minThreshold && joyMovedX == false) {
      difficulty = !difficulty;
      joyMovedX = true;
    }

    lcd.setCursor(7, 0);

    if (difficulty)
      lcd.print("Hard    ");
    else
      lcd.print("Easy    ");

    if (x1Value <= maxThreshold && x1Value >= minThreshold)
      joyMovedX = false;

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startS > 500) {
      joyMovedY = true;
      lcd.clear();
      return;
    }
  }
}

void setMatrixBrightness()
{
  joyMovedY = false;
  long startB = millis();
  lcd.clear();
  while ( joyMovedY == false) {
    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);
    lcd.setCursor(0, 0);
    lcd.print("Brightness:");
    lcd.setCursor(11, 0);
    lcd.print(brightnessLevel);

    if (brightnessLevel % 10 == brightnessLevel) { //to remove the second figure when we change from 10 to 9
      lcd.setCursor(12, 0);
      lcd.print(" ");
    }

    for (int row = 0; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
        lc.setLed(0, row, col, 1);
      }
    }

    if (x1Value < minThreshold && joyMovedX == false) {
      brightnessLevel++;
      if (brightnessLevel > 15)
        brightnessLevel = 1;
      lc.setIntensity(0, brightnessLevel);
      joyMovedX = true;
    }
    else if (x1Value > maxThreshold && joyMovedX == false) {
      brightnessLevel--;
      if (brightnessLevel < 1)
        brightnessLevel = 1;
      lc.setIntensity(0, brightnessLevel);
      joyMovedX = true;
    }

    //lcd.setCursor(11,0);
    // lcd.print(brightnessLevel);


    if (x1Value <= maxThreshold && x1Value >= minThreshold)
      joyMovedX = false;

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startB > 500) {
      joyMovedY = true;
      for (int row = 0; row < 8; row++)
      {
        for (int col = 0; col < 8; col++)
        {
          lc.setLed(0, row, col, 0);
        }
      }
      lcd.clear();
      return;
    }
  }
}

void settings()
{

  joyMovedY = false;
  bool  joyMovedUp = false;
  long startS = millis();
  bool arrow = false;

  //int posR[4] = {0, 6, 0, 10}, posC[4] = {0, 0, 1, 1}; //the positions where the arrow can be
  while (joyMovedUp == false) {
    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);

    lcd.setCursor(1, 0);
    lcd.print("Difficulty     ");
    lcd.setCursor(1, 1);
    lcd.print("Brightness      ");

    //dau in jos -> e mai mare
    lcd.setCursor(0, arrow);
    lcd.print(">");

    if (x1Value > maxThreshold && joyMovedX == false) {

      //we delete the present arrow
      lcd.setCursor(0, arrow);
      lcd.print(" ");
      arrow = !arrow;
      lcd.setCursor(0, arrow);
      lcd.print(">");

      joyMovedX = true;
    }

    if (x1Value < minThreshold && joyMovedX == false) {

      //we delete the present arrow
      lcd.setCursor(0, arrow);
      lcd.print(" ");
      arrow = !arrow;
      lcd.setCursor(0, arrow);
      lcd.print(">");

      joyMovedX = true;
    }
    //Serial.println(y1Value);
    if (x1Value >= minThreshold && x1Value <= maxThreshold) {
      joyMovedX = false;
    }

    if ((y1Value > maxThreshold ) && millis() - startS > 500 && joyMovedY == false) {
      if (arrow == false) {
        lcd.clear();
        setDifficulty();
      }
      if (arrow == true) {
        lcd.clear();
        setMatrixBrightness();
      }
      joyMovedY = true;
    }

    if (y1Value >= minThreshold && y1Value <= maxThreshold) {
      joyMovedY = false;
      joyMovedUp = false;
      //lcd.clear();
    }
    if (y1Value < minThreshold && joyMovedY == false) {
      joyMovedY = true;
      joyMovedUp = true;
      lcd.clear();
    }
  }
}

void info()
{

  joyMovedY = false;
  lcd.clear();

  long startInfo = millis();

  while ( joyMovedY == false) {

    y1Value = analogRead(pinY1);

    if ((millis() - startInfo) % 6000 < 3000) { //we are showing alternatively the messages
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The Connect4Game  ");
      lcd.setCursor(0, 1);
      lcd.print("by Alex Tenita   ");
    }

    if ((millis() - startInfo) % 6000 > 3000)
    {
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("@UnibucRobotics  ");
      lcd.setCursor(0, 1);
      lcd.print("github.com/teniiii");
    }

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startInfo > 500) {
      joyMovedY = true;
      lcd.clear();
      return;
    }
  }


}

void mainMenu() {
  lcd.setCursor(1, 0);
  lcd.print("Play ");
  lcd.setCursor(7, 0);
  lcd.print("HighScore");
  lcd.setCursor(1, 1);
  lcd.print("Settings ");
  lcd.setCursor(11, 1);
  lcd.print("Info  ");

  int posR[4] = {0, 6, 0, 10}, posC[4] = {0, 0, 1, 1}; //the positions where the arrow can be

  x1Value = analogRead(pinX1);
  y1Value = analogRead(pinY1);

  lcd.setCursor(posR[i % 4], posC[i % 4]);
  lcd.print(">");

  if (x1Value > maxThreshold && joyMovedX == false) {

    lcd.setCursor(posR[i % 4], posC[i % 4]); //we delete the present arrow
    lcd.print(" ");

    i--;
    if (i < 0)
      i = 3;

    lcd.setCursor(posR[i % 4], posC[i % 4]); //posR and posC are for the possible position on columns and rows for the arrow

    lcd.print(">");

    joyMovedX = true;
  }

  if (x1Value < minThreshold && joyMovedX == false) {

    lcd.setCursor(posR[i % 4], posC[i % 4]);
    lcd.print(" ");
    i++; //going for the next one
    if (i == 4)
      i = 0;

    lcd.print(" ");
    lcd.setCursor(posR[i], posC[i]); //posR and posC are for the possible position on columns and rows for the arrow
    lcd.print(">");

    joyMovedX = true;
  }

  if (x1Value >= minThreshold && x1Value <= maxThreshold) {
    joyMovedX = false;
  }

  if ((y1Value > maxThreshold || y1Value < minThreshold) && joyMovedY == false) {
    if (i == 0) {
      lcd.clear();
      //startGame();
      game();
    }
    if (i == 1) {  // depending on the i, we know what option is selected
      lcd.clear();
      high();
    }
    if (i == 2) {
      lcd.clear();
      settings();
    }
    if (i == 3) {
      lcd.clear();
      info();
    }
    joyMovedY = true;
  }

  if (y1Value >= minThreshold && y1Value <= maxThreshold) {
    joyMovedY = false;
  }

}







void showWinningMove()
{
  for (int row = 0; row < 8; row++) {         //shuting down the matrix
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, 0);
    }
  }
  if (x1Win == x2Win) {
    for (int i = min(y1Win, y2Win); i <= max(y1Win, y2Win); i++)
      lc.setLed(0, x1Win, i, 1);
  }
  else if (y1Win == y2Win) {
    for (int i = min(x1Win, x2Win); i <= max(x1Win, x2Win); i++)
      lc.setLed(0, i, y1Win, 1);
  }
  else
  {
    if (y2Win - y1Win == x2Win - x1Win) { //diag /
      int j = min(y1Win, y2Win);
      for (int i = min(x1Win, x2Win); i <= max(x1Win, x2Win) && j <= max(y1Win, y2Win); i++, j++)
        lc.setLed(0, i, j, 1);
    }
    else { //diag 2 (\)
      int j = min(y1Win, y2Win);
      for (int i = max(x1Win, x2Win); i >= min(x1Win, x2Win) && j <= max(y1Win, y2Win); i--, j++)
        lc.setLed(0, i, j, 1);
    }
  }

  showCongrats();

}

bool check(int b) {
  int vertical = 1;//(|)
  int horizontal = 1;//(-)
  int diagonal1 = 1;//(\)
  int diagonal2 = 1;//(/)

  int iV;//vertical index
  int iH;//horizontal index
  int a; //the row of the last dot inserted
  int player;
  //int index;
  //mat[i][j] i - row, j collumn
  for (a = 6; a > 0 && matrix[a][b] == 0; a--);

  player = matrix[a][b];
  //Serial.println(a);





  //check for horizontal(-)
  for (iH = b - 1; matrix[a][iH] == player && iH >= 0; iH--, horizontal++); //Check left
  iH++;
  for (iH = b + 1; matrix[a][iH] == player && iH <= 7; iH++, horizontal++); //Check right
  iH--;
  //Serial.println(horizontal);
  if (horizontal >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = a;
    y2Win = iH;
    //showWinningMove(a, b, a, iH);
    return true;
  }

  //check for diagonal 1 (\)
  for (iV = a - 1, iH = b - 1; matrix[iV][iH] == player && iV >= 0 && iH >= 0; diagonal1++, iV --, iH --); //up and left
  iV++, iH++;
  for (iV = a + 1, iH = b + 1; matrix[iV][iH] == player && iV <= 6 && iH <= 7; diagonal1++, iV ++, iH ++); //down and right
  iV--, iH--;
  //Serial.println(diagonal1);
  if (diagonal1 >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = iV;
    y2Win = iH;
    //showWinningMove(a, b, iV, iH);
    return true;
  }

  //check for diagonal 2(/)
  for (iV = a - 1, iH = b + 1; matrix[iV][iH] == player && iV >= 0 && iH <= 7; diagonal2++, iV --, iH ++); //up and right
  iV++, iH--;
  for (iV = a + 1, iH = b - 1; matrix[iV][iH] == player && iV <= 6 && iH >= 0;  diagonal2++, iV ++, iH --); //up and left
  iV--, iH++;
  if (diagonal2 >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = iV;
    y2Win = iH;
    //showWinningMove(a, b, iV, iH);
    return true;
  }

  if (a == 0) player = -1;

  //check for vertical(|)
  for (iV = a - 1; matrix[iV][b] == player && iV >= 0; iV--, vertical++); //Check up
  iV++; //the last i that checked the conditions before exiting the for
  for (iV = a + 1; matrix[iV][b] == player && iV <= 6; iV++, vertical++); //Check down
  iV--; //the last i that checked the conditions before exiting the for
  Serial.println(vertical);
  if (vertical >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = iV;
    y2Win = b;
    //showWinningMove(a, b, iV, b);

    return true;
  }


  return false;
}



void game()
{
  unsigned long startGame = millis();
  int numberOfGames = 0;
  y1Value = analogRead(pinY1);

  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 8; col++) {
      matrix[row][col] = 0;
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }

  while (numberOfGames < 3) {
    x1Value = analogRead(pinX1);
    x2Value = analogRead(pinX2);
    y1Value = analogRead(pinY1);
    y2Value = analogRead(pinY2);
    fullMatrix = true;
    for (int i = 0; i < 8; i++)
      if (matrix[1][i] == 0)
        fullMatrix = false;

    if (turn == 0) {
      lc.setLed(0, 7, pos1, 1);
      if (x1Value > 600 && joyMoved == false) {
        lc.setLed(0, 7, pos1, 0);
        pos1++;
        pos1 %= 8;
        joyMoved = true;
      }
      if (x1Value < 400 && joyMoved == false) {
        lc.setLed(0, 7, pos1 , 0);
        pos1--;
        if (pos1 < 0)
          pos1 = 7;
        joyMoved = true;
      }

      lc.setLed(0, 7, pos1, 1);
      //if(millis() - startGame > 1000 && (y1Value >= minThreshold && y1Value <= maxThreshold) )

      if (y1Value > 700 && joyMoved == false && millis() - startGame > 1000) {
        int startWait = millis();
        lc.setLed(0, 7, pos1, 0);
        int i;
        for (i = 6; i > 0 && matrix[i - 1][pos1] == 0; ) {
          lc.setLed(0, i, pos1, 1);
          if (millis() - startWait > 55) {
            lc.setLed(0, i, pos1, 0);
            i--;
            lc.setLed(0, i, pos1, 1);
            startWait = millis();

          }
        }
        if (matrix[6][pos1] == 0) {
          matrix[i][pos1] = 1;
          turn = 1;

        }
        else
          turn = 0;

        if (check(pos1)) {
          numberOfGames++;
          scorePlayer1++;
          showWinningMove();
          //if ((scorePlayer1 + 1) % 2 == 0 && scorePlayer1 == max(scorePlayer1 + 1, scorePlayer2)) {
          //mainMenu();
          //scorePlayer1++;
          lcd.clear();
          // numberOfGames++;
          //}
          //else { //stergere matrice
          //scorePlayer1++;

          for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 8; col++) {
              matrix[row][col] = 0;
              lc.setLed(0, row, col, matrix[row][col]);
            }
          }
        }
        //}

        joyMoved = true;
      }

      if (x1Value <= 600 && x1Value >= 400)// && yValue >= 400)
        joyMoved = false;

    }

    else {
      lc.setLed(0, 7, pos2, 1);
      if (x2Value > 600 && joyMoved == false) {
        lc.setLed(0, 7, pos2, 0);
        pos2++;
        pos2 %= 8;
        joyMoved = true;
      }
      if (x2Value < 400 && joyMoved == false) {
        lc.setLed(0, 7, pos2 , 0);
        pos2--;
        if (pos2 < 0)
          pos2 = 7;
        joyMoved = true;
      }

      if (y2Value > 700 && joyMoved == false) {

        int startWait = millis();
        lc.setLed(0, 7, pos2, 0);
        //if(
        int i;
        for (i = 6; i > 0 && matrix[i - 1][pos2] == 0; ) {
          lc.setLed(0, i, pos2, 0);
          if (millis() - startWait > 55) {
            lc.setLed(0, i, pos2, 0);
            i--;
            lc.setLed(0, i, pos2, 1);
            startWait = millis();

          }
        }
        if (matrix[6][pos2] == 0) {
          matrix[i][pos2] = 2;
          turn = 0;

        }
        else
          turn = 1;

        joyMoved = true;

        if (check(pos2)) {
          showWinningMove();
          scorePlayer2++;
          numberOfGames++;
          //if ((scorePlayer2 + 1) % 2 == 0 && scorePlayer2 == max(scorePlayer1, scorePlayer2 + 1)) {

          //scorePlayer2++;
          lcd.clear();
          //return;
          //}
          //else { //stergere matrice
          //scorePlayer2++;

          for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 8; col++) {
              matrix[row][col] = 0;
              lc.setLed(0, row, col, matrix[row][col]);
            }
          }
        }
        //}


        //Serial.println(check(pos2));
        //return;


      }

      if (x2Value <= 600 && x2Value >= 400)// && yValue >= 400)
        joyMoved = false;
    }
    lcd.setCursor(0, 0);
    lcd.print("P1");
    lcd.setCursor(14, 0);
    lcd.print("P2");
    lcd.setCursor(6, 0);
    if (difficulty)
      lcd.print("Hard");
    else
      lcd.print("Easy");
    lcd.setCursor(6, 1);
    lcd.print(scorePlayer1);
    lcd.print("--");
    lcd.print(scorePlayer2);
    if (turn == 1) {
      lcd.setCursor(1, 1);
      lcd.print(" ");
      lcd.setCursor(15, 1);
      lcd.print("^");
    }
    if (turn == 0) {
      lcd.setCursor(15, 1);
      lcd.print(" ");
      lcd.setCursor(1, 1);
      lcd.print("^");

    }

    unsigned long currentTime = millis();
    bool change = false;
    for (int row = 0; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
        if (matrix[row][col] == 1) {

          lc.setLed(0, row, col, matrix[row][col]);
        }
        if (matrix[row][col] == 2) {
          if (currentTime - startPulsing > 100) {
            change = true;
            lc.setLed(0, row, col, !state);
          }
        }

      }
    }

    if (change == true) {
      state = !state;
      startPulsing = millis();
    }

  }
}


void loop()
{
  mainMenu();
  Serial.println(1);
  //showWinningMove();

  //game();
  //welcome();
}
