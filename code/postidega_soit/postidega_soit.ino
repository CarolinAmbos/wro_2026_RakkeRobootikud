#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <Servo.h>

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX
//pins
const int ENB = 5;
const int IN4 = 7;
const int IN3 = 6;
int kiirus = 130;

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
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.println("algus");

  while (!huskylens.begin(mySerial)) {
    Serial.println("HuskyLens ühendus ebaõnnestus!");
    delay(1000);
  }

  Serial.println("HuskyLens OK");

}
void edasi(int kiirus, bool forward)
{
  analogWrite(ENB, kiirus);
  digitalWrite(IN3, forward);
  digitalWrite(IN4, !forward);

}
void stop(){
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
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
    if (eelmine_id == roheline_id){ //roheline aka vasakule
      rool.write(parem);
      delay(millis()-start);
    }
    else if (eelmine_id == punane_id){ //punane aka paremale
      rool.write(vasak);
      delay(millis()-start);
    }

    //vaja muidu servo teeb naljakat asja
    if (rool.read() != otse){
      rool.write(otse);
    }
    else{
      delay(20);
    }

    edasi(kiirus, true);
    eelmine_id = 0;
    //Serial.println(rool.read());
    //Serial.println("otse");
    return;
  }
  //anname klassile isendi
  HUSKYLENSResult result = huskylens.read(); 

  //mida ta teeb iga värvi korral
  if (result.ID == roheline_id) { 
    rool.write(vasak);
    edasi(kiirus, true);
    //Serial.println("vasak");
   
  } 
  else if (result.ID == punane_id) {
    rool.write(parem);
    edasi(kiirus, true);
    //Serial.println("parem");
  } 
  //seda vist väga vaja ei ole aga eks vaatab seda nalja
  else if (result.ID == roosa_id){
    //Serial.println("stop");
    //stop();
  }
  Serial.println(result.ID);
  eelmine_id = result.ID;
  
}

