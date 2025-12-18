
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Pinii pentru senzorul utrasonic 
// Masoara distanta folosind unde sonore 
// - Trimite un puls pe TRIG
// - Masoara cat dureaza ecoul sa vina pe ECHO
// - Distanta = (timp x viteza_sunetului)/2
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;
const int DISTANCE_THRESHOLD = 150;

//Fotorezistori 
//Masoara intensitatea luminii 
// - rezistenta scade cand creste lumina -> valoare analogica mai mare
const int LDR_LEFT = A0;
const int LDR_RIGHT = A1;

// Servo continuu 360
const int ROTATION_SERVO_CHANNEL = 4;
int SERVO_STOP = 315; // Valoarea la care servoul se opreste din rotit
bool calibrationMode = false; // Calibrare

//Driver PWM(16 canale) 
//Un cip care controleaza pana la 16 servomotoare simultan
//Arduino are doar 6 pini PWM, noi avem nevoie de 5 servomotoare care comunica prin I2C
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//Servomotoarele standard de 180 de grade 
#define SERVO_FREQ 50
const int PETALS_CLOSED = 125; //unghi mai mic
const int PETALS_OPEN = 485; //unghi mare

int currentPetalPosition = PETALS_CLOSED;
bool petalsOpen = false;
float currentDistance = 999;
int lightLeft = 0;
int lightRight = 0;

// Light tracking parameters
const int LIGHT_THRESHOLD = 20;        // Diferență mica de lumina pentru trigger
const int ROTATION_DURATION = 800;     // Durata rotatiei
const int SERVO_SPEED_OFFSET = 25;     // Viteza de rotatie

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_LEFT, INPUT);
  pinMode(LDR_RIGHT, INPUT);
  
  pwm.begin();   //Initializeaza driverul
  pwm.setPWMFreq(SERVO_FREQ); //Seteaza 50Hz (20ms perioada)
   
  // Start rotation servo STOPPED cu valoarea gasita
  pwm.setPWM(ROTATION_SERVO_CHANNEL, 0, SERVO_STOP);
  
  // pornim cu petelele inchise 
  setPetalPosition(PETALS_CLOSED);
  
  
  Serial.println("Servo STOP value: " + String(SERVO_STOP));
  Serial.println("Light threshold: " + String(LIGHT_THRESHOLD));
  Serial.println("System running normally...");
  Serial.println();
  
  delay(2000);
}

void loop() {
  
  currentDistance = measureDistance();  //Masoara distanta
  readLightSensors();  //Citeste LDR-urile 
  controlPetals();  //Deschide/inchide petalele
  trackLight(); //Urmareste lumina 
  printDebugInfo();  //Afiseaza datele
  delay(100);
}


float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);  
  digitalWrite(TRIG_PIN, LOW);
  
  //trimite un puls ultrasonic de 10 microsecunde 
  float duration_us = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration_us == 0) 
    return 999;  // obiectul e prea departe 
  return (duration_us * 0.0343) / 2;
  // 0.0343 = viteza sunetului si impartim la 2 pentru ca sunetul face dus intors
}



//citim senzorii de lumina 
void readLightSensors() {
  lightLeft = analogRead(LDR_LEFT);
  lightRight = analogRead(LDR_RIGHT);
}


void controlPetals() {
  if (currentDistance < DISTANCE_THRESHOLD && petalsOpen) {
    Serial.println(" Closing petals...");
    closePetals();     //daca cineva se apropie, inchidem petalele
    petalsOpen = false;
  } else if (currentDistance >= DISTANCE_THRESHOLD && !petalsOpen) {
    Serial.println("Opening petals...");
    openPetals();     //Nimeni aproape -> deschidem petalele 
    petalsOpen = true;  //ca sa nu se tot deschida si inchida 
  }
}


// Functiile de control ale petalelor
// de ce facem += 10
// de la 125 la 458 sunt 360 de unitati
// Daca facem pasi de 10 = 36 de pasi 
// 36 x 50 ms = 1.8 secunde pentru deschiderea lina si completa a petalelor
void openPetals() {
  for (int pos = currentPetalPosition; pos <= PETALS_OPEN; pos += 10) {
    setPetalPosition(pos);
    currentPetalPosition = pos;
    delay(50);
  }
  Serial.println("Petals fully open!");
}

void closePetals() {
  for (int pos = currentPetalPosition; pos >= PETALS_CLOSED; pos -= 10) {
    setPetalPosition(pos);
    currentPetalPosition = pos;
    delay(50);
  }
  Serial.println("Petals fully closed!");
}

void setPetalPosition(int position) {
  pwm.setPWM(0, 0, position);
  pwm.setPWM(1, 0, position);
  pwm.setPWM(2, 0, position);
  pwm.setPWM(3, 0, position);
}



void trackLight() {
  //le-am facut statice ca sa-si pastreze valoarea intre apeluri
  static unsigned long lastRotationTime = 0;
  static bool isRotating = false;
  
  int lightDifference = lightRight - lightLeft;
  
  
  if (isRotating) {
    if (millis() - lastRotationTime >= ROTATION_DURATION) {
      pwm.setPWM(ROTATION_SERVO_CHANNEL, 0, SERVO_STOP);
      isRotating = false;
      Serial.println("Rotation stopped");
    }
    return;
  }
  
  
  if (abs(lightDifference) > LIGHT_THRESHOLD) {
    int speed;
    const char* direction;
    
    if (lightDifference > 0) {
      speed = SERVO_STOP + SERVO_SPEED_OFFSET; // Dreapta 315 + 25 = 340
      direction = "RIGHT";
    } else {
      speed = SERVO_STOP - SERVO_SPEED_OFFSET; // Stanga 315 - 25 = 290
      direction = "LEFT";
    }
    
    //porneste rotatia cu viteza calculata 
    pwm.setPWM(ROTATION_SERVO_CHANNEL, 0, speed);
    lastRotationTime = millis();
    isRotating = true;
    
    Serial.print("Rotating ");
    Serial.print(direction);
    Serial.print(" | Light diff: ");
    Serial.print(lightDifference);
    Serial.println(" | Duration: 800ms");
  }
}



//asta putem sa stergem 
void printDebugInfo() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    int lightDifference = lightRight - lightLeft;
    
    Serial.print("📏 Distance: ");
    Serial.print(currentDistance);
    Serial.print("cm |  Petals: ");
    Serial.print(petalsOpen ? "OPEN" : "CLOSED");
    
    Serial.print(" |  Light L:");
    Serial.print(lightLeft);
    Serial.print(" R:");
    Serial.print(lightRight);
    Serial.print(" Diff:");
    Serial.print(lightDifference);
    
    // Indicator vizual
    Serial.print(" | ");
    if (abs(lightDifference) <= LIGHT_THRESHOLD) {
      Serial.print("BALANCED");
    } else if (lightDifference > 0) {
      Serial.print("→ RIGHT BRIGHTER");
    } else {
      Serial.print("← LEFT BRIGHTER");
    }
    
    Serial.println();
    lastPrint = millis();
  }
}