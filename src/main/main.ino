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


void sendLog(const IPAddress &ip, const char* msg, const char* data, unsigned int levelNo, const char* level) {
    if (checkWiFiConnectedAndTokenExpired()) resetToken(true);

    char* logData = DataFormatter::getLogData(ip, msg, data, levelNo, level);
    bool success = RequestSender::sendLog(LOG_ENDPOINT, token, logData);
    delete[] logData;
    //TODO: What if RequestSender::sendLog fails?
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
        sendLog(IP_ADDRESS, "NFC module fatal error", "Communication with NFC module failed.", 5, "ERROR");
        ESP.restart();
    }
    else sendLog(IP_ADDRESS, "Successful setup", "The board has set up successfully, no errors have occured during startup.", 1, "INFO");
}


void loop(void) {
    bool success;
    uint64_t card;

    // This also regularly checks if TOKEN_TIMOUT has expired and resets the token if so.
    sendLog(IP_ADDRESS, "Board is active", "The board is active and waiting for cards. No errors have occured.", 1, "INFO");
    
    success = cardReader.readCard(&card);
    if (success) {
        
        if (checkWiFiConnectedAndTokenExpired()) resetToken(true);
        
        char* accessEndpoint = DataFormatter::getAccessEndpoint(ACCESS_ENDPOINT_BASE, card, DEVICE_ID);
        bool access;
        success = RequestSender::requestAccess(accessEndpoint, token, &access);
        delete[] accessEndpoint;
        
        if (success) {
            
            if (access) {
                Serial.println("Access granted.\n");
                sendLog(IP_ADDRESS, "Access granted", "Access was granted to user with card " + card, 1, "INFO");
            }
            else {
                Serial.println("Access denied.\n");
                sendLog(IP_ADDRESS, "Access denied", "Access was denied for user with card " + card, 1, "INFO");
            }
        }
        
        else sendLog(IP_ADDRESS, "Access request failed", "The board failed to send an access request.", 5, "ERROR");
    }
    
    delay(MAIN_LOOP_DELAY);
}
