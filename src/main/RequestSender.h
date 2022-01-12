#include <stdint.h>


class RequestSender {

public:
    RequestSender();
    ~RequestSender();
    bool requestToken(const char* endpoint, const char* requestData, char** token);
    bool sendLog(const char* endpoint, const char* token, const char* ipAddr, const char* msg, const char* data, unsigned int levelNo, const char* level);
    bool requestAccess(const char* endpointBase, const char* token, uint64_t card, unsigned int id, bool* allowed);
};
