/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 *
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 *
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

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

/**
 * Initialize.
 */
void setup() {
    Serial.begin(115200); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

}

/**
 * Main loop.
 */
void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    //Serial.print(F("Card UID:"));
    //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    //Serial.println();
    //Serial.print(F("PICC type: "));
    //MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
   /* if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }*/

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    /*Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }*/

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
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    // Authenticate using key B
    /*Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }*/

    
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
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
    }

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
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
