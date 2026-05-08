#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <Servo.h>

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX
//pins
int IN1 = 7;
int IN2 = 6;
int ENA = 5;
int kiirus = 200;

//rooli suund
int parem = 50;
int otse = 90;
int vasak = 200;

//värvide id
int roheline_id = 1;
int punane_id = 2;
int roosa_id = 3;

int eelmine_id = 0;
bool soida = true;

int start = 0;


Servo rool;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);

  rool.attach(3); // servo pin
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  Serial.println("algus");

  while (!huskylens.begin(mySerial)) {
    Serial.println("HuskyLens ühendus ebaõnnestus!");
    delay(1000);
  }

  Serial.println("HuskyLens OK");

}
void edasi(int kiirus)
{
  analogWrite(ENA, kiirus);
  
   if(soida){
      digitalWrite(IN2, HIGH);
      digitalWrite(IN1, LOW);
      soida = false;
      //Serial.println("sõida");
    }
    else{
      digitalWrite(IN2, LOW);
      digitalWrite(IN1, LOW);
      soida = true;
      //Serial.println("stop");
    }

}
void stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(100);
}
void loop() {
  //kontrollib ühendust kaameraga
  if (!huskylens.request()) {
    stop();
    rool.write(otse);

    Serial.println("ei saanud huskylens request");
    return;
  }
  //kui kaamera ei näe ühtegi värvi siis sõida otse
  if (!huskylens.available()) {
    if (eelmine_id == 1){ //roheline aka vasakule
      rool.write(parem);
      delay(millis()-start);
    }
    else if (eelmine_id == 2){ //punane aka paremale
      rool.write(vasak);
      delay(millis()-start);
    }
    if (rool.read() != otse){
      rool.write(otse);
    }
    else{
      //delay(100);
    }
    edasi(kiirus);
    eelmine_id = 0;
    return;
  }
  //anname klassile isendi
  HUSKYLENSResult result = huskylens.read(); 

  //mida ta teeb iga värvi korral
  if (result.ID == roheline_id) { 
    rool.write(vasak);
    edasi(kiirus);
    if (eelmine_id != result.ID){
      start = millis();
    }
    //Serial.println("vasak");
   
  } 
  else if (result.ID == punane_id) {
    rool.write(parem);
    edasi(kiirus);
    if (eelmine_id != result.ID){
      start = millis();
    }
    //Serial.println("parem");
  } 
  
  Serial.println(result.ID);
  eelmine_id = result.ID;

  
}

