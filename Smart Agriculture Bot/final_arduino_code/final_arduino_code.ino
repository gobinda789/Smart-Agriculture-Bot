
#include <Servo.h>
#include <DHT.h>  // Add DHT library
#include <NewPing.h> // For ultrasonic sensor

#define MOISTURE_SENSOR_PIN A0

// Add DHT22 configuration
#define DHTPIN 22       // Pin connected to DHT22
#define DHTTYPE DHT22   // DHT22 sensor type
DHT dht(DHTPIN, DHTTYPE);  // Create DHT object

// Ultrasonic Sensor Configuration
#define TRIGGER_PIN 24
#define ECHO_PIN 25
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#define MOTOR1_RPWM 5
#define MOTOR1_LPWM 4
#define MOTOR2_RPWM 2
#define MOTOR2_LPWM 3
#define IN1 7
#define IN2 8
#define ENA 9
//for encoder motor
#define IN3 26
#define IN4 27
#define ENB 28

Servo servo1; // Flattening
Servo servo2; // Ploughing
Servo servo3; // Seed sowing
Servo servo4; // Moisture sensor

// Seed Sowing Variables
bool seedingActive = false;
unsigned long lastSeedTime = 0;

//encoder
const int encoderPin = 21;
const float wheelDiameter = 0.025;
const int slotsPerRevolution = 20;
volatile unsigned long pulseCount = 0;
const float circumference = PI * wheelDiameter;
const float distancePerPulse = circumference / slotsPerRevolution;
float currentLength = 0;
float currentBreadth = 0;


enum AutoState {
  IDLE,              // Bot is not in autonomous mode
  MEASURE_LENGTH,    // Measuring the length of the field
  TURN_1right, // Turning after measuring length
  MEASURE_BREADTH,   // Measuring the breadth of the field
  TURN_2right
};

AutoState currentState = IDLE; // Current state of the bot
unsigned long turnStartTime = 0; // Timestamp for turn delay
int count=0;

void setup() {
   dht.begin();
    pinMode(MOISTURE_SENSOR_PIN, INPUT);

    pinMode(MOTOR1_RPWM, OUTPUT);
    pinMode(MOTOR1_LPWM, OUTPUT);
    pinMode(MOTOR2_RPWM, OUTPUT);
    pinMode(MOTOR2_LPWM, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
    // encoder motor
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);

    servo1.attach(12);
    servo2.attach(10);
    servo3.attach(11);
    servo4.attach(13);

    // Initialize servo positions
    servo1.write(0);  // Flattening up
    servo2.write(30);   // Ploughing up
    servo3.write(15);  // Seed sowing up
    servo4.write(40);   // Moisture sensor up

  Serial.begin(115200); // Initialize Serial communication with ESP32

  pinMode(encoderPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPin), countPulse, FALLING);

}

void loop() {
  
      // Obstacle Detection
  unsigned int distance = sonar.ping_cm();
  if(distance > 0 && distance < 15) { 
    emergencyStop();
   
  }
     autonomousLoop();

 // Seed Sowing Automation
  if(seedingActive && (millis() - lastSeedTime >= 500)) {
    servo3.write(servo3.read() == 0 ? 15 : 0);
    lastSeedTime = millis();
  }

  static unsigned long lastSensorUpdate = 0;

    // Read DHT sensor and moisture  every 5 seconds
     if (millis() - lastSensorUpdate >= 2000) {
    lastSensorUpdate = millis();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int moistureValue = analogRead(MOISTURE_SENSOR_PIN);
    int moisturePercent = map(moistureValue, 1023, 0, 0, 100); // Convert to percentage

    
    if (!isnan(humidity) && !isnan(temperature)) {
    Serial.print("SENSOR_DATA:");
        Serial.print(temperature);
        Serial.print(",");
        Serial.print(humidity);
        Serial.print(",");
        Serial.print(moisturePercent);
       
    }
  }
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

         if (command == "SEED_SOWING_ON") seedingActive = true;
         else if (command == "SEED_SOWING_OFF") {
           seedingActive = false;
           servo3.write(15);
         }

        // Handle Commands
        if (command == "MOVE_FORWARD") moveForward(128);
        else if (command == "MOVE_BACKWARD") moveBackward(128);
        else if (command == "TURN_LEFT") turnLeft(128);
        else if (command == "TURN_RIGHT") turnRight(128);
        else if (command == "STOP") stopMotors();
        else if (command == "FLATTENING_UP") FlatenningUp();
        else if (command == "FLATTENING_DOWN") FlatenningDown();
        else if (command == "PLOUGHING_UP") ploughingUp();
        else if (command == "PLOUGHING_DOWN") ploughingDown();
        else if (command == "MOISTURE_UP") moistureUp();
        else if (command == "MOISTURE_DOWN") moistureDown();
        else if (command == "PUMP_ON") startPump();
        else if (command == "PUMP_OFF") stopPump();
        else if (command == "EMERGENCY_STOP") emergencyStop();
         else if (command == "AUTO_START") autoStart();
          else if (command == "LENGTH_MEASURE") start_L_measure();
          else if (command == "BREADTH_MEASURE") start_B_measure();
          else if (command == "STOP_LENGTH_MEASURE") stop_L_measuring();
          else if (command == "STOP_BREADTH_MEASURE") stop_B_measuring();
    }
}


   void  start_L_measure(){
      pulseCount = 0;
          moveForward(128);
          encodermotor();
    }

    void start_B_measure(){
    
        pulseCount = 0;
          moveForward(128);
          encodermotor();
    }

   void  stop_L_measuring(){
      stopMotors();
      stopencodermotor();
      currentLength = getCurrentDistance();
       Serial.print("LENGTH_DATA:");  
       Serial.println(currentLength); 
    }

    void stop_B_measuring(){
      stopMotors();
      stopencodermotor();
      currentBreadth = getCurrentDistance();
       Serial.print("BREADTH_DATA:"); 
       Serial.println(currentBreadth); 
    }

void ploughingUp(){
    servo2.write(30);
}
void ploughingDown(){
    servo2.write(10);
}

void FlatenningUp(){
    servo1.write(0); 
}
void FlatenningDown(){
    servo1.write(40); 
}
void moistureUp(){
    servo4.write(40); 
}
void moistureDown(){
    servo4.write(10); 
}


void moveForward(int speed) {
    analogWrite(MOTOR1_RPWM, speed);
    analogWrite(MOTOR1_LPWM, 0);
    analogWrite(MOTOR2_RPWM, speed);
    analogWrite(MOTOR2_LPWM, 0);
}

void moveBackward(int speed) {
    analogWrite(MOTOR1_RPWM, 0);
    analogWrite(MOTOR1_LPWM, speed);
    analogWrite(MOTOR2_RPWM, 0);
    analogWrite(MOTOR2_LPWM, speed);
}

void turnLeft(int speed) {
    analogWrite(MOTOR1_RPWM, 0);
    analogWrite(MOTOR1_LPWM, speed);
    analogWrite(MOTOR2_RPWM, speed);
    analogWrite(MOTOR2_LPWM, 0);
}

void turnRight(int speed) {
    analogWrite(MOTOR1_RPWM, speed);
    analogWrite(MOTOR1_LPWM, 0);
    analogWrite(MOTOR2_RPWM, 0);
    analogWrite(MOTOR2_LPWM, speed);
}

void stopMotors() {
    analogWrite(MOTOR1_RPWM, 0);
    analogWrite(MOTOR1_LPWM, 0);
    analogWrite(MOTOR2_RPWM, 0);
    analogWrite(MOTOR2_LPWM, 0);
}

void startPump() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 128);
}

void stopPump() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
}
void emergencyStop() {
  ploughingUp();
                          FlatenningUp();
                          stopPump();
                          seedingActive = false; // Disable seeding
                          servo3.write(15); // Reset seed servo to "up" position
    stopMotors();
    stopencodermotor();
    currentState = IDLE;
}


void encodermotor() {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 128);
}
void stopencodermotor() {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
}

float getCurrentDistance() {
  noInterrupts();
  float distance = pulseCount * distancePerPulse;
  interrupts();
  return distance;
}

void countPulse() {
  pulseCount++;
}

void autoStart() {
  
  ploughingDown();
  FlatenningDown();
  startPump();
  seedingActive = true; 
  delay(1000);

  if (currentState == IDLE) {
    
    pulseCount = 0;
    currentState = MEASURE_LENGTH;
  }
}

                  void autonomousLoop() {
                  unsigned int distance = sonar.ping_cm();
                  if (distance > 0 && distance < 15) {
                    emergencyStop(); // This now sets currentState = IDLE
                    return; // Exit the function to avoid further motor commands
                  }
                   if(seedingActive && (millis() - lastSeedTime >= 500)) {
                      servo3.write(servo3.read() == 0 ? 15 : 0);
                      lastSeedTime = millis();
                    }

                    switch (currentState) {

                      case MEASURE_LENGTH:

                        encodermotor();
                        moveForward(128);

                        // Check if length measurement is complete
                        if (getCurrentDistance() >= currentLength) {
                          stopencodermotor();
                          stopMotors();
                          turnStartTime = millis(); // Record turn start time
                          currentState = TURN_1right;
                        }
                        break;

                      case TURN_1right:
                          turnRight(128);
                          delay(3500);
                          stopMotors();
                          pulseCount = 0; // Reset encoder for breadth measurement
                          currentState = MEASURE_BREADTH;
                        
                        break;

                      case MEASURE_BREADTH:
                        encodermotor();
                        moveForward(128);
                        // Check if breadth measurement is complete
                        if (getCurrentDistance() >= currentBreadth) {
                          stopencodermotor();
                          stopMotors();
                          turnStartTime = millis(); // Record turn start time
                          currentState = TURN_2right;
                        }
                        break;

                      case TURN_2right:

                        // // Turn right for 2.2 seconds (non-blocking)
                        // if (millis() - turnStartTime < 3000) {
                          turnRight(128);
                          delay(3500);
                          stopMotors();
                          if (count==0){
                          count++;
                          pulseCount = 0;
                          currentState = MEASURE_LENGTH;
                          }
                          else{
                          ploughingUp();
                          FlatenningUp();
                          stopPump();
                          seedingActive = false; // Disable seeding
                          servo3.write(15); // Reset seed servo to "up" position
                          stopencodermotor();
                            currentState = IDLE; // Autonomous mode complete
                          }
                         
                        
                        break;

                      case IDLE:
                     
                        break;
                    }
                  }


