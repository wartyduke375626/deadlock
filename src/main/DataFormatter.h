#include <stdint.h>



namespace DataFormatter {
    char* getLogData(const char* ipAddr, const char* msg, const char* data, unsigned int levelNo, const char* level);
    char* getAccessEndpoint(const char* endpointBase, uint64_t card, unsigned int id);
};
