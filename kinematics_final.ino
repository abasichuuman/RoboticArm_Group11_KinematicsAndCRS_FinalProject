#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


#define NUM_SERVOS 6
#define SERVO_MIN 150
#define SERVO_MAX 600


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// Servo channel order: 0, 1, 2, 4, 5, 6
const int servoChannels[NUM_SERVOS] = {0, 1, 2, 4, 5, 6};


// Current positions for all servos
int currentPos[NUM_SERVOS] = {0, 1, 80, 0, 45, 45};


// Control flag for movement
bool isRunning = false;


// Target positions for different movements
const int movement1[NUM_SERVOS] = {158, 35, 0, 75, 90, 100};
const int movement2[NUM_SERVOS] = {158, 35, 0, 75, 90, 0};
const int movement3[NUM_SERVOS] = {0, 1, 5, 90, 45, 90};


// Movement sequences (indices into servoChannels array)
const int sequence1[] = {0, 5, 4, 3, 2, 1}; // Channels: 0, 6, 5, 4, 2, 1
const int sequence2[] = {0, 1, 2, 3, 4, 5}; // All at once (doesn't matter)
const int sequence3[] = {1, 2, 3, 4, 0, 5}; // Channels: 1, 2, 4, 5, 0, 6


void setup() {
 Serial.begin(115200);
 pwm.begin();
 pwm.setPWMFreq(50);
 delay(10);
  // Move to initial position
 writeAllServos(currentPos);
 delay(1000);
  Serial.println("Servo controller ready!");
 Serial.println("Type 'start' to begin movement sequence");
 Serial.println("Type 'stop' to halt movement");
}


void loop() {
 // Check for serial commands
 if (Serial.available() > 0) {
   String command = Serial.readStringUntil('\n');
   command.trim();
   command.toLowerCase();
  
   if (command == "start") {
     isRunning = true;
     Serial.println("Starting movement sequence...");
   } else if (command == "stop") {
     isRunning = false;
     Serial.println("Stopping movement sequence");
   }
 }
  // Only run movement sequence if enabled
 if (isRunning) {
   moveSequentialTo(movement1, sequence1, NUM_SERVOS);
   if (!isRunning) return; // Check if stopped during movement
   delay(50);
  
   moveSmoothTo(movement2);
   if (!isRunning) return;
   delay(50);
  
   moveSequentialTo(movement3, sequence3, NUM_SERVOS);
   if (!isRunning) return;
   delay(50);
 }
}


// Move servos in a specific sequence
void moveSequentialTo(const int target[], const int sequence[], int seqLength) {
 for (int s = 0; s < seqLength; s++) {
   if (!isRunning) return; // Stop immediately if commanded
  
   int servoIndex = sequence[s];
  
   // Move this servo to target position
   while (currentPos[servoIndex] != target[servoIndex]) {
     if (!isRunning) return; // Check during movement
    
     if (currentPos[servoIndex] < target[servoIndex]) {
       currentPos[servoIndex]++;
     } else {
       currentPos[servoIndex]--;
     }
     writeServo(servoIndex, currentPos[servoIndex]);
     delay(10);
   }
 }
}


// Smooth movement of all servos simultaneously to a target array
void moveSmoothTo(const int target[]) {
 bool moving = true;
  while (moving) {
   if (!isRunning) return; // Stop immediately if commanded
  
   moving = false;
  
   for (int i = 0; i < NUM_SERVOS; i++) {
     if (currentPos[i] != target[i]) {
       moving = true;
      
       if (currentPos[i] < target[i]) {
         currentPos[i]++;
       } else {
         currentPos[i]--;
       }
      
       writeServo(i, currentPos[i]);
     }
   }
   delay(5);
 }
}


// Write all servos at once
void writeAllServos(const int pos[]) {
 for (int i = 0; i < NUM_SERVOS; i++) {
   writeServo(i, pos[i]);
 }
}


// Write a single servo
void writeServo(int index, int angle) {
 int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
 pwm.setPWM(servoChannels[index], 0, pulse);
}

