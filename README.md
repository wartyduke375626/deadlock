# ESP8266 ISO14443A card reader

This project uses an ESP8266 NodeMCU board and a PN532 NFC module. The board reads an ISO14443A cards and communicates with a backend server, which authorizes or denies access according to the UID value contained on the card chip.

**The communication with the server is incomplete and TBA.**

## Required hardware:
- ESP8266 NodeMCU WiFi Development Board
   - user manual: https://handsontec.com/dataspecs/module/esp8266-V13.pdf
- PN532 NFC RFID Module
  - Amazon: https://www.amazon.com/HiLetgo-Communication-Arduino-Raspberry-Android/dp/B01I1J17LC
  - user manual: http://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf

## Required software:
- Arduino IDE: https://www.arduino.cc/en/software
- PN532 library: https://github.com/elechouse/PN532
- WiFiManager library: https://github.com/tzapu/WiFiManager (also downloadable within Arduino IDE)
- Arduino_JSON library: https://github.com/arduino-libraries/Arduino_JSON (also downloadable within Arduino IDE)
- ESP8266 board installed in Arduino IDE

## Setup:
1. Make sure the switch on the NFC module is set to use the SPI interfece:
   
| Channel 1 | Channel 2 |
|:---------:|:---------:|
| OFF       | ON        |

2. Connect these pins on the NFC module SPI interface to the board as following:

| PN532 Module | ESP8266 Board |
|:------------:|:-------------:|
| GND          | G             |
| VCC          | 3V            |
| SCK          | D5            |
| MISO         | D6            |
| MOSI         | D7            |
| SS           | D0            |

3. Install and setup the ESP8266 board in Arduino IDE - more information can be found in the board user manual: https://handsontec.com/dataspecs/module/esp8266-V13.pdf.
4. Download and include all the required libraries in the Arduino IDE sketch.
5. Flash.
6. After first startup, you will need to save your WiFi SSID and password to the board's EEPROM through the board's access point.

## How to set WiFi after first startup
During setup the board will attempt to connect to WiFi using the SSID and password saved in the board's EEPROM. If they are not present there or the connection process fails, the board will switch to WiFi setup mode, turn on it's access point and wait.

1. Connect to the board's access point with your device. Defaults are: SSID - 'ESP2866_AP', password - 'D3@d1ock'. These values can be changed in 'constants.h'.
2. In your browser go to '192.168.4.1'.
3. Select 'Configure WiFi' and insert your WiFi SSID and password, then select 'Save'.
4. Once a successful connection is established, the access point will be shut down and the card reader program will start. Upon failure it will stay in WiFi setup mode and you will need to repeat this process.

Once successful, the board will save the WiFi credentials in it's EEPROM and will automatically try to connect on next startup. Upon failure it will go back to WiFi setup mode and you need to repeat steps 1-4. More information can be found in the WiFiManager library documentation: https://github.com/tzapu/WiFiManager.

If WiFi gets disconnected after setup, the program will try to reconnect using the SSID and password used in the setup process.
