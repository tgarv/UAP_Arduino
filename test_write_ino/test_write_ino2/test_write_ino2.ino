/**************************************************************************/
/*! 
    @file     mifareclassic_updatendef.pde
    @author   Adafruit Industries
    @license  BSD (see license.txt)

    Updates a sector that is already formatted for NDEF (using
    mifareclassic_formatndef.pde for example), inserting a new url

    This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
    This library works with the Adafruit NFC Shield 
      ----> https://www.adafruit.com/products/789
 
    Check out the links above for our tutorials and wiring diagrams 
    These chips use I2C to communicate

    Adafruit invests time and resources providing this open source code, 
    please support Adafruit and open-source hardware by purchasing 
    products from Adafruit!

*/
/**************************************************************************/

#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

/*  
    We can encode many different kinds of pointers to the card,
    from a URL, to an Email address, to a phone number, and many more
    check the library header .h file to see the large # of supported
    prefixes! 
*/
// For a http://www. url:
//char * data = "d224206170706c69636174696f6e2f636f6d2e6578616d706c652e616e64726f69642e6265616d4265616d206d65207570210a0a4265616d2054696d653a2031363a30303a3033";
// char * data = "example.com/thisisalongstringwithsomewordsthatihopewillgetwrittentothetagsoicangraduate";
char * data = "example.com/example1example2example3example4exampl5example6example7example8example9";
uint8_t ndefprefix = NDEF_URIPREFIX_HTTP_WWWDOT;

// for an email address
//const char * url = "mail@example.com";
//uint8_t ndefprefix = NDEF_URIPREFIX_MAILTO;

// for a phone number
//const char * url = "+1 212 555 1212";
//uint8_t ndefprefix = NDEF_URIPREFIX_TEL;


void setup(void) {
  Serial.begin(115200);
  Serial.println("Looking for PN532...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
}

void loop(void) {
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  bool authenticated = false;               // Flag to indicate if the sector is authenticated

  // Use the default NDEF keys (these would have have set by mifareclassic_formatndef.pde!)
  uint8_t keya[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
  uint8_t keyb[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
  uint8_t keyc[6] = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5 };
  uint8_t keyd[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  Serial.println("Place your NDEF formatted Mifare Classic card on the reader to update the");
  Serial.println("NDEF record and press any key to continue ...");
  // Wait for user input before proceeding
  while (!Serial.available());
  // a key was pressed1
  while (Serial.available()) Serial.read();
    
  // Wait for an ISO14443A type card (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) 
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    // Make sure this is a Mifare Classic card
    if (uidLength != 4)
    {
      Serial.println("Ooops ... this doesn't seem to be a Mifare Classic card!"); 
      return;
    }
    
    // We probably have a Mifare Classic card ... 
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

    // Check if this is an NDEF card (using first block of sector 1 from mifareclassic_formatndef.pde)
    // Must authenticate on the first key using 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7
    
    // Serial.println("Authentication succeeded (seems to be an NDEF/NFC Forum tag) ...");
    
    // Serial.println("Updating sector 1 with URI as NDEF Message");
    
    nfc.mifareclassic_WriteString(uid, uidLength, keyb, 12, "Entirely new string\0");
    //success = nfc.mifareclassic_ClearData(uid, uidLength, keyb);
  }
  
  // Wait a bit before trying again
  Serial.println("\n\nDone!");
  delay(1000);
  Serial.flush();
  while(Serial.available()) Serial.read();
}
