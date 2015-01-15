#include <SoftwareSerial.h>
//#include <NewPing.h>

#define MAXDISTANCE 400 //Maximalt avstånd som avståndssensorn mäter
#define SENSORINPUT 9 //Avståndssensor input-sensor
#define SENSORTRIGGER 8 //Avståndssensor trigger-sensor
#define MOTOR1PIN1 10
#define MOTOR1PIN2 3
#define MOTOR2PIN1 5
#define MOTOR2PIN2 6
#define TXPIN 13
#define RXPIN 12

short mode = 1;
short lastmode = 1;
float pwm_left_turbo = 243.3;
float pwm_left_normal = 168;
short pwm_right_normal = 180;
int angle = 0;
int mindistance = 20;
int distances[8];
int lastDistance = MAXDISTANCE;

SoftwareSerial mySerial(RXPIN, TXPIN);

void turbo() {
  Serial.println("TURBO");
  mySerial.println("TURBO");
  
  digitalWrite(MOTOR1PIN1, HIGH);
  digitalWrite(MOTOR1PIN2, LOW);
  
  //digitalWrite(MOTOR2PIN1, HIGH);
  //digitalWrite(MOTOR2PIN2, LOW);
  
  analogWrite(MOTOR2PIN1, pwm_left_turbo);
  digitalWrite(MOTOR2PIN2, LOW);
}
  
/*Funktion för att färdas framåt, skall utvecklas
med ett argument i form utav avstånd*/
void DriveForward() {
  Serial.println("Spinning forward");
  mySerial.println("Spinning forward");
  
  analogWrite(MOTOR1PIN1, pwm_right_normal);
  digitalWrite(MOTOR1PIN2, LOW);
  
  //digitalWrite(MOTOR2PIN1, HIGH);
  //digitalWrite(MOTOR2PIN2, LOW);
  
  analogWrite(MOTOR2PIN1, pwm_left_normal);
  digitalWrite(MOTOR2PIN2, LOW);
}

/*Funktion för att färdas bakåt, här skall också
ett avståndsargument utvecklas*/
void DriveBackwards() {
  //Serial.println("Spinning backwards");
  
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, HIGH);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, HIGH);
}

//Funktion för att stanna helt
void Stop(){
  Serial.println("Stopping.");
  mySerial.println("Stopping.");
  
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, LOW);
}  

/*Funktion för att svänga till höger, beroende utav
vinkeln i grader. Denna funktion skall förfinas ytterligare.*/
void TurnRight(int Grader) {
  Serial.print("Turning ");
  Serial.print(Grader);
  Serial.println(" degrees right");
  mySerial.print("Turning ");
  mySerial.print(Grader);
  mySerial.println(" degrees right");
  
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, HIGH);
  
  digitalWrite(MOTOR2PIN1, HIGH);
  digitalWrite(MOTOR2PIN2, LOW);
  
  delay(2.6566*Grader+55.2476);
  angle -= Grader*180/PI;
  Stop();
}

//Funktion för att svänga vänster, samma som ovan
void TurnLeft(int Grader) {
  Serial.print("Turning ");
  Serial.print(Grader);
  Serial.println(" degrees left");
  mySerial.print("Turning ");
  mySerial.print(Grader);
  mySerial.println(" degrees left");
  
  analogWrite(MOTOR1PIN1, 245);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, HIGH);
  
  delay(2.6566*Grader+55.2476);
  angle += Grader*180/PI;
  Stop();
}

//Funktion för att omvandla avståndssensorns data till avstånd.
int MeasureDistance(){
  digitalWrite(SENSORTRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(SENSORTRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSORTRIGGER, LOW);
  
  long distance = pulseIn(SENSORINPUT, HIGH) * (0.03432/2);
  if (distance > MAXDISTANCE)
  {
    lastDistance = MAXDISTANCE;
    return MAXDISTANCE;
  }
  else
  {
  if (distance < 10 && lastDistance == MAXDISTANCE || distance < 0) {
    return lastDistance;
  }
  else
  {
    lastDistance = distance;
    return distance;
  }
  }
  
}

int measureDistance(int iterations) {
  int returnValue = 0;
  
  for (int i = 0; i < iterations; i++)
  {
    returnValue += MeasureDistance();
  }
  
  return returnValue/iterations;
}

//Funktion för att hitta största värdet i en array
int FindMax(int ARRAY[], byte START, byte END)
{
  int MAXIMUM, LOCATION;
  
  MAXIMUM = ARRAY[START];
  LOCATION = START;
  for(byte i = START + 1; i < END; i++)
  {
    if(ARRAY[i] > MAXIMUM)
    {
      MAXIMUM = ARRAY[i];
      LOCATION = i;
    }
  }
  
  return LOCATION;
}

//Huvudfunktion där roboten undviker hinder

void Main(){
  
 //Möjlighet att ändra avståndet ifrån datorn
  /*if (Serial.available() > 0)
  {
    int receivedValue = Serial.parseInt();
    if (receivedValue > 0)
    {
      mindistance = receivedValue;
      
    mySerial.print("New mindistance = ");
    mySerial.println(mindistance);
    
    Serial.print("New mindistance = ");
    Serial.println(mindistance);
    }
  }
  
  if (mySerial.available() > 0)
  {
    int receivedValue = mySerial.parseInt();
    if (receivedValue > 0)
    {
      mindistance = receivedValue;
      
    mySerial.print("New mindistance = ");
    mySerial.println(mindistance);
    
    Serial.print("New mindistance = ");
    Serial.println(mindistance);
    }
    
  } */ 
  
  int measuredDistance = MeasureDistance();
  mySerial.println(measuredDistance);
  Serial.println(measuredDistance);
  
  if(measuredDistance >= 100) {
    turbo();
    //delay(100);
  }
  else if(measuredDistance > mindistance){
    DriveForward();
    //delay(100);
  } 
  else {
    for (int i = 0; i < 3; i++)
    {
      distances[i] = NULL;
    }
    
    
    
    Stop();
    delay(500);
    
    
    TurnLeft(45);
    
    distances[0] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[1] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[2] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[3] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[4] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[5] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    distances[6] = MeasureDistance();
    delay(100);
    
    int Position = FindMax(distances, 0, 7);
    switch(Position)
    {
       case 0:
        TurnLeft(90);
        
        delay(1000);
        break;
       
       case 1:
        TurnLeft(135);
        
        delay(1000);
        break;
       
       case 2: 
        TurnLeft(180);
       
        delay(1000);
        break;
        
        case 3: 
        TurnRight(135);
       
        delay(1000);
        break;
        
        case 4: 
        TurnRight(90);
       
        delay(1000);
        break;
        
        case 5: 
        TurnRight(45);
       
        delay(1000);
        break;
        
       default:
       
       delay(1000);
       break;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(57600);
  mySerial.begin(9600);
  
  mySerial.print("AT");
  delay(1000);
  mySerial.print("AT+NAMERobot");
  delay(1000);
  mySerial.print("AT+PIN1337");
  mySerial.println();
  
pinMode(MOTOR1PIN1, OUTPUT);
pinMode(MOTOR1PIN2, OUTPUT);

pinMode(MOTOR2PIN1, OUTPUT);
pinMode(MOTOR2PIN2, OUTPUT);

pinMode(SENSORINPUT, INPUT);
pinMode(SENSORTRIGGER, OUTPUT);

digitalWrite(MOTOR1PIN1, LOW);
digitalWrite(MOTOR1PIN2, LOW);

digitalWrite(MOTOR2PIN1, LOW);
digitalWrite(MOTOR2PIN2, LOW);
 
Serial.println("Hello world!");
mySerial.println("Hello world!");
}

void loop() {
  // put your main code here, to run repeatedly: 
  
  //TurnTest4();
  
  if (Serial.available() > 0)
  {
    int value = Serial.parseInt();
    if (value != 0)
    {
      mode = value;
    }
  }
  
  if (mySerial.available() > 0)
  {
    int value = mySerial.parseInt();
    if (value != 0)
    {
      mode = value;
    }
  }
 
 if (mode != lastmode)
 {
   switch(mode) {
  case 1: 
  Serial.println("Running Main()");
  mySerial.println("Running Main()");
  break;
  case 2:
  Stop();
  Serial.println("Running SensorTest()");
  mySerial.println("Running SensorTest()");
  break;
  default:
  Serial.println("Need mode...");
  mySerial.println("Need mode...");
  break;
 }
 }
  
 switch(mode) {
  case 1: 
  Main();
  lastmode = 1;
  break;
  case 2:
  SensorTest();
  lastmode = 2;
  break;
  default:
  break;
 }
}

//Script för de olika test vi ska genomföra/har genomfört 

void TurnLeftms(int miliseconds)
{
  Serial.print("Turning left for ");
  Serial.print(miliseconds);
  Serial.println(" miliseconds");
  mySerial.print("Turning left for ");
  mySerial.print(miliseconds);
  mySerial.println(" miliseconds");
  
  digitalWrite(MOTOR1PIN1, HIGH);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, HIGH);
  
  delay(miliseconds);
  
  Stop();
}

void TurnRightms(int miliseconds)
{
  Serial.print("Turning right for ");
  Serial.print(miliseconds);
  Serial.println(" miliseconds");
  mySerial.print("Turning right for ");
  mySerial.print(miliseconds);
  mySerial.println(" miliseconds");
  
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, HIGH);
  
  digitalWrite(MOTOR2PIN1, HIGH);
  digitalWrite(MOTOR2PIN2, LOW);
  
  delay(miliseconds);
  
  Stop();
}

void TurnTest1(){
  delay(10000);
  for(int i=15 ; i<21 ; i++){
    TurnLeftms(10*i);
    delay(20000);
  }
  
}

void TurnTest2(){
  delay(5000);
  TurnLeft(50000);
  
}


void TurnTest3(){
  //Test av färdig svängfunktion
    delay(5000);
    TurnLeft(45);
    
    distances[0] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[1] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[2] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[3] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[4] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[5] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    
    distances[6] = MeasureDistance();
    delay(1000);
    
    TurnLeft(45);
    delay(1000);
}

void TurnTest4() {
  delay(4000);
  TurnRight(360);
  
  delay(4000);
  TurnLeft(360);
  
  delay(4000);
  TurnRight(180);
  
  delay(4000);
  TurnLeft(180);
  
  delay(4000);
}

void SensorTest(){
  //Test av avståndssensorn
  long distance = MeasureDistance();
  Serial.print("Distance = ");
  Serial.println(distance);
  mySerial.print("Distance = ");
  mySerial.println(distance);
  delay(100);
}

void driveTest()
{
  DriveForward();
}
