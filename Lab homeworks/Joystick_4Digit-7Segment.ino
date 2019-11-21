const int pinSW = A2;
const int pinX = A0;
const int pinY = A1;

int xValue = 0;
int yValue = 0;
int swValue = 0;

bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;
bool mode = false; //false - change digit ; true - change value

int number = 0;
int digit = 0;

int swState, lastSwState = 0;

const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfValues = 10;

int dpState = 1;

int currentNumber = 0;
unsigned long delayCounting = 50;
unsigned long lastIncreasing = 0;

int digitValue[noOfDisplays] = {
  0, 0, 0, 0
};

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
};

byte digitMatrix[noOfValues][segSize - 1] = {
  // a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void displayNumber(byte, byte);
void showDigit(int);
void changeValue();
void changeDigit();
void fullNumberDisplay();

void setup() {
  // put your setup code here, to run once:

  for (int i = 0; i < segSize; i++) { //segsize - 1
    pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < segSize; i++) { //-1
    pinMode(digits[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  Serial.begin(9600);

}

void loop() { //===================================
  
  if (mode == false) { 
    changeDigit();            //mode == false <==> Change Digit mode
  } else {
    changeValue();           //mode == true <==> Change Value mode
  }
  fullNumberDisplay();        //function for showing the number
  Serial.println(dpState);

} //=============================================

void fullNumberDisplay()
{
  int full = 0,localDigit = 0, lastDigit; //we memorise the number in full and we display it using while
  
  for (int i = 3; i > -1; i--) {
    full = full * 10 + digitValue[i];
  }

  while (full != 0) {
    lastDigit = full % 10; // get the last digit
    showDigit(localDigit);
    displayNumber(digitValue[localDigit], LOW);
    localDigit++; // move to next display
    full = full / 10;
    delay(5);
  }
}

void clearNumber(byte digit, byte decimalPoint) {
  for(int j = 3; j > -1 ; j--){
  showDigit(digit);
  for (int i = 0; i < segSize - 1; i++) {  //this is for deleting numbers, but it helps with the dp as well
    digitalWrite(segments[i], 0);
  }
}
  digitalWrite(segments[segSize - 1], decimalPoint);
}

void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);  //this is for displaying numbers
  }
  digitalWrite(segments[segSize - 1], decimalPoint);
}

void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);    //this is for a digit to be activated
  }
  digitalWrite(digits[num], LOW);
}

void changeDigit() {
  xValue = analogRead(pinX);
  
  if (xValue > maxThreshold && joyMoved == false) {
    if (digit > 0) {
      digit--;
    } else {
      digit = 3;
    }
    joyMoved = true;
  }     

  if (xValue < minThreshold && joyMoved == false) {     //the code for toggling on bits
    if (digit < 3) {
      digit++;
    } else {
      digit = 0;
    }
    joyMoved = true;
  }


  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
  clearNumber(digitValue[digit], HIGH);
  delay(5);
  swState = digitalRead(pinSW);
  if (swState !=  lastSwState) {
    if (swState == LOW) {
      dpState = !dpState;   //if the button is pushed, the mode changes
      mode = !mode;
    }
  }
  lastSwState = swState;
  
}

void changeValue() {  //similar to changeValue, but we are toggling between values for the selected digit
  int lastDigit;
  number = currentNumber;
  
  yValue = analogRead(pinY);
  if (yValue < minThreshold && joyMoved == false) {
    if (digitValue[digit] > 0) {
      number--;
      currentNumber--;
      digitValue[digit]--;
    } else {
      number = 9;
      currentNumber = 9;
      digitValue[digit] = 9;
    }
    joyMoved = true;
  }

  if (yValue > maxThreshold && joyMoved == false) {
    if (digitValue[digit] < 9) {
      digitValue[digit]++;
    } else {
      digitValue[digit] = 0;
    }
    joyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }

  swState = digitalRead(pinSW);
  if (swState !=  lastSwState) {
    if (swState == LOW) {
      dpState = !dpState;
      mode = !mode;
    }
  }
  lastSwState = swState;

  displayNumber(digitValue[digit], dpState);

}
