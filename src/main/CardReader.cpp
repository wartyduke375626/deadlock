#include "CardReader.h"
#include "PN532.h"
#include <SoftwareSerial.h>



CardReader::CardReader(PN532* nfc, uint16_t timeout) {
    this->nfc = nfc;
    this->timeout = timeout;
}


CardReader::~CardReader() {}


uint64_t CardReader::convertToDecimal(const uint8_t* uid, uint8_t uidLength) {
    uint64_t card = 0;
    
    for (size_t i=0; i<uidLength; ++i) {
        uint64_t tmp = uid[i];
        card += tmp << (8*i);
    }

    return card;
}


bool CardReader::begin() {
    nfc->begin();
    
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (!versiondata) {
        Serial.println("Didn't find PN53x board.\n");
        return false;
    }
    
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
    Serial.print("Firmware version: "); Serial.print((versiondata>>16) & 0xFF, DEC); Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    Serial.println("Waiting for an ISO14443A Card...\n");
        
    nfc->SAMConfig();
    return true;
}


bool CardReader::readCard(uint64_t* card) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, timeout);
    
    if (success) {
        *card = convertToDecimal(uid, uidLength);
        
        Serial.print("Found an ISO14443A card with UID value: "); Serial.println(*card);
        return true;
    }

    Serial.println("Waiting for an ISO14443A Card...\n");
    return false;
}
