#include "RequestSender.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>
#include "constants.h"



RequestSender::RequestSender() {}


RequestSender::~RequestSender() {}



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
        Serial.print("Failed to acquire token: "); Serial.println(https.getString());
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

    Serial.print("Token acquired: "); Serial.println(*token);
    Serial.println();
    return true;
}



char* constructLogData(const char* ipAddr, const char* msg, const char* data, unsigned int levelNo, const char* level) {
    char levelNoStr[3*sizeof(unsigned int)];
    sprintf(levelNoStr, "%d", levelNo);

    JSONVar json;
    json["ip_addr"] = ipAddr;
    json["msg"] = msg;
    json["data"] = data;
    json["levelno"] = levelNoStr;
    json["level"] = level;

    String jsonString = JSON.stringify(json);

    char* ret = new char[jsonString.length() + 1];
    jsonString.toCharArray(ret, jsonString.length() + 1);

    return ret;
}



char* constructTokenData(const char* token) {
    char* ret = new char[strlen("Bearer ") + strlen(token) + 1];
    strcpy(ret, "Bearer ");
    strcat(ret, token);

    return ret;
}



bool RequestSender::sendLog(const char* endpoint, const char* token, const char* ipAddr, const char* msg, const char* data, unsigned int levelNo, const char* level) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    char* logData = constructLogData(ipAddr, msg, data, levelNo, level);
    Serial.print("Sending log: "); Serial.println(logData);

    https.begin(client, endpoint);

    char* tokenData = constructTokenData(token);
    
    https.addHeader("Authorization", tokenData);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Content-Length", String(strlen(logData)));

    int responseCode = https.POST(logData);
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.print("Failed to send log: "); Serial.println(https.getString());
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());

    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data");
        return false;
    }
    if (!jsonData.hasOwnProperty("success")) {
        Serial.println("Response data JSON does not contain \"success\" key");
        return false;
    }
    if (!(bool) jsonData["success"]) {
        Serial.println("Response data JSON \"success\" key value is not equal to \"true\"");
        return false;
    }

    delete[] logData;
    delete[] tokenData;
    
    Serial.println("Log sent successfully");
    Serial.println();
    return true;
}



char* constructAccessEndpoint(const char* endpointBase, uint64_t card, unsigned int id) {
    char cardStr[3*sizeof(uint64_t)];
    sprintf(cardStr, "%lld", card);

    char idStr[3*sizeof(unsigned int)];
    sprintf(idStr, "%d", id);
    
    char* ret = new char[strlen(endpointBase) + strlen("?card=") + strlen(cardStr) + strlen("&ap_id=") + strlen(idStr) + 1];
    strcpy(ret, endpointBase);
    strcat(ret, "?card=");
    strcat(ret, cardStr);
    strcat(ret, "&ap_id=");
    strcat(ret, idStr);

    return ret;
}



bool RequestSender::requestAccess(const char* endpointBase, const char* token, uint64_t card, unsigned int id, bool* allowed){
    HTTPClient https;
    WiFiClientSecure client;
    client.setInsecure();

    char* accessEdpoint = constructAccessEndpoint(endpointBase, card, id);
    Serial.print("Sending access request to endpoint: "); Serial.println(accessEdpoint);
    
    https.begin(client, accessEdpoint);

    char* tokenData = constructTokenData(token);

    https.addHeader("Authorization", tokenData);

    int responseCode = https.POST("");
    Serial.print("HTTP Response code: "); Serial.println(responseCode);

    if (responseCode != 200) {
        Serial.print("Failed to send log: "); Serial.println(https.getString());
        return false;
    }

    JSONVar jsonData = JSON.parse(https.getString());

    if (JSON.typeof(jsonData) == "undefined") {
        Serial.println("Failed to parse JSON from response data");
        return false;
    }
    if (!jsonData.hasOwnProperty("allow")) {
        Serial.println("Response data JSON does not contain \"allow\" key");
        return false;
    }

    *allowed = (bool) jsonData["allow"];

    delete[] accessEdpoint;
    delete[] tokenData;
    
    Serial.println("Access request sent successfully");
    Serial.println();
    return true;
}
