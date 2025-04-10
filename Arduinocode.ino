#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
 
// Firebase credentials
#define WIFI_SSID "FOE-Student"
#define WIFI_PASSWORD "abcd@1234"
#define FIREBASE_HOST "https://neo6mtesting123-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_API_KEY "AIzaSyA99e-vzF-T536HYE-T7mehji30e5By8UM"
 
#define IN1 2    // Motor 1 Direction
#define IN2 4
#define IN3 19   // Motor 2 Direction
#define IN4 18
 
const int wire1 = 33;  // First input wire (3.7V)
const int wire2 = 34;  // Second input wire (0V)
 

 
// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
 
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Use UART2: GPIO16 (RX), GPIO17 (TX)
 
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
 
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);
 
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 26, 25); // GPS module UART1 for GPS (TX=26, RX=25)
 
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
 
  // Set Firebase configuration
  config.database_url = FIREBASE_HOST;  // Firebase Realtime Database URL
  config.api_key = FIREBASE_API_KEY;    // Set Firebase API key
 
  // Initialize Firebase with config and auth
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
 
    // Sign up for Firebase (anonymous authentication)
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
      delay(500);
      w1 = digitalRead(wire1);
      w2 = digitalRead(wire2);
      if (w1 == HIGH && w2 == LOW) {
        stopCarFor10Seconds();        
      }
    
 
   
  }
 
  stopMotors();
}
 
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
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
// Read data from the GPS
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
 
  // If GPS location is updated
  if (gps.location.isUpdated()) {
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();
 
    Serial.print("Lat: ");
    Serial.print(latitude, 6);
    Serial.print(" | Lng: ");
    Serial.println(longitude, 6);
 
    // Send latitude and longitude to Firebase
    Firebase.RTDB.setDouble(&fbdo, "/location/latitude", latitude);
    Firebase.RTDB.setDouble(&fbdo, "/location/longitude", longitude);
 
 
  }
  delay(10000);
}
