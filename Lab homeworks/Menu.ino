#include <LiquidCrystal.h>
const int RS = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);


const int pinSW = A2;
const int pinX = A0;
const int pinY = A1;

int xValue = 0;
int yValue = 0;
int swValue = 0;
int swState, lastSwState = 0;

bool joyMovedX = false;
bool joyMovedY = false;
int minThreshold = 400;
int maxThreshold = 600;

int level = 1;
long lastUpdate = 0;
long score = 0, highScore = 0;
int i = 0;

void mainM();

void setup() {

  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();

  Serial.begin(9600);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
}

void showCongrats()
{

  long startCongr = millis();
  //lcd.clear();
  while (joyMovedY == false) { //(yValue <= maxThreshold && yValue >= minThreshold){

    xValue = analogRead(pinX);
    yValue = analogRead(pinY);


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

    /*if(yValue > maxThreshold || yValue < minThreshold){
      joyMovedY = true;
      lcd.clear();
      return;
      }
    */

    swState = digitalRead(pinSW);


    if (swState == LOW) {
      lastSwState = swState;
      lcd.clear();
      return;
    }



  }

}

void startGame() {

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);


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

    yValue = analogRead(pinY);

    lcd.setCursor(1, 0);
    lcd.print("HighScore:                ");
    lcd.setCursor(1, 1);
    lcd.print(highScore);
    if ((yValue > maxThreshold || yValue < minThreshold) && millis() - startH > 500) {
      joyMovedY = true;
      lcd.clear();
      return;
    }
  }

}

void settings() {

  joyMovedY = false;
  long startS = millis();
  lcd.clear();
  while ( joyMovedY == false) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    lcd.setCursor(0, 0);
    lcd.print("Level:");
    lcd.setCursor(7, 0);
    lcd.print(level);

    if (xValue < minThreshold && joyMovedX == false) { //the levels can be between 1 and 999
      level--;
      if (level < 1)
        level = 1;
      joyMovedX = true;
    }
    if (xValue > maxThreshold && joyMovedX == false) {
      level++;
      if (level > 999)
        level = 1;
      joyMovedX = true;
    }

    if (xValue <= maxThreshold && yValue >= minThreshold)
      joyMovedX = false;

    if ((yValue > maxThreshold || yValue < minThreshold) && millis() - startS > 500) {
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
  lcd.print("Settings      ");

  int posR[3] = {0, 6, 0}, posC[3] = {0, 0, 1}; //the positions where the arrow can be

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  lcd.setCursor(posR[i % 3], posC[i % 3]);
  lcd.print(">");

  if (xValue > maxThreshold && joyMovedX == false) {

    lcd.setCursor(posR[i % 3], posC[i % 3]); //we delete the present arrow
    lcd.print(" ");

    i++; //going for the next one
    if (i == 3)
      i = 0;

    lcd.setCursor(posR[i % 3], posC[i % 3]); //posR and posC are for the possible position on columns and rows for the arrow

    lcd.print(">");

    joyMovedX = true;
  }

  if (xValue < minThreshold && joyMovedX == false) {

    lcd.setCursor(posR[i % 3], posC[i % 3]);
    lcd.print(" ");
    i--;
    if (i < 0) i = 2;

    lcd.print(" ");
    lcd.setCursor(posR[i], posC[i]); //posR and posC are for the possible position on columns and rows for the arrow
    lcd.print(">");

    joyMovedX = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMovedX = false;
  }

  if ((yValue > maxThreshold || yValue < minThreshold) && joyMovedY == false) {
    if (i == 0) {
      lcd.clear();
      startGame();
    }
    if (i == 1) {  // depending on the i, we know what option is selected
      lcd.clear();
      high();
    }
    if (i == 2) {
      lcd.clear();
      settings();
    }
    joyMovedY = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }

}

void loop() {

  mainMenu();

}
