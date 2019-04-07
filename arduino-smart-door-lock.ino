#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

// dataBlock1 - For card one
byte dataBlock1[]    = {
    0x01, 0x02, 0x03, 0x04, //  1,  2,  3,   4,
    0x05, 0x06, 0x07, 0x08, //  5,  6,  7,   8,
    0x09, 0x0a, 0xff, 0x0b, //  9,  10,  255, 11,
    0x0c, 0x0d, 0x0e, 0x0f  // 12,  13,  14,  15
};
// dataBlock2 - For card two
byte dataBlock2[]    = {
    0x02, 0x5C, 0x17, 0x9A, //  2,   92,  23,  154,
    0xD7, 0xE2, 0x4B, 0x1C, //  215, 126, 75,  28,
    0x62, 0x0A, 0xFF, 0x01, //  98,  10,  255, 1,
    0x78, 0xB7, 0x72, 0x15  //  120, 183, 114, 21
};
// dataBlock3 - For card three
byte dataBlock3[]    = {
    0x03, 0x33, 0xFE, 0xA5, //  3,   51,  254,  165,
    0x34, 0x41, 0x41, 0x08, //  52,  65,  65,   8,
    0x59, 0x0A, 0xFF, 0x0B, //  89,  10,  255,  11,
    0x98, 0xAD, 0xD6, 0x5F  //  152, 173, 214,  95
};

byte sector         = 1;
byte blockAddr      = 4;
byte trailerBlock   = 7;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8,7,6,5}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String inputCode, passCode;
bool isCorect = false;

int onLedPin = A4;
int correctLedPin = A5;
int falseLedPin = A6;
int relayDoorPin = 4;

void setup(){
  Serial.begin(115200);

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  passCode = String("4000");

  pinMode(onLedPin, OUTPUT);
  pinMode(correctLedPin, OUTPUT);
  pinMode(falseLedPin, OUTPUT);
  pinMode(relayDoorPin, OUTPUT);

  digitalWrite(onLedPin, HIGH);

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
  
void loop(){
  keypadFunc();

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  readNFC();
}

void keypadFunc() {
  char key = keypad.getKey();
    // just print the pressed key
   if (key){
    Serial.println(key);
  } 
  
  if (key == 'A') {
    Serial.println("Key A is pressed");
    closeDoor();
  }

  // this checkes if 4 is pressed, then do something. Here  we print the text but you can control something.
  if (key =='4'){
    Serial.println("Key 4 is pressed");
    inputCode += '4';
  }

  if (key =='0'){
    Serial.println("Key 0 is pressed");
    inputCode += '0';
  }

  if (key =='*'){
    Serial.println("Key * is pressed");
    inputCode = "";
    Serial.println("inputCode is: "+inputCode);
  }

  if (key =='#'){
    Serial.println("inputCode is: "+inputCode);
    Serial.println("Key # is pressed");
    if (inputCode == passCode){
      isCorect = true;
      Serial.println("The pincode is: true");
      
      digitalWrite(correctLedPin, HIGH);
      openDoor();
      delay(1000);
      digitalWrite(correctLedPin, LOW);
    } else {
      isCorect = false;
      Serial.println("The pincode is: false");
      digitalWrite(falseLedPin, HIGH);
      delay(1000);
      digitalWrite(falseLedPin, LOW);
    }
    inputCode = "";
  }
}

void readNFC() {
 /* // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
      return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
      return;*/

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7

  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  // Read data from the block
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
  }
  /*Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();
  Serial.println();*/
  
  // Check that data in block is what we have written
  // by counting the number of bytes that are equal
  Serial.println(F("Checking result..."));
  
  byte count = 0;
  if (buffer[0] == 0x01) {
      count = chack_bytes(buffer, dataBlock1);
  } else if (buffer[0] == 0x02) {
      count = chack_bytes(buffer, dataBlock2);
  } else if (buffer[0] == 0x03) {
      count = chack_bytes(buffer, dataBlock3);
  }
  
  if (count == 16) {
    openDoor();
  } else {
    falseInput();
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

byte chack_bytes(byte buffer[18], byte dataBlock[]) {
    byte count = 0;

    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i]){
            Serial.print("buffer == dataBlock in: "); Serial.println(i);
            count++;
        } else {
            Serial.print("buffer =/= dataBlock in: "); Serial.println(i);
            return count;
            break;
        }
    }
    return count;
}

void falseInput() {

}

void openDoor() {
  Serial.println("Open door");
  digitalWrite(relayDoorPin, HIGH);
  digitalWrite(onLedPin, LOW);
}

void closeDoor() {
  Serial.println("Close door");
  digitalWrite(relayDoorPin, LOW);
  digitalWrite(onLedPin, HIGH);
}