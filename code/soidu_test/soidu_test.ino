int IN1 = 7;
int IN2 = 6;
int ENA = 5;
#include <Servo.h>

Servo rool;
void setup() {
  // put your setup code here, to run once:
  rool.attach(3); // servo pin
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

}
void tagasi(int ms, int kiirus)
{
  analogWrite(ENA, kiirus);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(ms);
  digitalWrite(IN1, LOW);
}

void edasi(int ms, int kiirus)
{
  analogWrite(ENA, kiirus);

  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  delay(ms);
  digitalWrite(IN2, LOW);
}
/*
rooli 180 kraadi = otse
rooli 90 kraadi = vasakule
rooli 2700 kraadi = paremale
*/
void loop() {
  // put your main code here, to run repeatedly:
  //edasi(500, 130);

  edasi(2000, 60);
}
