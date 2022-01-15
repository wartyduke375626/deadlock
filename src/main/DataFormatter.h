#include <ESP8266WiFi.h>
#include <stdint.h>



namespace DataFormatter {
    char* getLogData(const IPAddress &ip, const char* msg, const char* data, unsigned int levelNo, const char* level);
    char* getAccessEndpoint(const char* endpointBase, uint64_t card, unsigned int id);
};
