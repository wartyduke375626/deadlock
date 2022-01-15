#include "DataFormatter.h"
#include <Arduino_JSON.h>



char* DataFormatter::getLogData(const IPAddress &ip, const char* msg, const char* data, unsigned int levelNo, const char* level) {
    char ipStr[16];
    sprintf(ipStr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    
    char levelNoStr[3*sizeof(unsigned int)];
    sprintf(levelNoStr, "%d", levelNo);

    JSONVar json;
    json["ip_addr"] = ipStr;
    json["msg"] = msg;
    json["data"] = data;
    json["levelno"] = levelNoStr;
    json["level"] = level;

    String jsonString = JSON.stringify(json);

    char* ret = new char[jsonString.length() + 1];
    jsonString.toCharArray(ret, jsonString.length() + 1);

    return ret;
}


char* DataFormatter::getAccessEndpoint(const char* endpointBase, uint64_t card, unsigned int id) {
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
