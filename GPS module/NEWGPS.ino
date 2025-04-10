#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "https://neo6mtesting123-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_API_KEY "AIzaSyA99e-vzF-T536HYE-T7mehji30e5By8UM"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Use UART2: GPIO16 (RX), GPIO17 (TX)

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // GPS Baud and pins

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
}

void loop() {
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

  delay(3000); // Delay for 3 seconds before sending again
}
