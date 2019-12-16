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


bool joyMovedX = false;
bool joyMovedY = false;
int minThreshold = 300;
int maxThreshold = 700;

int level = 1;
int brightnessLevel = 1;
long lastUpdate = 0;
long score = 0, highScore = 0;
int i = 0;

bool difficulty = false;

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
  highScore = EEPROM.read(memoryAddress);
}


void welcome()
{
  long startWelcoming = millis();
  while ( millis() - startWelcoming < 5000) {
    lcd.setCursor(0,0);
    lcd.print("Welcome to the");
    lcd.setCursor(0,1);
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
      lcd.print("Congratulations,");
      lcd.setCursor(0, 1);
      lcd.print("you finished! ");
    }

    if ((millis() - startCongr) % 6000 > 3000)
    {

      lcd.setCursor(0, 0);
      lcd.print("Press button       ");
      lcd.setCursor(0, 1);
      lcd.print("to exit!      ");
    }

    if(y1Value > maxThreshold || y1Value < minThreshold){
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
    else
    if (x1Value < minThreshold && joyMovedX == false) {
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
    lcd.setCursor(11,0);
    lcd.print(brightnessLevel);

    if(brightnessLevel % 10 ==brightnessLevel){ //to remove the second figure when we change from 10 to 9
      lcd.setCursor(12,0);
      lcd.print(" ");
    }
    

    if (x1Value < minThreshold && joyMovedX == false) {
      brightnessLevel++;
      if (brightnessLevel > 15)
        brightnessLevel = 1;
      //update the brightness
      joyMovedX = true;
    }
    else
    if (x1Value > maxThreshold && joyMovedX == false) {
      brightnessLevel--;
      if (brightnessLevel < 1)
        brightnessLevel = 1;
      //update the brightness
      joyMovedX = true;
    }

    //lcd.setCursor(11,0);
   // lcd.print(brightnessLevel);
    

    if (x1Value <= maxThreshold && x1Value >= minThreshold)
      joyMovedX = false;

    if ((y1Value > maxThreshold || y1Value < minThreshold) && millis() - startB > 500) {
      joyMovedY = true;
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
  while(joyMovedUp == false){
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
  if(y1Value < minThreshold && joyMovedY == false){
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
  lcd.setCursor(11,1);
  lcd.print("Info");

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
      startGame();
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
    if(i == 3){
      lcd.clear();
      info();
    }
    joyMovedY = true;
  }

  if (y1Value >= minThreshold && y1Value <= maxThreshold) {
    joyMovedY = false;
  }

}







void showWinningMove(int x1, int y1, int x2, int y2)
{
  for (int row = 0; row < 8; row++) {         //shuting down the matrix
    for (int col = 0; col < 8; col++) {
        lc.setLed(0, row, col, 0);
    }
  }  
  if (x1 == x2){    
    for (int i = min(y1, y2); i <= max(y1, y2); i++)
      lc.setLed(0, x1, i, 1);   
  }
  else
  if (y1 == y2){
    for (int i = min(x1, x2); i <= max(x1, x2); i++)
      lc.setLed(0, i, y1, 1);
  }
  else
  {
    if (y2 - y1 == x2 - x1){ //diag /
      int j = min(y1, y2);
      for (int i = min(x1, x2); i <= max(x1, x2) && j <= max(y1, y2); i++, j++)
        lc.setLed(0, i, j, 1);
    }
    else { //diag 2 (\)
      int j = min(y1, y2);
      for (int i = max(x1, x2); i >= min(x1, x2) && j <= max(y1, y2); i--, j++)
        lc.setLed(0, i, j, 1);
    }
  }
  
}

bool check(int b){
  int vertical = 1;//(|)
  int horizontal = 1;//(-)
  int diagonal1 = 1;//(\)
  int diagonal2 = 1;//(/)

  int i;//vertical
  int ii;//horizontal
  int a; //the row of the last dot inserted
  //int index;
  //mat[i][j] i - row, j collumn
  for(a = 6; a > 0 && matrix[a][b] == 0; a--);
  int player = matrix[a][b];
  Serial.println(a);


  //Serial.println(vertical);
  //check for vertical(|)
  for(i = a + 1; matrix[i][b] == player && i <= 5; i++, vertical++);//Check down
  i--; //the last i that checked the conditions before exiting the for
  for(i = a - 1; matrix[i][b] == player && i >= 0; i--, vertical++);//Check up
  i++; //the last i that checked the conditions before exiting the for
  if(vertical >= 4) {
    //showWinningMove(a, b, i, b);
    //Serial.println(vertical);
    return true;
  }
  
    
  //check for horizontal(-)
  for(ii = b - 1; matrix[a][ii] == player && ii >= 0; ii--, horizontal++);//Check left
  ii++;
  for(ii = b + 1; matrix[a][ii] == player && ii <= 6; ii++, horizontal++);//Check right
  ii--;
  //Serial.println(horizontal);
  if(horizontal >= 4) {
    //showWinningMove(a, b, a, ii);
    return true;
  }
  
  //check for diagonal 1 (\)
  for(i = a - 1, ii = b - 1; matrix[i][ii] == player && i>=0 && ii >=0; diagonal1++, i --, ii --);//up and left
  i++,ii++;
  for(i = a + 1, ii = b + 1; matrix[i][ii] == player && i<=5 && ii <=6; diagonal1++, i ++, ii ++);//down and right
  i--,ii--;
  if(diagonal1 >= 4) {
    //showWinningMove(a, b, i, ii);
    return true;
  }
  
  //check for diagonal 2(/)
  for(i = a - 1, ii = b + 1; matrix[i][ii] == player && i>=0 && ii <= 6; diagonal2++, i --, ii ++);//up and right
  i++,ii--;
  for(i = a + 1, ii = b - 1; matrix[i][ii] == player && i<=5 && ii >=0;  diagonal2++, i ++, ii --);//up and left
  i--,ii++;
  if(diagonal2 >= 4) {
    //showWinningMove(a, b, i, ii);
    return true;
  }
  
  return false;
}


void game()
{
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

    if (y1Value > 700 && joyMoved == false) {
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

      joyMoved = true;


    }

    if (x1Value <= 600 && x1Value >= 400)// && yValue >= 400)
      joyMoved = false;

    if (check(pos1)); //return;
    //Serial.println(check(pos1));
    
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
      

    }

    if (x2Value <= 600 && x2Value >= 400)// && yValue >= 400)
      joyMoved = false;

      if (check(pos2));
      //Serial.println(check(pos2));
      //return;
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



void loop()
{
  //mainMenu();

  
  game();
  //welcome();

  
  
}