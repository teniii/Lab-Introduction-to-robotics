const int passiveBuzzer = A0;
const int activeBuzzer = 9;
const int pushButton = 2;

int buttonState = 0;
int lastButtonState = 0;
int passiveBuzzerVal = 0;

bool activeBuzzerState = false;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long buzzerDelay = 5000;
unsigned long startWaiting = 0;

void listening();

void changeActiveBuzzerState();

void setup() {
  // put your setup code here, to run once:
  pinMode(passiveBuzzer, INPUT);
  pinMode(activeBuzzer, OUTPUT);
  pinMode(pushButton, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (activeBuzzerState == false) { //if the active buzzer doesn't yell, we're waiting for the knock
    listening(); 
      }
      
  else {
    changeActiveBuzzerState(); //if it's on, we're waiting for the button to be pushed
  }
}

void listening() {
  
  int passiveBuzzerVal = analogRead(passiveBuzzer);
  Serial.println(passiveBuzzerVal);
  if (passiveBuzzerVal > 25) {                      //the 25 value it's the barrier between the noise and a real knock
    startWaiting = millis();                        //the moment when we are starting to wait the 5 sec
    while(millis() - startWaiting < buzzerDelay){}  //delay
        activeBuzzerState = true;                   //the active buzzer will start to yell
        digitalWrite(activeBuzzer, activeBuzzerState);
  }
      
}

void changeActiveBuzzerState() {
  
  int reading = digitalRead(pushButton);            //we memorize the current state
  
  if (reading != lastButtonState) {                 //if it's different then the previous one we start the clock
    lastDebounceTime = millis();                  
  }

  if (millis() - lastDebounceTime > debounceDelay) { //the delay
    if (reading != buttonState) {
      buttonState = reading;                        //we chage the button state on the way down of the push ( LOW )
      if (buttonState == LOW) {   
        activeBuzzerState = !activeBuzzerState;
        digitalWrite(activeBuzzer, activeBuzzerState);
      }
    }
  }

  lastButtonState = reading; //we memorize the new state
}
