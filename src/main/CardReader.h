#include "PN532.h"



class CardReader {
    
private:
    PN532* nfc;
    uint16_t timeout;
    uint64_t convertToDecimal(const uint8_t* uid, uint8_t uidLength);
    
public:
    CardReader(PN532* nfc, uint16_t timeout);
    ~CardReader();
    bool begin();
    bool readCard(uint64_t* card);
};
