#include "CardReader.h"
#include "PN532.h"
#include <SoftwareSerial.h>



CardReader::CardReader() {
    Serial.println("Default constructor called.");
}

CardReader::CardReader(PN532* nfc) {
    this->nfc = nfc;
}

CardReader::~CardReader() {}


bool CardReader::begin() {
    nfc->begin();
    
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (!versiondata) {
        Serial.println("Didn't find PN53x board");
        return false;
    }
    
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
        
    nfc->SAMConfig();
    Serial.println("Waiting for an ISO14443A Card ...");
    Serial.println("");
    return true;
}

uint64_t convert(const uint8_t *uid, uint8_t uidLength) {
    uint64_t snr = 0;
    
    for (size_t i=0; i<uidLength; ++i) {
        uint64_t tmp = uid[i];
        snr += tmp << (8*i);
    }

    return snr;
}

bool CardReader::readCard(uint64_t *snr) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
    
    success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
        Serial.print("  UID Value: "); nfc->PrintHex(uid, uidLength);

        *snr = convert(uid, uidLength);
        return true;
    }

    return false;
}
