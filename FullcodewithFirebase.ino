#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase credentials
#define WIFI_SSID "FOE-Student"
#define WIFI_PASSWORD "abcd@1234"
#define FIREBASE_HOST "https://neo6mtesting123-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_API_KEY "AIzaSyA99e-vzF-T536HYE-T7mehji30e5By8UM"

// Motor Pins
#define IN1 2
#define IN2 4
#define IN3 19
#define IN4 18
#define ENA 32
#define ENB 22

// Metal Detector Input Pins
const int wire1 = 33;  // 3.7V signal
const int wire2 = 34;  // 0V signal

// PWM settings
const int pwmDuty = 128; // Half speed

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// GPS setup on UART1 (TX=26, RX=25)
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1

void setup() {
  // Motor pin setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);

  // Serial monitors
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 25, 26); // RX=25, TX=26

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Firebase configuration
  config.api_key = FIREBASE_API_KEY;
  config.database_url = FIREBASE_HOST;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up successful!");
  } else {
    Serial.println("Firebase sign-up failed: " + fbdo.errorReason());
  }
}

void loop() {
  int w1 = digitalRead(wire1);
  int w2 = digitalRead(wire2);

  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 15; i++) {
      moveForward();
      analogWrite(ENA, pwmDuty);
      analogWrite(ENB, pwmDuty);
      delay(500);

      w1 = digitalRead(wire1);
      w2 = digitalRead(wire2);
      if (w1 == HIGH && w2 == LOW) {
        stopCarAndSendLocation();
      }
    }

    moveRight();
    delay(1250);

    for (int i = 0; i < 15; i++) {
      moveForward();
      analogWrite(ENA, pwmDuty);
      analogWrite(ENB, pwmDuty);
      delay(500);

      w1 = digitalRead(wire1);
      w2 = digitalRead(wire2);
      if (w1 == HIGH && w2 == LOW) {
        stopCarAndSendLocation();
      }
    }

    moveLeft();
    delay(1250);
  }

  stopMotors();
}

// ========== MOTOR CONTROL ==========

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

// ========== METAL DETECTION + GPS LOCATION ==========

void stopCarAndSendLocation() {
  stopMotors();
  Serial.println("Metal detected! Waiting for GPS...");

  unsigned long start = millis();
  bool gpsSent = false;

  // Wait up to 10 seconds for GPS data
  while (millis() - start < 10000) {
    while (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated() && !gpsSent) {
      double lat = gps.location.lat();
      double lng = gps.location.lng();

      Serial.print("Lat: "); Serial.println(lat, 6);
      Serial.print("Lng: "); Serial.println(lng, 6);

      if (Firebase.RTDB.setDouble(&fbdo, "/location/latitude", lat)) {
        Serial.println("Latitude sent to Firebase.");
      } else {
        Serial.println("Failed to send latitude: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setDouble(&fbdo, "/location/longitude", lng)) {
        Serial.println("Longitude sent to Firebase.");
      } else {
        Serial.println("Failed to send longitude: " + fbdo.errorReason());
      }

      gpsSent = true;
    }
  }

  delay(10000); // Still wait for 10 seconds before moving again
}
