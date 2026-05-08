#include "HUSKYLENS.h"
#include "Servo.h"
#include "Wire.h"

// ========== PIN DEFINITIONS ==========
#define SERVO_PIN     3
#define MOTOR_ENA     5
#define MOTOR_IN1     7
#define MOTOR_IN2     6

// ========== TUNING CONSTANTS ==========
#define FRAME_CENTER    160    // HuskyLens frame is 320px wide
#define BASE_SPEED      150    // 0-255, start low and tune up
#define SLOW_SPEED      100
#define SERVO_CENTER    90     // Adjust until wheels are straight
#define SERVO_LEFT_MAX  60     // Max left steering angle
#define SERVO_RIGHT_MAX 120    // Max right steering angle
#define KP              0.08   // Proportional gain, tune this!

// ========== OBJECTS ==========
HUSKYLENS huskylens;
Servo steeringServo;

// ========== STATE MACHINE ==========
enum State {
  FOLLOW_LANE,
  AVOID_RED,    // Red pillar - pass on RIGHT (steer left)
  AVOID_GREEN,  // Green pillar - pass on LEFT (steer right)
  FINISH
};

State currentState = FOLLOW_LANE;

// ========== LAP COUNTING ==========
int sectionsCrossed = 0;
int lapCount = 0;
bool orangeVisible = false;

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(SERVO_CENTER);
  
  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  
  // Wait for HuskyLens
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens not found, check wiring...");
    delay(500);
  }
  Serial.println("HuskyLens connected!");
  
  // Set mode - COLOR RECOGNITION for obstacle challenge
  // Change to ALGORITHM_LINE_TRACKING for open challenge
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
  
  delay(2000); // Wait before start
}

// ========== MOTOR FUNCTIONS ==========
void driveForward(int speed) {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, speed);
}

void driveStop() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, 0);
}

// ========== STEERING FUNCTION ==========
void setSteering(int angle) {
  angle = constrain(angle, SERVO_LEFT_MAX, SERVO_RIGHT_MAX);
  steeringServo.write(angle);
}

// ========== PROPORTIONAL STEERING ==========
// Pass in where the target IS on screen (x: 0-320)
// Returns servo angle
int calcSteering(int targetX) {
  int error = targetX - FRAME_CENTER;
  int steerAngle = SERVO_CENTER + (int)(error * KP);
  return constrain(steerAngle, SERVO_LEFT_MAX, SERVO_RIGHT_MAX);
}

// ========== MAIN LOOP ==========
void loop() {
  
  // Stop after 3 laps
  if (lapCount >= 3) {
    driveStop();
    setSteering(SERVO_CENTER);
    Serial.println("3 laps complete!");
    while(true); // Halt
  }

  if (!huskylens.request()) {
    // Can't reach HuskyLens - keep last action briefly
    Serial.println("HuskyLens request failed");
    return;
  }

  // ===== OBSTACLE CHALLENGE LOGIC =====
  obstacleChallenge();
}

void obstacleChallenge() {
  
  HUSKYLENSResult redPillar;
  HUSKYLENSResult greenPillar;
  bool redSeen = false;
  bool greenSeen = false;

  // Read all detected objects
  // ID 1 = RED pillar (you train this first)
  // ID 2 = GREEN pillar (you train this second)
  while (huskylens.available()) {
    HUSKYLENSResult result = huskylens.read();
    
    if (result.ID == 1) {  // RED
      redPillar = result;
      redSeen = true;
    }
    if (result.ID == 2) {  // GREEN
      greenPillar = result;
      greenSeen = true;
    }
  }

  // ===== STATE MACHINE =====
  switch (currentState) {

    case FOLLOW_LANE:
      driveForward(BASE_SPEED);
      setSteering(SERVO_CENTER); // Drive straight, tune with wall sensors ideally
      
      // Transition: pillar spotted ahead
      if (redSeen && redPillar.yCenter > 100) {   // Close enough
        currentState = AVOID_RED;
        Serial.println("-> AVOID RED");
      }
      if (greenSeen && greenPillar.yCenter > 100) {
        currentState = AVOID_GREEN;
        Serial.println("-> AVOID GREEN");
      }
      break;

    case AVOID_RED:
      // Red pillar must be on our RIGHT
      // So steer LEFT to keep it on our right side
      driveForward(SLOW_SPEED);
      {
        if (redSeen) {
          // Steer so pillar goes to right side of frame
          // Target: pillar x should be at ~240 (right third of screen)
          int targetPillarX = 240;
          int error = redPillar.xCenter - targetPillarX;
          int steer = SERVO_CENTER - (int)(error * KP * 1.5);
          setSteering(steer);
          
          // Pillar passed when it disappears or goes behind us (yCenter < 50)
          if (redPillar.yCenter < 50) {
            currentState = FOLLOW_LANE;
            Serial.println("-> Red cleared, FOLLOW_LANE");
          }
        } else {
          // Lost sight of pillar - it's behind us
          currentState = FOLLOW_LANE;
        }
      }
      break;

    case AVOID_GREEN:
      // Green pillar must be on our LEFT
      // So steer RIGHT to keep it on our left side
      driveForward(SLOW_SPEED);
      {
        if (greenSeen) {
          // Target: pillar x should be at ~80 (left third of screen)
          int targetPillarX = 80;
          int error = greenPillar.xCenter - targetPillarX;
          int steer = SERVO_CENTER - (int)(error * KP * 1.5);
          setSteering(steer);
          
          if (greenPillar.yCenter < 50) {
            currentState = FOLLOW_LANE;
            Serial.println("-> Green cleared, FOLLOW_LANE");
          }
        } else {
          currentState = FOLLOW_LANE;
        }
      }
      break;

    case FINISH:
      driveStop();
      break;
  }

  // ===== LAP COUNTING =====
  // Basic: count time-based laps as fallback
  // Better: train HuskyLens on orange line as ID 3
  countLaps();
  
  delay(20); // ~50Hz loop
}

// ========== LAP COUNTING ==========
// Option A: Time based (rough but works as start)
unsigned long lapStartTime = 0;
unsigned long LAP_TIME_MS = 8000; // Tune to your robot's actual lap time

void countLaps() {
  if (lapStartTime == 0) lapStartTime = millis();
  
  if (millis() - lapStartTime > LAP_TIME_MS) {
    lapCount++;
    lapStartTime = millis();
    Serial.print("Lap completed! Total laps: ");
    Serial.println(lapCount);
  }
}