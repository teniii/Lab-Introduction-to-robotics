const int pot1Pin=A0;
const int pot2Pin=A3;
const int pot3Pin=A5;

const int redPin=3;
const int greenPin=6;
const int bluePin=11;

//int sum[3]={0}, count=0, numberValues=100;

void setup() {
  /* put your setup code here, to run once:*/
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  
  pinMode(pot1Pin,INPUT);
  pinMode(pot2Pin,INPUT);
  pinMode(pot3Pin,INPUT);
  Serial.begin(9600);
  
}



void loop() {
  int pot1Val=analogRead(pot1Pin);
  int pot2Val=analogRead(pot2Pin);
  int pot3Val=analogRead(pot3Pin);
  //Easier than map, I'll divide the potVal by 4  
  
  analogWrite(bluePin,pot1Val/32); //The standar value would be pot1Val/4 <=> map(pot1Val,0,1023,0,255)
  analogWrite(greenPin,pot2Val/32);//But I have some oscillations from the potentiometers 
  analogWrite(redPin,pot3Val/32);  //and they are not so obvious when the values are lower
  
  //analogWrite(bluePin,map(pot1Val,0,1023,0,255)); The second method, using map
  //analogWrite(greenPin,map(pot2Val,0,1023,0,255));
  //analogWrite(redPin,map(pot3Val,0,1023,0,255)); 

/*
 * This would be a solution for my oscillations, diminishing them
 * But it will still appear some combinations of colours when it shouldn't
 * 
 if(count==numberValues)
  {
    analogWrite(bluePin,sum[0]/count);
    analogWrite(greenPin,sum[1]/count);
    analogWrite(redPin,sum[2]/count);
    count=0;
    sum[0]=0; 
    sum[1]=0;
    sum[2]=0; 
  }
  
  sum[0]+=pot1Val/4;
  sum[1]+=pot2Val/4;
  sum[2]+=pot3Val/4;
  count++;
  */
}
