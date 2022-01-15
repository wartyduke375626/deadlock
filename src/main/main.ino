#include <SPI.h>
#include <PN532_SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "PN532.h"

#include "constants.h"
#include "CardReader.h"
#include "DataFormatter.h"
#include "RequestSender.h"



PN532_SPI pn532spi(SPI, SS_PIN);
PN532 nfc(pn532spi);
WiFiManager wifiManager;
CardReader cardReader(&nfc, CARD_READER_TIMEOUT);

unsigned long lastTokenTime = 0;
char* token = "";
unsigned long lastTestLogTime = 0;


void checkWiFiConnected() {
    while(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected, waiting for reconnect...");
        delay(MAIN_LOOP_DELAY);
    }
    Serial.println("WiFi Reconnected");
    Serial.println();
}


void setup(void) {
    Serial.begin(115200);
    bool success;
    
    //Uncomment this line to reset saved WiFi credentials for testing purposes
    //wifiManager.resetSettings();
    
    wifiManager.autoConnect(AUTOCONNECT_AP_SSID, AUTOCONNECT_AP_PASSWORD);
    Serial.println();
    checkWiFiConnected();
    
    success = RequestSender::requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);
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
        
        success = RequestSender::requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);
        if (success) lastTokenTime = millis();
        else Serial.print("Failed to acquire token");
    }

    if ((millis() - lastTestLogTime) > TEST_LOG_TIMEOUT) {
        checkWiFiConnected();

        char* logData = DataFormatter::getLogData("0.0.0.1", "Test message", "Test message data", 1, "INFO");
        success = RequestSender::sendLog(LOG_ENDPOINT, token, logData);
        delete[] logData;
        if (success) lastTestLogTime = millis();
        else { Serial.println("Failed to acquire token");  token = ""; }
    }

    
    success = cardReader.readCard(&card);
    if (success) {
        checkWiFiConnected();

        bool allowed;
        char* accessEndpoint = DataFormatter::getAccessEndpoint(ACCESS_ENDPOINT_BASE, card, 1);
        success = RequestSender::requestAccess(accessEndpoint, token, &allowed);
        delete[] accessEndpoint;
        if (success) {
            if (allowed) Serial.println("Access allowed");
            else Serial.println("Access denied");
        }
        else Serial.println("Failed to request access");
    }
    
    
    delay(MAIN_LOOP_DELAY);
}
