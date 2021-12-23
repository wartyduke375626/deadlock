#include <SPI.h>
#include <PN532_SPI.h>
#include <ESP8266WiFi.h>
#include "PN532.h"

#include "functions.h"
#include "CardReader.h"
#include "RequestSender.h"


#define SS_PIN 16

#define CARD_READER_TIMEOUT 500
#define CARD_READ_DELAY 1000

#define WIFI_SSID "TP-Link_Kuch/Obyv_2.4GHz"
#define WIFI_PASSWORD "password"

#define TOKEN_ENDPOINT "https://web-app2.uniba.sk/students-backend/token/"
#define TOKEN_REQUEST_DATA "username=vita3&password=secret"
#define TOKEN_TIMEOUT 60000


PN532_SPI pn532spi(SPI, SS_PIN);
PN532 nfc(pn532spi);
CardReader cardReader(&nfc, CARD_READER_TIMEOUT);
RequestSender requestSender;

unsigned long lastTokenTime = 0;
char* token;


void setup(void) {
    Serial.begin(115200);
    Serial.println("Hello!");

    wifiConnect(WIFI_SSID, WIFI_PASSWORD);
    
    bool success = cardReader.begin();
    if (!success) while(true);

    if(WiFi.status() == WL_CONNECTED) {
        success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);

        if (success) {
            Serial.print("Token acquired: "); Serial.println(token);
            Serial.println();
        } else {
            Serial.print("Failed to acquire token");
        }
        
    } else {
        Serial.println("WiFi Disconnected");
    }
}


void loop(void) {
    bool success;
    
    if ((millis() - lastTokenTime) > TOKEN_TIMEOUT) {
        
        if(WiFi.status() == WL_CONNECTED) {
            delete [] token;
            success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);

            if (success) {
                Serial.print("Token acquired: "); Serial.println(token);
                Serial.println();
            } else {
                Serial.print("Failed to acquire token");
            }
            
        } else {
            Serial.println("WiFi Disconnected");
        }
        
        lastTokenTime = millis();
    }

    
    uint64_t snr;
    success = cardReader.readCard(&snr);
    
    if (success) {
        Serial.print("  SNR: "); Serial.println(snr);
        Serial.println();
    }
    
    delay(CARD_READ_DELAY);
}
