#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 15 // D8
#define RST_PIN 2 // D4
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your card near the reader...");
}

void loop() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Get and print the card UID
  String cardUID = getCardNumber();
  Serial.print("Detected Card UID: ");
  Serial.println(cardUID);

  // Display product name based on the card UID
  displayProductName(cardUID);

  // Halt the card and stop encryption to avoid repeated readings
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(2000); // Delay to avoid repeated readings
}

String getCardNumber() {
  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      UID += "0";
    }
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  UID.toUpperCase(); // Convert to uppercase for consistency
  return UID;
}

void displayProductName(String cardUID) {
  // Map UIDs to product names
  if (cardUID == "7B76AE02") {
    Serial.println("Product: iPhone 16");
  } else if (cardUID == "DE85AE02") {
    Serial.println("Product: Crack Placements without DSA Book");
  } else {
    Serial.println("Product: Unknown Card");
  }
  
  Serial.println("------------");
}
