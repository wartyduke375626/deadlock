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
char* token = "";
unsigned long lastTestLogTime = 0;


void checkWiFiConnected() {
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected");
        while(WiFi.status() != WL_CONNECTED) delay(MAIN_LOOP_DELAY);
        Serial.println("WiFi Reconnected");
        Serial.println();
    }
}


void setup(void) {
    Serial.begin(115200);
    bool success;
    
    //Uncomment this line to reset saved WiFi credentials for testing purposes
    //wifiManager.resetSettings();
    
    wifiManager.autoConnect(AUTOCONNECT_AP_SSID, AUTOCONNECT_AP_PASSWORD);
    Serial.println();
    checkWiFiConnected();
    
    success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);
    if (success) lastTokenTime = millis();
    else { Serial.println("Failed to acquire token");  token = ""; } 

    success = cardReader.begin();
    if (!success) while(true);
}


void loop(void) {
    bool success;
    uint64_t card;

    if ((millis() - lastTokenTime) > TOKEN_TIMEOUT) {
        checkWiFiConnected();
        delete[] token;
        
        success = requestSender.requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);
        if (success) lastTokenTime = millis();
        else Serial.print("Failed to acquire token");
    }

    if ((millis() - lastTestLogTime) > TEST_LOG_TIMEOUT) {
        checkWiFiConnected();
        
        success = requestSender.sendLog(LOG_ENDPOINT, token, "0.0.0.1", "Test message", "Test message data", 1, "INFO");
        if (success) lastTestLogTime = millis();
        else { Serial.println("Failed to acquire token");  token = ""; }
    }

    
    success = cardReader.readCard(&card);
    if (success) {
        checkWiFiConnected();

        bool allowed;
        success = requestSender.requestAccess(ACCESS_ENDPOINT_BASE, token, card, 1, &allowed);
        if (success) {
            if (allowed) Serial.println("Access allowed");
            else Serial.println("Access denied");
        }
        else Serial.println("Failed to request access");
    }
    
    
    delay(MAIN_LOOP_DELAY);
}
