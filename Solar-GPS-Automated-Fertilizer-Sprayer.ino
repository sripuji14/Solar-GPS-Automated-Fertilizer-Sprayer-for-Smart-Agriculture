/* ==========================================================
   AUTOMATED SOLAR FERTILIZER SPRAYING SYSTEM
   Components:
   - Arduino UNO
   - 2x TB6612FNG motor drivers (4 DC motors)
   - Ultrasonic Sensor HC-SR04
   - 12V diaphragm pump controlled by IRLZ44N MOSFET
   - 12V Battery with buck converter (5V to Arduino)
   ========================================================== */

// ---------- TB6612FNG Motor Driver Pin Configuration ----------
#define STBY 4   // Standby pin (common to both motor drivers)

// Driver 1 (Left motors)
#define PWMA 5
#define AIN1 6
#define AIN2 7
#define PWMB 9
#define BIN1 10
#define BIN2 11

// Driver 2 (Right motors)
#define PWMA2 3
#define AIN1_2 12
#define AIN2_2 13
#define PWMB2 2
#define BIN1_2 8
#define BIN2_2 A0

// ---------- Ultrasonic Sensor ----------
#define TRIG A1
#define ECHO A2

// ---------- Pump (MOSFET control) ----------
#define PUMP A3  // Gate connected through 220Ω resistor

// ---------- Variables ----------
long duration;
int distance;
int motorSpeed = 180;  // Motor speed (0–255)
int pumpPWM = 200;     // Pump speed (PWM duty)
int obstacleLimit = 25; // Distance in cm to detect obstacle

// ==========================================================
// --------------------- SETUP ------------------------------
// ==========================================================
void setup() {
  Serial.begin(9600);

  // Motor driver pins
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(PWMA2, OUTPUT); pinMode(AIN1_2, OUTPUT); pinMode(AIN2_2, OUTPUT);
  pinMode(PWMB2, OUTPUT); pinMode(BIN1_2, OUTPUT); pinMode(BIN2_2, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Pump pin
  pinMode(PUMP, OUTPUT);

  // Enable both motor drivers
  digitalWrite(STBY, HIGH);

  Serial.println("System Initialized");
  delay(1000);
}

// ==========================================================
// ---------------------- MAIN LOOP --------------------------
// ==========================================================
void loop() {
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < obstacleLimit) {
    // Obstacle detected
    stopMotors();
    stopPump();
    Serial.println("Obstacle detected! Reversing...");
    delay(500);

    backward();
    delay(1000);

    turnRight();
    delay(800);
  } 
  else {
    // Clear path
    forward();
    analogWrite(PUMP, pumpPWM);  // Start spraying while moving
  }

  delay(100);
}

// ==========================================================
// ----------------- MOTOR CONTROL FUNCTIONS ----------------
// ==========================================================

void forward() {
  moveMotor(AIN1, AIN2, PWMA, true);
  moveMotor(BIN1, BIN2, PWMB, true);
  moveMotor(AIN1_2, AIN2_2, PWMA2, true);
  moveMotor(BIN1_2, BIN2_2, PWMB2, true);
  Serial.println("Moving Forward");
}

void backward() {
  moveMotor(AIN1, AIN2, PWMA, false);
  moveMotor(BIN1, BIN2, PWMB, false);
  moveMotor(AIN1_2, AIN2_2, PWMA2, false);
  moveMotor(BIN1_2, BIN2_2, PWMB2, false);
  Serial.println("Moving Backward");
}

void stopMotors() {
  analogWrite(PWMA, 0); analogWrite(PWMB, 0);
  analogWrite(PWMA2, 0); analogWrite(PWMB2, 0);
  Serial.println("Motors Stopped");
}

void turnRight() {
  // Left wheels forward, right wheels backward
  moveMotor(AIN1, AIN2, PWMA, true);
  moveMotor(BIN1, BIN2, PWMB, true);
  moveMotor(AIN1_2, AIN2_2, PWMA2, false);
  moveMotor(BIN1_2, BIN2_2, PWMB2, false);
  Serial.println("Turning Right");
}

void moveMotor(int in1, int in2, int pwm, bool forwardDir) {
  if (forwardDir) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  analogWrite(pwm, motorSpeed);
}

// ==========================================================
// ----------------- PUMP CONTROL FUNCTIONS -----------------
// ==========================================================
void stopPump() {
  analogWrite(PUMP, 0);
  Serial.println("Pump OFF");
}

// ==========================================================
// ----------------- ULTRASONIC FUNCTION --------------------
// ==========================================================
int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000); // Timeout 30ms
  int dist = duration * 0.034 / 2; // Convert to cm
  return dist;
}
