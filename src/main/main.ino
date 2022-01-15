#include <SPI.h>
#include <PN532_SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "PN532.h"

#include "constants.h"
#include "CardReader.h"
#include "DataFormatter.h"
#include "RequestSender.h"


#define IP_ADDRESS IPAddress(0, 0, 0, 1)

// Use this instead if you want to send your current local IP in log requests.
//#define IP_ADDRESS WiFi.localIP();



PN532_SPI pn532spi(SPI, SS_PIN);
PN532 nfc(pn532spi);

WiFiManager wifiManager;
CardReader cardReader(&nfc, CARD_READER_TIMEOUT);

unsigned long lastTokenTime = 0;
char* token;



// Checks if WiFi is connected, if not will loop untill WiFi reconnects. Returns 'true' if TOKEN_TIMEOUT has expired during this process.
bool checkWiFiConnectedAndTokenExpired() {
    if (WiFi.status() != WL_CONNECTED) {
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Fatal error: WiFi disconnected, waiting for reconnect...\n");
            delay(MAIN_LOOP_DELAY);
        }

        Serial.println("WiFi reconnected.\n");
    }

    return (millis() - lastTokenTime) > TOKEN_TIMEOUT;
}


void resetToken(bool deleteOld) {
    while (true) {
        
        if (deleteOld) delete[] token;
        bool success = RequestSender::requestToken(TOKEN_ENDPOINT, TOKEN_REQUEST_DATA, &token);
        
        if (success) {
            lastTokenTime = millis();
            return;
        }

        Serial.println("Fatal error: failed to acquire access token, retrying...\n");
        delay(MAIN_LOOP_DELAY);
    }
}



void setup(void) {
    Serial.begin(115200);
    
    // Uncomment this line to reset saved WiFi credentials for testing purposes.
    //wifiManager.resetSettings();
    
    wifiManager.autoConnect(AUTOCONNECT_AP_SSID, AUTOCONNECT_AP_PASSWORD);
    Serial.println();

    checkWiFiConnectedAndTokenExpired();
    resetToken(false);

    bool success = cardReader.begin();
    if (!success) {
        if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
        
        char* logData = DataFormatter::getLogData(IP_ADDRESS, "NFC module fatal error", "Communication with NFC module failed.", 5, "ERROR");
        success = RequestSender::sendLog(LOG_ENDPOINT, token, logData);
        delete[] logData;
        
        ESP.restart();
    }
    else {
        if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
        
        char* logData = DataFormatter::getLogData(IP_ADDRESS, "Successful setup", "The board has set up successfully, no errors have occured during startup.", 1, "INFO");
        success = RequestSender::sendLog(LOG_ENDPOINT, token, logData);
        delete[] logData;
    }
}


void loop(void) {
    bool success;
    uint64_t card;
    char* buffer;
    
    if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
    
    success = cardReader.readCard(&card);
    if (success) {
        
        if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
        
        buffer = DataFormatter::getAccessEndpoint(ACCESS_ENDPOINT_BASE, card, DEVICE_ID);
        bool allowed;
        success = RequestSender::requestAccess(buffer, token, &allowed);
        delete[] buffer;
        
        if (success) {
            if (allowed) {
                Serial.println("Access granted.\n");
                
                if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
                
                buffer = DataFormatter::getLogData(IP_ADDRESS, "Access granted", "Access was granted to user with card " + card, 1, "INFO");
                success = RequestSender::sendLog(LOG_ENDPOINT, token, buffer);
                delete[] buffer;
            }
            else {
                Serial.println("Access denied.\n");

                if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
                
                buffer = DataFormatter::getLogData(IP_ADDRESS, "Access denied", "Access was denied for user with card " + card, 1, "INFO");
                success = RequestSender::sendLog(LOG_ENDPOINT, token, buffer);
                delete[] buffer;
            }
        }
        else {
            if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
            
            buffer = DataFormatter::getLogData(IP_ADDRESS, "Access request failed", "The board failed to send an access request.", 5, "ERROR");
            success = RequestSender::sendLog(LOG_ENDPOINT, token, buffer);
            delete[] buffer;
        }
    }
    
    delay(MAIN_LOOP_DELAY);
}
