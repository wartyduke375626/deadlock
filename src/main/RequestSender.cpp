#include "RequestSender.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>



bool RequestSender::requestToken(const char* endpoint, const char* requestData, char** token) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    Serial.println("Sending token request...");
    
    https.begin(client, endpoint);

    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    https.addHeader("Content-Length", String(strlen(requestData)));

    int responseCode = https.POST(requestData);
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.println("Failed to acquire token: " + https.getString() + "\n");
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());
    
    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data.\n");
        return false;
    }
    if (!jsonData.hasOwnProperty("access_token")) {
        Serial.println("Response data JSON does not contain \"access_token\" key.\n");
        return false;
    }

    *token = new char[strlen(jsonData["access_token"]) + 1];
    strcpy(*token, jsonData["access_token"]);

    Serial.print("Token acquired: "); Serial.println(*token); Serial.println();
    return true;
}


char* getTokenData(const char* token) {
    char* ret = new char[strlen("Bearer ") + strlen(token) + 1];
    strcpy(ret, "Bearer ");
    strcat(ret, token);

    return ret;
}


bool RequestSender::sendLog(const char* endpoint, const char* token, const char* logData) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    char* tokenData = getTokenData(token);

    Serial.print("Sending log: "); Serial.println(logData);

    https.begin(client, endpoint);
    
    https.addHeader("Authorization", tokenData);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Content-Length", String(strlen(logData)));

    int responseCode = https.POST(logData);
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.println("Failed to send log: " + https.getString() + "\n");
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());

    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data.\n");
        return false;
    }
    if (!jsonData.hasOwnProperty("success")) {
        Serial.println("Response data JSON does not contain \"success\" key.\n");
        return false;
    }
    if (JSON.typeof(jsonData["success"]) != "boolean") {
        Serial.println("Response data JSON \"success\" is not of type boolean.\n");
        return false;
    }
    if (!(bool) jsonData["success"]) {
        Serial.println("Response data JSON \"success\" key value is not equal to \"true\".\n");
        return false;
    }

    delete[] tokenData;
    
    Serial.println("Log sent successfully.\n");
    return true;
}


bool RequestSender::requestAccess(const char* endpoint, const char* token, bool* allowed) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    char* tokenData = getTokenData(token);

    Serial.print("Sending access request to endpoint: "); Serial.println(endpoint);
    
    https.begin(client, endpoint);

    https.addHeader("Authorization", tokenData);

    int responseCode = https.POST("");
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.println("Failed to send log: " + https.getString() + "\n");
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());

    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data.\n");
        return false;
    }
    if (!jsonData.hasOwnProperty("allow")) {
        Serial.println("Response data JSON does not contain \"allow\" key.\n");
        return false;
    }
    if (JSON.typeof(jsonData["allow"]) != "boolean") {
        Serial.println("Response data JSON \"allow\" is not of type boolean.\n");
        return false;
    }

    *allowed = (bool) jsonData["allow"];

    delete[] tokenData;
    
    Serial.println("Access request sent successfully.\n");
    return true;
}
