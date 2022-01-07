#include <SPI.h>
#include <PN532_SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "PN532.h"

#include "constants.h"
#include "CardReader.h"
#include "RequestSender.h"


PN532_SPI pn532spi(SPI, SS_PIN);
PN532 nfc(pn532spi);
WiFiManager wifiManager;
CardReader cardReader(&nfc, CARD_READER_TIMEOUT);
RequestSender requestSender;

unsigned long lastTokenTime = 0;
char* token;


void setup(void) {
    Serial.begin(115200);
    bool success;
    
    //Uncomment this line to reset saved WiFi credentials for testing purposes
    //wifiManager.resetSettings();
    
    wifiManager.autoConnect(AUTOCONNECT_AP_SSID, AUTOCONNECT_AP_PASSWORD);
    Serial.println();

    if(WiFi.status() == WL_CONNECTED) {
        success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);

        if (success) {
            lastTokenTime = millis();
            Serial.print("Token acquired: "); Serial.println(token);
            Serial.println();
        } else {
            Serial.print("Failed to acquire token");
        }
        
    } else {
        Serial.println("WiFi Disconnected");
        while(WiFi.status() != WL_CONNECTED) delay(MAIN_LOOP_DELAY);
        Serial.println("WiFi Reconnected");
        Serial.println();
    }

    success = cardReader.begin();
    if (!success) while(true);
}


void loop(void) {
    bool success;
    uint64_t snr;
    
    success = cardReader.readCard(&snr);
    
    if (success) {
        Serial.print("  SNR: "); Serial.println(snr);
        Serial.println();
    }

    
    if ((millis() - lastTokenTime) > TOKEN_TIMEOUT) {
        
        if(WiFi.status() == WL_CONNECTED) {
            delete [] token;
            success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);

            if (success) {
                lastTokenTime = millis();
                Serial.print("Token acquired: "); Serial.println(token);
                Serial.println();
            } else {
                Serial.print("Failed to acquire token");
            }
            
        } else {
            Serial.println("WiFi Disconnected");
            while(WiFi.status() != WL_CONNECTED) delay(MAIN_LOOP_DELAY);
            Serial.println("WiFi Reconnected");
            Serial.println();
        }
    }

    
    delay(MAIN_LOOP_DELAY);
}
