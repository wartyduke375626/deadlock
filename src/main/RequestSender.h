#include <stdint.h>



namespace RequestSender {
    bool requestToken(const char* endpoint, const char* requestData, char** token);
    bool sendLog(const char* endpoint, const char* token, const char* logData);
    bool requestAccess(const char* endpoint, const char* token, bool* allowed);
};
