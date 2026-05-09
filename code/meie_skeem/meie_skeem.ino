#include <Servo.h>
#include <SoftwareSerial.h>
#include "HUSKYLENS.h"

SoftwareSerial mySerial(13, 10);
HUSKYLENS huskylens;
Servo rool;

// ---- Pins -----------------------------------------------
const int L_TRIG    = 9,  L_ECHO = 12;
const int R_TRIG    = 8,  R_ECHO = 4;
const int SERVO_PIN = 11;
const int MOTOR_IN3 = 7;
const int MOTOR_IN4 = 6;
const int MOTOR_ENB = 5;

// ---- HuskyLens colour IDs -------------------------------
const int ID_RED    = 1;
const int ID_GREEN  = 2;
const int ID_ORANGE = 3;
const int ID_BLUE   = 4;

const int vasak = 130;
const int parem = 50;
const int otse = 90;

const int wall_too_close = 70;

const float ALPHA = 0.25;

float emaL = -1, emaR = -1;

int eelmine_id = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(9600);

  pinMode(L_TRIG, OUTPUT); pinMode(L_ECHO, INPUT);
  pinMode(R_TRIG, OUTPUT); pinMode(R_ECHO, INPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);

  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENB, 0);

  rool.attach(SERVO_PIN);
  rool.write(otse);

  if (huskylens.begin(mySerial)) {
    huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
    Serial.println("HuskyLens OK");
  } else {
    Serial.println("HuskyLens not found");
  }

  delay(1000);
  driveForward(160);

}

float readCm(int trig, int echo) {
  digitalWrite(trig, LOW);  delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long d = pulseIn(echo, HIGH, 25000UL);
  return d ? d * 0.0343 / 2.0 : -1.0;
}

float updateEMA(float prev, float raw) {
  if (raw < 2.0 || raw > 400.0) return prev;
  return (prev < 0) ? raw : ALPHA * raw + (1.0 - ALPHA) * prev;
}

void loop() {
  delay(200);
  emaL = readCm(L_TRIG, L_ECHO);
  delay(5);  // prevents cross-talk between sensors
  emaR = readCm(R_TRIG, R_ECHO);
  /*
  Serial.print("parem pool : ");
  Serial.println(emaR);
  Serial.print("vasak pool : ");
  Serial.println(emaL);
  */

  // put your main code here, to run repeatedly:
  //kontrollib ühendust kaameraga
  if (!huskylens.request()) {
    Serial.println("ei saanud huskylens request");
    return;
  }
  HUSKYLENSResult result = huskylens.read(); 
  //kui näeb
  if (!huskylens.available()) {
    // Jah
    if (result.ID == ID_GREEN || result.ID == ID_BLUE){ //roheline aka vasakule
      rool.write(vasak); 
    }
    if (result.ID == ID_RED || result.ID == ID_ORANGE){ //punane aka paremale
      rool.write(parem);
    }
    //ei näe
    if(emaL <= wall_too_close){
      rool.write(parem);
      Serial.println("vasak");
    }
    else if(emaR <= wall_too_close){
      rool.write(vasak);
      Serial.println("parem");
    }
    //vaja muidu servo teeb naljakat asja
    else {
      if (rool.read() != otse){
        rool.write(otse);
        eelmine_id = 0;
        Serial.println("otse");
      }
      else{
        delay(20);
      }
    }
    
    
  }
  eelmine_id = result.ID;
}
void driveForward(int spd) {
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);
  analogWrite(MOTOR_ENB, spd);
}

