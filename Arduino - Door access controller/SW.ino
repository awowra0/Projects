#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <RTClib.h>

Servo servo1;
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
RTC_DS1307 rtc;

const int pMotion = 2;
const int pDoor = 7;
const int pLEDr = 5;
const int pLEDg = 4;
const int pLEDy = 6;
const int pServo = 8;
const int pBuzzer = 15;
const int pButton = 14;

int CurrentDoorState; // current state of door sensor
int LastDoorState;    // previous state of door sensor
unsigned long TheTime; //loop
unsigned long Timer1; //door opened
unsigned long Timer2; //door unlocked
unsigned long Timer3; //extended opened
unsigned long Timer4; //motion
unsigned long Timer5;
int Timers [6] = {0,0,0,0,0,0};
int Counter = 0;
int Motion;
int Access = 0; //0 - locked, 1 - unlocked, 3 - pre alarm, 4 - alarm
int bButton;
//String ids[10] = ("EA CC 73 2E", "EA CC 73 2D", "EA CC 73 2C", "EA CC 73 2F");
int Time1 = 5000;
int Time2 = 3000;
int Time3 = 10000;
int Time4 = 5000;
int Time5 = 30000;
int CounterMAX = 2;
String sdata;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(pDoor, INPUT_PULLUP);
  pinMode(pLEDr, OUTPUT);
  pinMode(pLEDg, OUTPUT);
  pinMode(pLEDy, OUTPUT);
  pinMode(pMotion, INPUT_PULLUP);
  pinMode(pButton, INPUT_PULLUP);
  digitalWrite(pLEDg, 0);
  digitalWrite(pLEDr, 0);
  digitalWrite(pLEDy, 0);
  noTone(pBuzzer);
  servo1.attach(pServo);
  servo1.write(5);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("System initialized.");
  dateprint();

  CurrentDoorState = digitalRead(pDoor); // read state
  if (CurrentDoorState == 1) {
    digitalWrite(pLEDg, 1);
    Access = 1;
    Timers[1] = 1;
    Timer1 = millis();
    Serial.println("door initially opened.");
  }
  else {
    digitalWrite(pLEDr, 1);
    Serial.println("door initially closed.");
  }
}

void loop() {
  LastDoorState = CurrentDoorState;
  CurrentDoorState  = digitalRead(pDoor);
  Motion = digitalRead(pMotion);
  TheTime = millis();
  bButton = digitalRead(pButton);

  if (Serial.available() > 0) { // wprowadzono napis
    sdata = Serial.readStringUntil('#');
    if (sdata=="DATA"){
      Serial.println("Current date: ");
      dateprint();
      Serial.println("done.");
    }
    else if (sdata=="ALARM_OFF" and Access == 4){
          Serial.println("Alarm turned off by command.");
          Timers[1] = 0;
          Timers[2] = 0;
          Timers[3] = 0;
          noTone(pBuzzer);
          dateprint();
          if (CurrentDoorState == 1){
            Access = 1;
            Timers[3] = 1;
            Timer3 = millis();
            digitalWrite(pLEDr, 0);
            digitalWrite(pLEDg, 1);
            Serial.println("alarm turned off, door opened.");
          }
          else {
            Access = 0;
            servo1.write(5);
            digitalWrite(pLEDg, 0);
            digitalWrite(pLEDr, 1);
            Serial.println("alarm turned off, door closed.");
          }
    }
    else if (sdata=="UNLOCK" and Access == 0){
          Serial.println("Door unlocked by command.");
          Access = 1;
          servo1.write(90);
          digitalWrite(pLEDg, 1);
          digitalWrite(pLEDr, 0);
          Timers[2] = 1;
          Timer2 = millis();
          dateprint();
          Serial.println("access granted.");
    }
    else if (sdata=="TIMER" and Access == 1 and Timers[2] == 0){
      Serial.println("Time extended by command.");
      if (Timers[1] == 1){
        Timers[1] = 0;
        Timers[3] = 1;
      }
      Timer3 = millis();
      digitalWrite(pLEDr, 0);
      dateprint();
      Serial.println("time extended.");
    }
  }
  
  if (Access == 3){ //alarm setup
    Timers[5] = 0;
    tone(pBuzzer,1500);
    digitalWrite(pLEDg, 0);
    digitalWrite(pLEDr, 1);
    Access = 4;
    delay(100);
  }

  else if ( mfrc522.PICC_IsNewCardPresent()){
    if ( mfrc522.PICC_ReadCardSerial()) {
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      if (content.substring(1) == "EA CC 73 2E"){ //correct card
        dateprint();
        Serial.print(content.substring(1));
        if(Access == 0){
          Access = 1;
          servo1.write(90);
          digitalWrite(pLEDg, 1);
          digitalWrite(pLEDr, 0);
          Serial.println(", access granted.");
        }
        if (Access == 4){
          Timers[1] = 0;
          Timers[2] = 0;
          Timers[3] = 0;
          noTone(pBuzzer);
          if (CurrentDoorState == 1){
            Access = 1;
            Timers[3] = 1;
            Timer3 = millis();
            digitalWrite(pLEDr, 0);
            digitalWrite(pLEDg, 1);
            Serial.println(", alarm turned off, door opened.");
          }
          else {
            Access = 0;
            servo1.write(5);
            digitalWrite(pLEDg, 0);
            digitalWrite(pLEDr, 1);
            Serial.println(", alarm turned off, door closed.");
          }
        }
        if (Timers[1] == 0 and Timers[2] == 0 and Timers[3] == 0 and Access == 1){
          Timers[2] = 1;
          Timer2 = millis();
        }
        else if (Timers[2] == 1){
          digitalWrite(pLEDr, 0);
          Timer2 = millis();
          Serial.println(", door lock reset.");
        }
        else if (Timers[1] == 1 and Timers[3] == 0){
          Timers[1] = 0;
          Timers[3] = 1;
          digitalWrite(pLEDr, 0);
          Timer3 = millis();
          Serial.println(", extended time request.");
        }
        else if (Timers[1] == 0 and Timers[3] == 1){
          digitalWrite(pLEDr, 0);
          Timer3 = millis();
          Serial.println(", extended time reset.");
        }
        Timers[5] = 0;
        Counter = 0;
        tone(pBuzzer, 2000, 125);
        delay(125);
        noTone(pBuzzer);
        delay(875);
      }
      else {
        dateprint();
        Serial.print(content.substring(1));
        Counter++;
        if (Counter > CounterMAX){ //alarm
          if (Access == 4){
            Serial.println(", unautorised access during alarm.");
          }
          else {
            Access = 3;
            Serial.println(", unautorised access, alarm turned on.");
          }
        }
        else {
          Serial.println(", access denied.");
          if (Timers[5] == 0){
            Timers[5] = 1;
          }
          Timer5 = millis();
        }
        if (Access == 4){
          tone(pBuzzer, 1700,333);
          delay(333);
          noTone(pBuzzer);
          delay(167);
          tone(pBuzzer, 1500);
          delay(400);
        }
        else {
        tone(pBuzzer, 1700, 400);
        delay(400);
        noTone(pBuzzer);
        delay(600);
        }
      }
    }
  }

  else if (bButton == 0){ //button pushed
    dateprint();
    if (Access == 0){ //somebody inside tries to leave
      Access = 1;
      servo1.write(90);
      Timers[2] = 1;
      Timer2 = millis();
      digitalWrite(pLEDg, 1);
      digitalWrite(pLEDr, 0);
      tone(pBuzzer, 2000, 125);
      delay(125);
      noTone(pBuzzer);
      delay(875);
      Serial.println("button pushed, door unlocked.");
    }
    else if (Access == 1){ //door 
      if (Timers[2] == 1){
        digitalWrite(pLEDr, 0);
        Timer2 = millis();
        Serial.println("button pushed, lock reset.");
      }
      else if (Timers[1] == 1){ // somebody needs more time
        digitalWrite(pLEDr, 0);
        Timers[1] = 0;
        Timers[3] = 1;
        Timer3 = millis();
        Serial.println("button pushed, extended time request.");
      }
      else if (Timers[3] == 1){ 
        Timer3 = millis(); //reset time
        digitalWrite(pLEDr, 0);
        Serial.println("button pushed, extended time reset.");
      }
      delay(400);
    }
    else if (Access == 4){
      servo1.write(90);
      Serial.println("button pushed during alarm, emergency exit.");
    }
    delay(200);
  }
  
  else if (LastDoorState == 0 and CurrentDoorState == 1) { // door opened
    if (Access == 0){ //opened by force - alarm
      Access = 3;
      dateprint();
      Serial.println("unautorised access, alarm turned on.");
    }
    else if (Access == 1){ //unlocked by card and opened
      digitalWrite(pLEDr, 0);
      Timer1 = millis();
      Timers[1] = 1;
      Timers[2] = 0;
      dateprint();
      Serial.println("door opened.");
    }
  }
  else if (LastDoorState == 1 and CurrentDoorState == 0) { // door closed
    Timers[1] = 0;
    Timers[3] = 0;
    if (Access == 1) Access = 0;
    servo1.write(5);
    digitalWrite(pLEDg, 0);
    digitalWrite(pLEDr, 1);
    dateprint();
    Serial.println("door closed.");
  }

  else if (Timers[2] == 1 and Timer2 + Time2 - 1000 < TheTime){
    digitalWrite(pLEDr, 1);
    if (Timers[2] == 1 and Timer2 + Time2 < TheTime){
      Timers[2] = 0;
      Access = 0;
      servo1.write(5);
      digitalWrite(pLEDg, 0);
      digitalWrite(pLEDr, 1);
      dateprint();
      Serial.println("door locked.");
    }
  }
  
  else if (Timers[1] == 1 and Timer1 + Time1 - 3000 < TheTime){ //5000-3000
    digitalWrite(pLEDr, 1); // 3 seconds left
    if (Timers[1] == 1 and Timer1 + Time1 < TheTime){ // 5 seconds alarm
      Timers[1] = 0;
      Access = 3;
      dateprint();
      Serial.println("time's up, alarm turned on.");
    } 
  }
  else if (Timers[3] == 1 and Timer3 + Time3 - 3000 < TheTime){
    digitalWrite(pLEDr, 1); // 3 seconds left
    if (Timers[3] == 1 and Timer3 + Time3 < TheTime){ // extended time alarm
    Timers[3] = 0;
    Access = 3;
    dateprint();
    Serial.println("extended time's up, alarm turned on.");
    }
  }
  if (Timers[5] == 1 and Timer5 + Time5 < TheTime){
    Timers[5] = 0;
    Counter = 0;
    dateprint();
    Serial.println("counter reset.");
  }
  
  
  if (Timers[4] == 1 and Timer4 + Time4 < TheTime){ // no motion
    Timers[4] = 0;
    digitalWrite(pLEDy, 0);
    dateprint();
    Serial.println("motion lost");
  }

  if (Motion == 1) {
    if (Timers[4] == 0){
      Timers[4] = 1;
      digitalWrite(pLEDy, 1);
      Timer4 = millis();
      dateprint();
      Serial.println("motion detected.");
    }
    else if (Timer4 + 1000 < TheTime) Timer4 = millis();
  }
}

void dateprint(){
  DateTime now = rtc.now();
  Serial.print(now.day(), DEC);
  Serial.print(".");
  Serial.print(now.month(), DEC);
  Serial.print(".");
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(", ");
  return;
}
