#include <SoftwareSerial.h>

#define MAXDISTANCE 400 //Maximalt avstånd som avståndssensorn mäter
#define SENSORINPUT 9   //Avståndssensor input-sensor
#define SENSORTRIGGER 8 //Avståndssensor trigger-sensor
#define MOTOR1PIN1 10   //Pin för att styra logik för motorerna
#define MOTOR1PIN2 3
#define MOTOR2PIN1 5
#define MOTOR2PIN2 6
#define TXPIN 13        //Transmit-pin för bluetooth
#define RXPIN 12        //Receive-pin för bluetooth

short mode = 2;
short lastmode = 2;
float pwm_left_turbo = 243.3;
float pwm_left_normal = 168;
short pwm_right_normal = 180;
int angle = 0;
int mindistance = 20;
int distances[8];
int lastDistance = MAXDISTANCE;

SoftwareSerial mySerial(RXPIN, TXPIN);
/*Använd bluetoothmodulen på pins som ej är interna TX och RX
vilket gör att både bluetooth och COM över USB kan användas samtidigt*/

//Turbofunktion, roboten åker snabbare framåt
void turbo() {
  Serial.println("TURBO");
  mySerial.println("TURBO");

  digitalWrite(MOTOR1PIN1, HIGH);
  digitalWrite(MOTOR1PIN2, LOW);

  analogWrite(MOTOR2PIN1, pwm_left_turbo);
  digitalWrite(MOTOR2PIN2, LOW);
}

// Funktion för att färdas framåt
void DriveForward() {
  Serial.println("Spinning forward");
  mySerial.println("Spinning forward");

  analogWrite(MOTOR1PIN1, pwm_right_normal);
  digitalWrite(MOTOR1PIN2, LOW);

  analogWrite(MOTOR2PIN1, pwm_left_normal);
  digitalWrite(MOTOR2PIN2, LOW);
}

//Funktion för att färdas bakåt
void DriveBackwards() {

  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, HIGH);

  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, HIGH);
}

//Funktion för att stanna helt
void Stop() {
  Serial.println("Stopping.");
  mySerial.println("Stopping.");

  digitalWrite(MOTOR1PIN1, LOW);
  digitalWrite(MOTOR1PIN2, LOW);

  digitalWrite(MOTOR2PIN1, LOW);
  digitalWrite(MOTOR2PIN2, LOW);
}

/*Funktion för att svänga till höger, beroende utav
vinkeln i grader.*/
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

  //Använd den funktion som approximerats via experiment
  delay(2.6566 * Grader + 55.2476);
  angle -= Grader * 180 / PI;
  Stop();
}

/*Funktion för att svänga vänster, beroende utav
vinkeln i grader.*/
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

  //Använd den funktion som approximerats via experiment
  delay(2.6566 * Grader + 55.2476);
  angle += Grader * 180 / PI;
  Stop();
}

//Funktion för att omvandla avståndssensorns data till avstånd.
long MeasureDistance() {
  digitalWrite(SENSORTRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(SENSORTRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSORTRIGGER, LOW);

  /*Använd datatypen long och multiplicera tiden ifrån pulseIn
  med faktorn 0.03432/2 vilket ger avståndet till hindret som hittats.
  Detta fungerar eftersom 0.03432 är ljudhastigheten i cm/us.
  Faktorn 1/2 är eftersom avståndssensorn mäter tiden för pulsen
  att träffa objektet och komma tillbaka. Vi vill bara ha reda på
  tiden fram till att pulsen träffat objektet. De vilkorssatser
  som används nedan är för att förhindra avståndssensorn att mäta
  ett negativt värde vilket uppstod tidigare.*/
  long distance = pulseIn(SENSORINPUT, HIGH) * (0.03432 / 2);
  if (distance > MAXDISTANCE)
  {
    lastDistance = MAXDISTANCE;
    return MAXDISTANCE;
  }
  else if (distance < 6 && lastDistance == MAXDISTANCE || distance < 0) {
    return lastDistance;
  }
  else
  {
    lastDistance = distance;
    return distance;
  }


}

/*Mät avståndet iterations antal gånger och beräkna sedan
medelvärdet utav dessa värden.*/
int measureDistance(int iterations) {
  int returnValue = 0;

  for (int i = 0; i < iterations; i++)
  {
    returnValue += MeasureDistance();
  }

  return returnValue / iterations;
}

//Funktion för att hitta största värdet i en array
int FindMax(int ARRAY[], byte START, byte END)
{
  int MAXIMUM, LOCATION;

  /*Jämför de olika värden som finns i ARRAY.
    Om ett ARRAY[i] är större än MAXIMUM, ansätt 
    MAXIMUM = ARRAY[i]. Ansätt också LOCATION = i.
    När alla index jämförts, returnera i.*/
  MAXIMUM = ARRAY[START];
  LOCATION = START;
  for (byte i = START + 1; i < END; i++)
  {
    if (ARRAY[i] > MAXIMUM)
    {
      MAXIMUM = ARRAY[i];
      LOCATION = i;
    }
  }

  return LOCATION;
}

//Huvudfunktion där roboten undviker hinder

void Main() {

  int measuredDistance = MeasureDistance();
  mySerial.println(measuredDistance);
  Serial.println(measuredDistance);

  if (measuredDistance >= 100) {
    turbo();
  }
  else if (measuredDistance > mindistance) {
    DriveForward();
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
    switch (Position)
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

  if (Serial.available() > 0)
  {
    int value = Serial.parseInt();
    if (value != NULL)
    {
      mode = value;
    }
  }

  if (mySerial.available() > 0)
  {
    int value = mySerial.parseInt();
    if (value != NULL)
    {
      mode = value;
    }
  }

  if (mode != lastmode)
  {
    switch (mode) {
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
        Serial.println("mode = " + mode);
        mySerial.println("Need mode...");
        mySerial.println("mode = " + mode);
        break;
    }
  }

  switch (mode) {
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

/*Funktion som gör att roboten svänger vänster en
viss tid för att sedan stanna.*/
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

/*Funktion som gör att roboten svänger höger en
viss tid för att sedan stanna.*/
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

/*Sväng vänster i 150ms och öka sedan tiden med 10ms tills
210ms uppnåtts. Stanna också i 20 sekunder mellan varje
svängningstillfälle.*/
void TurnTest1() {
  delay(10000);
  for (int i = 15 ; i < 21 ; i++) {
    TurnLeftms(10 * i);
    delay(20000);
  }

}

/*Vänta 5 sekunder och sväng sedan en lång tid för att
kunna approximera den maximala vinkelhastigheten hos roboten*/
void TurnTest2() {
  delay(5000);
  TurnLeft(50000);

}

/*Test av svängfunktion för att se hur väl den stämmer
överens med verkligheten.*/
void TurnTest3() {
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

//Testa svängfunktionen vid större vinklar
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

//Stå still och mät endast avståndet till ett objekt
void SensorTest() {
  //Test av avståndssensorn
  long distance = MeasureDistance();
  Serial.print("Distance = ");
  Serial.println(distance);
  mySerial.print("Distance = ");
  mySerial.println(distance);
  delay(100);
}

//Kör framåt
void driveTest()
{
  DriveForward();
}
