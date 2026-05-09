#include <Servo.h>
#include <SoftwareSerial.h>
#include "HUSKYLENS.h"

SoftwareSerial mySerial(13, 10);
HUSKYLENS huskylens;
Servo steeringServo;

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

// ---- Tuning ---------------------------------------------
const int   CENTER      = 90;
const int   DODGE_ANGLE = 30;
const int   WALL_ANGLE  = 12;
const float WALL_CLOSE  = 15.0;
const float WALL_GONE   = 80.0;
const float WALL_TURN   = 12.0;
const float ALPHA       = 0.25;

// ---- EMA state ------------------------------------------
float emaL = -1, emaR = -1;

// ---- Timers and flags -----------------------------------
bool dodging       = false;
bool returning     = false;
bool cornering     = false;
bool wallTurning   = false;
bool wallReturning = false;

int dodgeDir    = 0;
int wallTurnDir = 0;

unsigned long dodgeStart      = 0;
unsigned long cornerStart     = 0;
unsigned long wallTurnStart   = 0;
unsigned long wallReturnStart = 0;

const unsigned long DODGE_MS     = 400;
const unsigned long CORNER_MS    = 700;
const unsigned long WALL_TURN_MS = 350;

// ---- Track direction ------------------------------------
int trackDir = 0;

// =========================================================
void setup() {
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

  steeringServo.attach(SERVO_PIN);
  steeringServo.write(CENTER);

  if (huskylens.begin(mySerial)) {
    huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
    Serial.println("HuskyLens OK");
  } else {
    Serial.println("HuskyLens not found");
  }

  delay(1000);
  driveForward(160);
}

// =========================================================
void loop() {
  emaL = updateEMA(emaL, readCm(L_TRIG, L_ECHO));
  delay(5);
  emaR = updateEMA(emaR, readCm(R_TRIG, R_ECHO));

  bool seenRed    = false;
  bool seenGreen  = false;
  bool pillarGone = true;

  if (huskylens.request() && huskylens.available()) { //kui huskylens on olemas ja näeb midagi
    while (huskylens.available()) { //vaatab läbi kõik objektid mida kaamera näeb
      HUSKYLENSResult r = huskylens.read(); 
      //Reads one detected object. Skips it if it isn't a colour block — just a safety check against garbage data.
      if (r.command != COMMAND_RETURN_BLOCK) continue;

      //Kui pole veel teada mis pidi sõidame siis esimene joon määrab suuna
      if (r.ID == ID_ORANGE && trackDir == 0) trackDir = +1; 
      if (r.ID == ID_BLUE   && trackDir == 0) trackDir = -1;

      //Checks if a red or green pillar is in the middle 60% of the camera frame. 
      //Pillars at the very edges are ignored because they're either already passed or off to the side. 
      //Sets a simple true/false flag that the dodge logic reads later.
      bool ahead = (r.xCenter > 60 && r.xCenter < 260);
      if (r.ID == ID_RED   && ahead) seenRed   = true;
      if (r.ID == ID_GREEN && ahead) seenGreen = true;

      //kontrollib möödudes kas me oleme juba möödunud meie postist v ei
      int targetID = (dodgeDir == +1) ? ID_RED : ID_GREEN;
      if (dodging && r.ID == targetID && r.xCenter > 30 && r.xCenter < 290)
        pillarGone = false;
    }
  }

  // --- Dodge timer -----------------------------------------
  //mõõdab ära ümbersõidu aja
  if (dodging) {
    if (pillarGone || millis() - dodgeStart >= DODGE_MS) {
      dodging    = false;
      returning  = true;
      dodgeStart = millis();
      steeringServo.write(constrain(CENTER - dodgeDir * DODGE_ANGLE, 60, 120));
    }
  }
  // --- Dodge return timer ----------------------------------
  else if (returning) {
    bool centred = (emaL > 0 && emaR > 0)
                   ? (abs(emaR - emaL) < 5.0)
                   : (millis() - dodgeStart >= DODGE_MS);
    if (centred) {
      returning = false;
      steeringServo.write(CENTER);
    }
  }
  // --- Corner timer ----------------------------------------
  else if (cornering) {
    if (millis() - cornerStart >= CORNER_MS) {
      cornering = false;
      steeringServo.write(CENTER);
    }
  }
  // --- Wall turn timer -------------------------------------
  else if (wallTurning) {
    if (millis() - wallTurnStart >= WALL_TURN_MS) {
      wallTurning     = false;
      wallReturning   = true;
      wallReturnStart = millis();
      steeringServo.write(constrain(CENTER - wallTurnDir * 40, 60, 120));
    }
  }
  // --- Wall return timer -----------------------------------
  else if (wallReturning) {
    bool centred = (emaL > 0 && emaR > 0)
                   ? (abs(emaR - emaL) < 5.0)
                   : (millis() - wallReturnStart >= WALL_TURN_MS);
    if (centred) {
      wallReturning = false;
      steeringServo.write(CENTER);
    }
  }
  // --- Default: drive forward ------------------------------
  else {
    if (seenRed) {
      startDodge(+1);
    } else if (seenGreen) {
      startDodge(-1);
    } else if (isCorner()) {
      startCornerTurn();
    } else if (emaR > 0 && emaR < WALL_TURN) {
      startWallTurn(-1);           // right sensor too close → turn left
    } else if (emaL > 0 && emaL < WALL_TURN) {
      startWallTurn(+1);           // left sensor too close → turn right
    } else {
      wallCorrect();
    }
    driveForward(160);
  }

  delay(25);
}

// =========================================================
void startDodge(int dir) {
  dodgeDir   = dir;
  dodgeStart = millis();
  dodging    = true;
  steeringServo.write(constrain(CENTER + dir * DODGE_ANGLE, 60, 120));
}

void startCornerTurn() {
  cornering   = true;
  cornerStart = millis();
  int dir     = (trackDir != 0) ? trackDir : +1;
  steeringServo.write(constrain(CENTER + dir * 40, 60, 120));
}

void startWallTurn(int dir) {
  wallTurning   = true;
  wallTurnStart = millis();
  wallTurnDir   = dir;
  steeringServo.write(constrain(CENTER + dir * 40, 60, 120));
}

bool isCorner() {
  bool leftGone  = (emaL < 0 || emaL > WALL_GONE);
  bool rightGone = (emaR < 0 || emaR > WALL_GONE);
  return (leftGone && rightGone);
}

void wallCorrect() {
  int steer = CENTER;
  if (emaL > 0 && emaR > 0) {
    float diff = emaR - emaL;
    steer = CENTER + (int)(diff * 0.5);
  } else if (emaL > 0 && emaL < WALL_CLOSE) {
    steer = CENTER + WALL_ANGLE;
  } else if (emaR > 0 && emaR < WALL_CLOSE) {
    steer = CENTER - WALL_ANGLE;
  }
  steeringServo.write(constrain(steer, 60, 120));
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

void driveForward(int spd) {
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);
  analogWrite(MOTOR_ENB, spd);
}