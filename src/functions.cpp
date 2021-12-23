#include <ESP8266WiFi.h>



void wifiConnect(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    
    Serial.print("Connected to WiFi network with IP Address: "); Serial.println(WiFi.localIP());
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
}
