//for the Matrix and the game
#include "LedControl.h"
#include <EEPROM.h>
LedControl lc = LedControl(12, 11, 10, 1);
const int pinX1 = A0, pinY1 = A1, pinX2 = A2, pinY2 = A3;
const int pinBuzzer = 6;
int buzzerTone = 2000;
int buzzerPeriod = 4000;
int buzzerDelay = 50;
int timeLeft = 9000;

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


int brightnessLevel = 1;
long lastUpdate = 0;
long scorePlayer1 = 0, scorePlayer2 = 0, highScore = 0, score = 0;
int i = 0;
int x1Win = 1, y1Win = 4, x2Win = 4, y2Win = 7; //the coordinates of the winning move (start + end)

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
  welcome();
  highScore = EEPROM.read(memoryAddress); //we take the highscore from the EEPROM
}






void welcome()
{
  unsigned long startWelcoming = millis();
  while ( millis() - startWelcoming < 5000) { //this is the message that will welcome the players for 5 seconds
    lcd.setCursor(0, 0);
    lcd.print("Welcome to the");
    lcd.setCursor(0, 1);
    lcd.print("Connect4 Game!");
  }
  lcd.clear();
}





void showWinningMove();

void showCongrats() {

  noTone(pinBuzzer); //we stop the buzzer if the game is in hard mode
  unsigned long startCongr = millis();
  joyMovedY = false;

  for (int row = 0; row < 8; row++) {         //shuting down the matrix
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, 0);
    }
  }

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

    if ((millis() - startCongr) % 6000 > 3000) {

      lcd.setCursor(0, 0);
      lcd.print("P1,move joystick       ");
      lcd.setCursor(0, 1);
      lcd.print("up to continue!            ");

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
      if (y2Win - y1Win == x2Win - x1Win) { //diag1 (/)
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

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startCongr > 1000) {
      lcd.clear();
      joyMovedY = true;

      for (int row = 0; row < 8; row++) {         //shuting down the matrix
        for (int col = 0; col < 8; col++) {
          lc.setLed(0, row, col, 0);
        }
      }
      return;
    }
  }

}





void high() { //the highscore menu

  joyMovedY = false;

  unsigned long startH = millis();
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






void setDifficulty() {
  joyMovedY = false;

  unsigned long startS = millis();
  lcd.clear();
  while ( joyMovedY == false) {
    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);
    lcd.setCursor(3, 0);
    lcd.print("Difficulty:");


    if (x1Value > maxThreshold && joyMovedX == false) {
      difficulty = !difficulty;
      joyMovedX = true;
    }
    else if (x1Value < minThreshold && joyMovedX == false) {
      difficulty = !difficulty;//if the joystick is used with X axis, the difficulty changes
      joyMovedX = true;
    }

    lcd.setCursor(6, 1);

    if (difficulty)
      lcd.print("Hard    ");
    else
      lcd.print("Easy    ");

    if (x1Value <= maxThreshold && x1Value >= minThreshold)
      joyMovedX = false;

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startS > 500) {
      joyMovedY = true; //for exiting from the menu
      lcd.clear();
      return;
    }
  }
}




void setMatrixBrightness() {

  joyMovedY = false;
  unsigned long startB = millis();
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

    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, row, col, 1);
      }
    }
    //depending on which side of the X axis the joystick is moved, the brightness changes
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


    if (x1Value <= maxThreshold && x1Value >= minThreshold)
      joyMovedX = false;

    //if the Y axis is used and it passed half of second since we've entered this menu
    //we exit, but not before we let everything clean (the matrix and the lcd (the code can't be cleaner than this, sorry :( ))

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startB > 500) {
      joyMovedY = true;
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          lc.setLed(0, row, col, 0);
        }
      }
      lcd.clear();
      return;
    }
  }
}





void settings() {

  joyMovedY = false;
  bool  joyMovedUp = false;
  unsigned long startS = millis();
  bool arrow = false; //the positon of the arrow. It's a boolean because there are
  //only 2 positions possible

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

    //if the X axis is used, the arrow changes

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


    if (x1Value >= minThreshold && x1Value <= maxThreshold) {
      joyMovedX = false;
    }

    //if the Y axis is used in the DOWN motion, the arrow-indicated function will be called

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

    //if the Y axis is used in the UP motion, it will go back to the mainMenu

    if (y1Value < minThreshold && joyMovedY == false) {
      joyMovedY = true;
      joyMovedUp = true;
      lcd.clear();
    }
  }
}



//////////////////////////////////to be continued

void info()
{

  joyMovedY = false;
  lcd.clear();

  unsigned long startInfo = millis();

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






void game();

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


  unsigned long startWin = millis();
  y1Value = analogRead(pinY1);
  y2Value = analogRead(pinY2);
  //while ( millis() - startWin > 500 && y1Value >= minThreshold && y1Value <= maxThreshold && y1Value >= minThreshold && y1Value <= maxThreshold){
  y1Value = analogRead(pinY1);
  y2Value = analogRead(pinY2);
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
    if (y2Win - y1Win == x2Win - x1Win) { //diag1 (/)
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

  if (horizontal >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = a;
    y2Win = iH;

    return true;
  }

  //check for diagonal 1 (\)
  for (iV = a - 1, iH = b - 1; matrix[iV][iH] == player && iV >= 0 && iH >= 0; diagonal1++, iV --, iH --); //up and left
  iV++, iH++;
  for (iV = a + 1, iH = b + 1; matrix[iV][iH] == player && iV <= 6 && iH <= 7; diagonal1++, iV ++, iH ++); //down and right
  iV--, iH--;

  if (diagonal1 >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = iV;
    y2Win = iH;

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

    return true;
  }

  if (a == 0) player = -1; //for the vertical case we have to put another value in the player variable

  //check for vertical(|)
  for (iV = a - 1; matrix[iV][b] == player && iV >= 0; iV--, vertical++); //Check up
  iV++; //the last i that checked the conditions before exiting the for
  for (iV = a + 1; matrix[iV][b] == player && iV <= 6; iV++, vertical++); //Check down
  iV--; //the last i that checked the conditions before exiting the for

  if (vertical >= 4) {
    x1Win = a;
    y1Win = b;
    x2Win = iV;
    y2Win = b;

    return true;
  }

  return false;
}




void endOfGame() {


  unsigned long startEnd = millis();
  lcd.clear();
  while (joyMovedY == false) { //(y1Value <= maxThreshold && y1Value >= minThreshold){

    x1Value = analogRead(pinX1);
    y1Value = analogRead(pinY1);


    if (scorePlayer2 == scorePlayer1) {


      lcd.setCursor(0, 0);
      lcd.print("WOW! IT'S A TIE!    ");
      lcd.setCursor(0, 1);
      lcd.print("                       ");
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print("THE BIG WINNER:    ");
      lcd.setCursor(2, 1);
      if (scorePlayer1 > scorePlayer2)
        lcd.print("PLAYER1!!!");
      if (scorePlayer1 < scorePlayer2)
        lcd.print("PLAYER1!!!");
    }
    delay(3000);
    if ((y2Value > maxThreshold || y2Value < minThreshold) && millis() - startEnd > 1000) {
      lcd.clear();
      joyMovedY = true;
      return;
    }
  }

}








void game() {

  unsigned long startGame = millis();
  unsigned long startTurn = millis();
  unsigned long timeLcdDecrement = millis();

  int numberOfGames = 0;

  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 8; col++) {
      matrix[row][col] = 0;
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
  fullMatrix = false;
  while (numberOfGames < 3 && fullMatrix == false) {
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
        for (i = 6; i > 0 && matrix[i - 1][pos1] == 0; ) { //the dropping effect
          lc.setLed(0, i, pos1, 1);
          if (millis() - startWait > 55) {
            lc.setLed(0, i, pos1, 0);
            i--;
            lc.setLed(0, i, pos1, 1);
            startWait = millis();

          }
        }
        if (matrix[6][pos1] == 0) {
          matrix[i][pos1] = 1;   //the turn will change if the collumn isn't full
          turn = 1;
          startTurn = millis();
          timeLeft = 9000;
          noTone(pinBuzzer);
        }
        else
          turn = 0; //if the collumn is full, the turn won't change

        if (check(pos1)) { //if this was a winning move
          numberOfGames++; //the score and the no of games updates
          scorePlayer1++;
          //showWinningMove();

          showCongrats();   //showing the congrats message


          for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 8; col++) {
              matrix[row][col] = 0;
              lc.setLed(0, row, col, matrix[row][col]); //reseting the matrix
            }
          }

          if (scorePlayer1 > highScore) { ///checking for highscore
            highScore = scorePlayer1;
            EEPROM.write(memoryAddress, highScore);
          }
          //lcd.clear();
        }


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
          startTurn = millis();
          timeLeft = 9000;
          noTone(pinBuzzer);
        }
        else
          turn = 1;
        joyMoved = true;

        if (check(pos2)) {
          //showWinningMove();
          showCongrats();
          scorePlayer2++;
          numberOfGames++;

          for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 8; col++) {
              matrix[row][col] = 0;
              lc.setLed(0, row, col, matrix[row][col]);
            }
          }

          if (scorePlayer2 > highScore) {
            highScore = scorePlayer2;
            EEPROM.write(memoryAddress, highScore);
          }

        }
      }

      if (x2Value <= 600 && x2Value >= 400)// && yValue >= 400)
        joyMoved = false;
    }
    lcd.setCursor(0, 0);
    lcd.print("P1");
    lcd.setCursor(14, 0);
    lcd.print("P2");
    lcd.setCursor(5, 0);
    if (difficulty) {
      lcd.print("Hard:");

      lcd.print(timeLeft / 1000);
    }
    else
      lcd.print(" Easy");
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

    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {

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


    if (difficulty) {
      /*
        int buzzerTone = 2000;
        int buzzerPeriod = 4000;
        int buzzerDelay = 500;
        int timeLeft = 9000;
      */

      if (millis() - startTurn > 7000) {
        tone(pinBuzzer, buzzerTone, buzzerDelay);
        buzzerDelay /= 2;
        buzzerPeriod /= 2;
        buzzerDelay /= 2;
      }
      if (millis() - timeLcdDecrement  > 1000) { //every second, the timer will decrease by 1
        timeLeft -= 1000;
        lcd.setCursor(10, 0);
        lcd.print(timeLeft / 1000);
        timeLcdDecrement = millis();
      }

      if (timeLeft / 1000 < 1) { //if the timer is 0, we make the move automatically
        int i, pos = -1, posValue = 0, scorePlayer;
        if (turn == 0) {
          pos = pos1;
          posValue = 1;
          scorePlayer = scorePlayer1;
        }
        if (turn == 1) {
          pos = pos2;
          posValue = 2;
        }

        int startWait = millis();
        lc.setLed(0, 7, pos, 0);


        for (i = 6; i > 0 && matrix[i - 1][pos] == 0; ) { //the dropping effect
          lc.setLed(0, i, pos, 1);
          if (millis() - startWait > 55) {
            lc.setLed(0, i, pos, 0);
            i--;
            lc.setLed(0, i, pos, 1);
            startWait = millis();

          }
        }
        matrix[i][pos] = posValue;
        turn = !turn;

        if (check(pos)) { //if this was a winning move
          numberOfGames++; //the score and the no of games updates
          if (pos == pos1)
            scorePlayer1++;
          else
            scorePlayer2++;
          showWinningMove();
          //showCongrats();   //showing the congrats message


          for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 8; col++) {
              matrix[row][col] = 0;
              lc.setLed(0, row, col, matrix[row][col]); //reseting the matrix
            }
          }

          if (scorePlayer1 > highScore) { ///checking for highscore
            highScore = scorePlayer1;
            EEPROM.write(memoryAddress, highScore);
          }

          if (scorePlayer2 > highScore) { ///checking for highscore
            highScore = scorePlayer2;
            EEPROM.write(memoryAddress, highScore);
          }
        }

        timeLeft = 9000; //we reset the timer, we change the turn and deactivate the buzzer
        startTurn = millis();
        noTone(pinBuzzer);

      }
    }
  }
  //endOfGame();
  lcd.clear();
}






void loop()
{
  mainMenu();
}
