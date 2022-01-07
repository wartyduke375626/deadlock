#include "PN532.h"



class CardReader {
    
private:
    PN532* nfc;
    uint16_t timeout;
    
public:
    CardReader(PN532* nfc, uint16_t timeout);
    ~CardReader();
    bool begin();
    bool readCard(uint64_t* snr);
};
