#include <Servo.h>

Servo servo;

const int pushButton1 = 2;
const int pushButton2 = 4;
const int pushButton3 = 7;
const int servoPin = 9;
const int buzzerPin = 11;

const int numberOfActions = 4;
 int key[numberOfActions] = {1, 2, 2, 3};
int actionVector[numberOfActions];
const int numberAttempts = 4;
int attemptsLeft;

int buttonState1;
int lastButtonState1 = LOW;
int buttonState2;
int lastButtonState2 = LOW;
int buttonState3;
int lastButtonState3 = LOW;
int servoState = HIGH; // low - The gate is open
int alarmState = LOW; // low - is off
int alarmTone = 700; 

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long debounceDelay = 50;
unsigned long startAlarm;
unsigned long alarmPeriod = 8000;


void noise();

void setup() {
  servo.attach(servoPin);
  pinMode(pushButton1, INPUT_PULLUP);
  pinMode(pushButton2, INPUT_PULLUP);
  pinMode(pushButton3, INPUT_PULLUP);
  Serial.begin(9600);
  attemptsLeft = 5; // the activation consumes an attempt
  noTone(buzzerPin);
}

void loop()
{
  int i;
  int reading1, reading2, reading3;

  if (alarmState == HIGH)
    {
      /*
      if (millis() - startAlarm > alarmPeriod || servoState == LOW)
        noTone(buzzerPin), alarmState = LOW ;
      else
        tone(buzzerPin, alarmTone);
        */
        alarm();
    }
  else 
    noTone(buzzerPin);

  for ( i = 0; i < numberOfActions; )
  {
    //FIRST button
    
    reading1 = digitalRead(pushButton1);  //we read the value of the button
    
    if (reading1 != lastButtonState1) {  //if its diferrent then the previous one
      lastDebounceTime1 = millis();     //we save the time when the action started
    }
    if ((millis() - lastDebounceTime1) > debounceDelay) { // if it's a user action and not a noise from the circuit

      if (reading1 != buttonState1) { //if the read state is different than the saved state
        buttonState1 = reading1;      //we update the state

        if (buttonState1 == HIGH) {   //if the button is pressed, we put in the vector of actions the number of the button. In this case, 1 
          actionVector[i] = 1;
          if (servoState == LOW && i != 3) //if the servo is in the open position and the number of actions is different then 3 (the sequence is incomplete)
          {
            servo.write(90);  //we put the servo in the close position
            servoState = !servoState; // changing servo's state
            i = -1;           //i will be incremented, so in the end will be 0 (it will be a new sequence)
            noise();          //noise so you know that the box is closed
          }
          i++;                //i represents the number of actions made by the user.
                              //after a button is pushed, it's stored in the vector of actions and
                              // we go on the next position of the vector
        }
      }
    }

    lastButtonState1 = reading1; ///updating the last state of the button1
 
    //SECOND button

    reading2 = digitalRead(pushButton2);  //the same procedure as the first button
    if (reading2 != lastButtonState2) {
      lastDebounceTime2 = millis();
    }
    if ((millis() - lastDebounceTime2) > debounceDelay) {

      if (reading2 != buttonState2) {
        buttonState2 = reading2;

        if (buttonState2 == HIGH) {
          actionVector[i] = 2;
          if (servoState == LOW && i != 3)
          {
            servo.write(90);
            servoState = !servoState;
            i = -1;
            noise();
          }
          i++;

        }
      }
    }

    lastButtonState2 = reading2;

    //THIRD button

    reading3 = digitalRead(pushButton3); //the same procedure as the first two buttons
    if (reading3 != lastButtonState3) {
      lastDebounceTime3 = millis();
    }
    if ((millis() - lastDebounceTime3) > debounceDelay) {

      if (reading3 != buttonState3) {
        buttonState3 = reading3;

        if (buttonState3 == HIGH) {
          actionVector[i] = 3;
          if (servoState == LOW && i != 3)
            servo.write(90), servoState = !servoState, i = -1, noise();
          i++;

        }
      }
    }

    lastButtonState3 = reading3;
  }

  if (i == numberOfActions && servoState == HIGH) //if the sequence is complete and the box is closed
  { 
      noiseEndOfSequence(attemptsLeft - 1); //we announce the user how many attempts has left
      attemptsLeft--;   //we decrement the attempts
  }

  


  for (i = 0; i < numberOfActions; i++) //checking is the sequence is correct
  {
    if (key[i] != actionVector[i]) //if one element is not ok, the i will stop on the current value
      break;
  }

  if (i == numberOfActions)     //if is correct, unlock and change the state and make a noise to announce
    {
    servo.write(0);       //unlock the box
    servoState = LOW;     //update the state
    noise();              //anouncing the user that the box is opened
    attemptsLeft = 4;     //we reset the number of attempts for when it's getting closed
    changePassword();     //entering in the reset of the password process
    }
    
  else                          //if the sequence is wrong, we lock the servo and update the state
  {
    servo.write(90);      
    servoState = HIGH;
  }

  if (attemptsLeft < 1 && alarmState == LOW)
    alarmState = !alarmState, tone(buzzerPin, alarmTone);//, alarm();

  if (attemptsLeft < 1)
    attemptsLeft = 4;

  
}


void noise()
{
  unsigned long timeStart = millis();
  const int noisePeriod = 200;
  const int noiseValue = 350;
  while (millis() - timeStart < noisePeriod)
  {
    tone(buzzerPin, noiseValue, noisePeriod);
  }

  noTone(buzzerPin);
}

void noiseEndOfSequence(int n)
{
  unsigned long timeStart = millis();
  const int noisePeriod = 80;
  const int noiseValue = 1400;
  for(int i = 0; i < n; i++)
  {
    tone(buzzerPin, noiseValue, noisePeriod);
    delay(110);
  }

  noTone(buzzerPin);
}

void alarm()
{
  unsigned long timeStart = millis();

  while (millis() - timeStart < alarmPeriod) //while the period of the alarm is finished, the alarm will buzz
  {
    tone(buzzerPin, alarmTone, alarmPeriod);
  }

  alarmState = !alarmState; //once  it's over, we change the state

  noTone(buzzerPin); //we make sure that there are not any other sounds
  
}

void changePassword()
{
  int reading1, reading2, reading3;
  
  for (int  i = 0; i < numberOfActions; )  //Here we have the same procedure as in the loop
  {                                        //For each button, we check if it's state has changed
                                           //And if the button was pushed, the key vector
                                           //In which is stored the password, will be over-written
    reading1 = digitalRead(pushButton1); 
    if (reading1 != lastButtonState1) {
      lastDebounceTime1 = millis();
    }
    if ((millis() - lastDebounceTime1) > debounceDelay) {

      if (reading1 != buttonState1) {
        buttonState1 = reading1;

        if (buttonState1 == HIGH) {
          key[i] = 1;
          i++;
        }
      }
    }

    lastButtonState1 = reading1;

    //second button

    reading2 = digitalRead(pushButton2);
    if (reading2 != lastButtonState2) {
      lastDebounceTime2 = millis();
    }
    if ((millis() - lastDebounceTime2) > debounceDelay) {

      if (reading2 != buttonState2) {
        buttonState2 = reading2;

        if (buttonState2 == HIGH) {
          key[i] = 2;
          i++;

        }
      }
    }

    lastButtonState2 = reading2;
    
    //THIRD button

    reading3 = digitalRead(pushButton3); 
    if (reading3 != lastButtonState3) { 
      lastDebounceTime3 = millis();    
    }
    if ((millis() - lastDebounceTime3) > debounceDelay) {

      if (reading3 != buttonState3) {
        buttonState3 = reading3;

        if (buttonState3 == HIGH) {
          key[i] = 3;
          i++;

        }
      }
    }

    lastButtonState3 = reading3;

  }
  
noise(); //Anouncing the user that  the password has been saved
}
