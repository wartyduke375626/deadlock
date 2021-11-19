#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include "CardReader.h"


#define SS_PIN 16


PN532_SPI pn532spi(SPI, SS_PIN);
PN532 nfc(pn532spi);
CardReader cardReader(&nfc);


void setup(void) {
    Serial.begin(115200);
    Serial.println("Hello!");

    bool init = cardReader.begin();

    if (!init) while (1);
}


void loop(void) {
    uint64_t snr;
    bool success = cardReader.readCard(&snr);
    
    if (success) {
        Serial.print("  SNR: "); Serial.println(snr);
        Serial.println();
    }
}
