#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "BluetoothSerial.h"
 
 
#define IN1 2    // Motor 1 Direction
#define IN2 4
#define IN3 19   // Motor 2 Direction
#define IN4 18
 
const int wire1 = 33;  // First input wire (3.7V)
const int wire2 = 34;  // Second input wire (0V)
 
// PWM config
const int pwmFreq = 1000;
const int pwmResolution = 8; // 8-bit (0-255)
const int pwmDuty = 128;     // Half speed
 
bool alreadySent = false;
 
// GPS and Bluetooth setup
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);       // UART1 for GPS (TX=26, RX=25)
BluetoothSerial SerialBT;          // Bluetooth Serial
 
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
 
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);
 
  // Set up PWM for ENA and ENB
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 26, 25);  // GPS module UART1 for GPS (TX=26, RX=25)
  SerialBT.begin("Metal Detector GPS");       // Bluetooth name
 
  Serial.println("Waiting for GPS signal...");
  SerialBT.println("Bluetooth connected! Waiting for GPS signal...");
}
 
void loop() {
  int w1 = digitalRead(wire1);
  int w2 = digitalRead(wire2);
 
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 15; i++) {
      moveForward();
      delay(500);
      w1 = digitalRead(wire1);
      w2 = digitalRead(wire2);
      if (w1 == HIGH && w2 == LOW) {
        stopCarFor10Seconds();        
      }
    }
 
    moveRight();
    delay(1250);
 
    for (int i = 0; i < 15; i++) {
      moveForward();
      delay(500);
      w1 = digitalRead(wire1);
      w2 = digitalRead(wire2);
      if (w1 == HIGH && w2 == LOW) {
        stopCarFor10Seconds();        
      }
    }
 
    moveLeft();
    delay(1250);
  }
 
  stopMotors();
}
 
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
}
 
void moveRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  }
 
void moveLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
}
 
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
 
}
 
void stopCarFor10Seconds() {
  stopMotors();
  Serial.println("Detected condition! Stopping car for 10 seconds...");
  SerialBT.println("Metal Detected! Stopping car for 10 seconds...");
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
 
    if (gps.location.isUpdated()) {
      double lat = gps.location.lat();
      double lng = gps.location.lng();
      double alt = gps.altitude.meters();
      int sats = gps.satellites.value();
 
      // Display on Serial Monitor
      Serial.println("---- GPS Data ----");
      Serial.print("Latitude: "); Serial.println(lat, 6);
      Serial.print("Longitude: "); Serial.println(lng, 6);
      Serial.print("Altitude: "); Serial.println(alt);
      Serial.print("Satellites: "); Serial.println(sats);
      Serial.println("------------------");
 
      // Send via Bluetooth
      SerialBT.println("---- GPS Data ----");
      SerialBT.print("Latitude: "); SerialBT.println(lat, 6);
      SerialBT.print("Longitude: "); SerialBT.println(lng, 6);
      SerialBT.print("Altitude: "); SerialBT.println(alt);
      SerialBT.print("Satellites: "); SerialBT.println(sats);
      SerialBT.println("------------------");
    }
  }
  delay(10000);
}
