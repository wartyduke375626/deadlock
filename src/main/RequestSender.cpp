#include "RequestSender.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>



RequestSender::RequestSender() {}

RequestSender::~RequestSender() {}

bool RequestSender::requestToken(const char* server, const char* requestData, char** token) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    Serial.println("Sending token request...");
    
    https.begin(client, server);

    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    https.addHeader("Content-Length", String(strlen(requestData)));

    int responseCode = https.POST(requestData);
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.println("Failed to acquire token");
        Serial.print("HTTP Response code: "); Serial.println(responseCode);
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());
    
    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data");
        return false;
    }
    if (!jsonData.hasOwnProperty("access_token")) {
        Serial.println("Response data JSON does not contain \"access_token\" key");
        return false;
    }

    *token = new char[strlen(jsonData["access_token"]) + 1];
    strcpy(*token, jsonData["access_token"]);

    return true;
}
