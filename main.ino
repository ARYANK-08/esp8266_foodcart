#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SPI.h>
#include <MFRC522.h>

// WiFi and Firebase credentials
#define WIFI_SSID "Aryan"
#define WIFI_PASSWORD "mystKira@06"

// RFID pins
#define SS_PIN 15  // D8
#define RST_PIN 2  // D4

// Buzzer pin
#define BUZZER_PIN 5  // D1

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// RFID object
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your card near the reader...");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Configure Firebase
  config.api_key = "api_key";
  config.database_url = "https:<>firebaseio.com/";
  auth.user.email = "mystkira7@gmail.com";
  auth.user.password = "123456";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void playSuccessBeep() {
  // Play a short beep for successful scan
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);  // Beep for 200ms
  digitalWrite(BUZZER_PIN, LOW);
}

void playErrorBeep() {
  // Play two short beeps for error
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void loop() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Get card UID
  String cardUID = getCardNumber();
  Serial.print("Detected Card UID: ");
  Serial.println(cardUID);

  // Get product name and send to Firebase
  String productName = getProductName(cardUID);
  bool success = sendToFirebase(cardUID, productName);

  // Play appropriate sound based on success
  if (success && productName != "Unknown Product") {
    playSuccessBeep();
  } else {
    playErrorBeep();
  }

  // Halt the card and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000);  // Reduced delay to make the system more responsive
}

String getCardNumber() {
  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      UID += "0";
    }
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  UID.toUpperCase();
  return UID;
}

String getProductName(String cardUID) {
  if (cardUID == "7B76AE02") {
    return "iPhone 16";
  } else if (cardUID == "DE85AE02") {
    return "Crack Placements without DSA Book";
  } else {
    return "Unknown Product";
  }
}

bool sendToFirebase(String cardUID, String productName) {
  // Create a JSON object with timestamp
  String timestamp = String(millis());
  
  if (Firebase.setString(fbdo, "/scannedProducts/" + cardUID + "/productName", productName)) {
    Serial.println("Product name sent to Firebase successfully");
    
    // Add timestamp
    Firebase.setString(fbdo, "/scannedProducts/" + cardUID + "/lastScanned", timestamp);
    
    // Add scan count
    Firebase.setInt(fbdo, "/scannedProducts/" + cardUID + "/scanCount", 
                   Firebase.getInt(fbdo, "/scannedProducts/" + cardUID + "/scanCount") + 1);
    
    return true;
  } else {
    Serial.println("Failed to send to Firebase");
    Serial.println("Reason: " + fbdo.errorReason());
    return false;
  }
}
