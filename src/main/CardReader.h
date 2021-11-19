#include "PN532.h"



class CardReader {
    
private:
    PN532 *nfc;
    
public:
    CardReader();
    CardReader(PN532* nfc);
    ~CardReader();
    bool begin();
    bool readCard(uint64_t *snr);
};
