#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <Servo.h>

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX

int IN1 = 6;
int IN2 = 5;
int ENA = 7;
int kiirus = 130;

int vasak = 65;
int otse = 90;
int parem = 115;

Servo rool;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);

  rool.attach(3); // servo pin
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

}
void edasi(int kiirus)
{
  analogWrite(ENA, kiirus);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
}
void stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(4000);
}
void loop() {
  // put your main code here, to run repeatedly:void loop() {
  if (!huskylens.request()) {
    edasi(kiirus);
    //Serial.println("ei saanud huskylens request");
    return;
  }

  if (!huskylens.available()) {
    edasi(kiirus);
    Serial.println("otse");
    return;
  }

  HUSKYLENSResult result = huskylens.read();

  if (result.ID == 1) {
    rool.write(vasak);
    Serial.println("vasak");
  } 
  else if (result.ID == 2) {
    rool.write(parem);
    Serial.println("parem");
  } 
  else if (result.ID == 3){
    Serial.println("stop");
    //stop();
  }
  else {
    edasi(kiirus);
    Serial.println("teine otse");
  }

  delay(100);
}

