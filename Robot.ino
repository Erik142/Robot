#define SENSORINPUT 9 //Avståndssensor input-sensor
#define SENSORTRIGGER 8 //Avståndssensor trigger-sensor
#define MOTOR1PIN1 3
#define MOTOR1PIN2 2
#define MOTOR2PIN1 5
#define MOTOR2PIN2 4


int angle = 0;
int mindistance = 20;
int distances[4];

/*Funktion för att färdas framåt, skall utvecklas
med ett argument i form utav avstånd*/
void DriveForward() {
  //Serial.println("Spinning forward");
  
  digitalWrite(MOTOR1PIN1, HIGH);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, HIGH);
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
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, LOW);
}  

/*Funktion för att svänga till höger, beroende utav
vinkeln i grader. Denna funktion skall förfinas ytterligare.*/
void TurnRight(int Grader) {
  
  //Serial.println("Turning right");
  
  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, HIGH);
  
  digitalWrite(MOTOR2PIN1, HIGH);
  digitalWrite(MOTOR2PIN2, LOW);
  
  delay(5*(Grader+38));
  angle -= Grader*180/PI;
}

//Funktion för att svänga vänster, samma som ovan
void TurnLeft(int Grader) {
  //Serial.println("Turning left");
  
  digitalWrite(MOTOR1PIN1, HIGH);
  digitalWrite(MOTOR1PIN2, LOW);
  
  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, HIGH);
  
  delay(5*(Grader+38));
  angle += Grader*180/PI;
}

//Funktion för att omvandla avståndssensorns data till avstånd.
int MeasureDistance(){
    digitalWrite(SENSORTRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSORTRIGGER, LOW);
  
  int distance = pulseIn(SENSORINPUT, HIGH);

  distance = (distance * (0.03432/2));
  
  return distance; 
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
  if (Serial.available() > 0)
  {
    mindistance = Serial.parseInt();
  }
  
  
  if(MeasureDistance() > mindistance){
    DriveForward();
    delay(100);
  } 
  else {
    for (int i = 0; i < 3; i++)
    {
      distances[i] = NULL;
    }
    
    
    
    Stop();
    //DriveBackwards();
    delay(500);
    
    
    TurnLeft(45);
    
    Stop();
    distances[0] = MeasureDistance();
    delay(100);
    
    TurnLeft(45);
    
    Stop();
    distances[1] = MeasureDistance();
    delay(100);
    
    TurnRight(135);
    
    Stop();
    distances[2] = MeasureDistance();
    delay(100);
    
    TurnRight(45);
    
    Stop();
    distances[3] = MeasureDistance();
    delay(100);
    
    /*Serial.println("Measured distances:");
    for (int x = 0; x < 4; x++)
    {
    Serial.println(distances[x]);
    }*/
    
    int Position = FindMax(distances, 0, 3);
    switch(Position)
    {
       case 0:
        TurnLeft(135);
        
        Stop();
        delay(1000);
        break;
       case 1:
        TurnLeft(180);
        
        Stop();
        delay(1000);
        break;
       case 2: 
        TurnLeft(45);
       
       Stop();
        delay(1000);
        break;
       default:
       Stop();
       delay(1000);
        break;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
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
}

void loop() {
  // put your main code here, to run repeatedly: 
 
  Main();
  
  
}

//Script för de olika test vi ska genomföra/har genomfört 

void TurnTest1(){
  delay(10000);
  for(int i=1 ; i<13 ; i++){
    TurnLeft(10*i);
    Stop();
    delay(10000);
  }
  
}

void TurnTest2(){
  delay(5000);
  TurnLeft(50000);
  
}


void TurnTest3(){
  //Test av färdig svängfunktion
  delay(1000);
  TurnLeft(45);
  Stop();
  delay(100);
  TurnLeft(45);
  Stop();
  delay(100);
  TurnRight(135);
  Stop();
  delay(100);
  TurnRight(45);
  Stop();
  delay(100);
  TurnLeft(90);
  Stop();
  delay(4500);
}

void SensorTest(){
  //Test av avståndssensorn
  
  Serial.println(MeasureDistance());
  delay(100);
}
