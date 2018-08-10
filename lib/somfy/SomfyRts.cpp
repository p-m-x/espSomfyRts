#include "SomfyRts.h"

SomfyRts::SomfyRts(uint8_t tx_pin, bool debug) {
    _debug = debug;
    _tx_pin = tx_pin;
    
    pinMode(_tx_pin, OUTPUT);
    digitalWrite(_tx_pin, LOW);

    if (EEPROM.get(EEPROM_ADDRESS, rollingCode) < newRollingCode) {
        EEPROM.put(EEPROM_ADDRESS, newRollingCode);
    }
    if (Serial) {
        Serial.print("Simulated remote number : "); Serial.println(REMOTE, HEX);
        Serial.print("Current rolling code    : "); Serial.println(rollingCode);
    }
}

SomfyRts::SomfyRts(uint8_t tx_pin) {
    SomfyRts(tx_pin, false);
}

void SomfyRts::buildFrame(unsigned char *frame, unsigned char button) {
    unsigned int code;
    EEPROM.get(EEPROM_ADDRESS, code);
    frame[0] = 0xA7;         // Encryption key. Doesn't matter much
    frame[1] = button << 4;  // Which button did  you press? The 4 LSB will be the checksum
    frame[2] = code >> 8;    // Rolling code (big endian)
    frame[3] = code;         // Rolling code
    frame[4] = REMOTE >> 16; // Remote address
    frame[5] = REMOTE >>  8; // Remote address
    frame[6] = REMOTE;       // Remote address

    if (Serial) {
        Serial.print("Frame         : ");
        for(byte i = 0; i < 7; i++) {
            if(frame[i] >> 4 == 0) { // Displays leading zero in case the most significant
                Serial.print("0");       // nibble is a 0.
            }
                Serial.print(frame[i],HEX); Serial.print(" ");
        }
    }
  
    // Checksum calculation: a XOR of all the nibbles
    checksum = 0;
    for(byte i = 0; i < 7; i++) {
        checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
    }
    checksum &= 0b1111; // We keep the last 4 bits only


    //Checksum integration
    frame[1] |= checksum;   //  If a XOR of all the nibbles is equal to 0, the blinds will
                            // consider the checksum ok.

    if (Serial) {
        Serial.println(""); Serial.print("With checksum : ");
        for(byte i = 0; i < 7; i++) {
            if(frame[i] >> 4 == 0) {
                Serial.print("0");
            }
                Serial.print(frame[i],HEX); Serial.print(" ");
        }
    }

    // Obfuscation: a XOR of all the bytes
    for(byte i = 1; i < 7; i++) {
        frame[i] ^= frame[i-1];
    }

    if (Serial) {
        Serial.println(""); Serial.print("Obfuscated    : ");
        for(byte i = 0; i < 7; i++) {
            if(frame[i] >> 4 == 0) {
                Serial.print("0");
            }
                Serial.print(frame[i],HEX); Serial.print(" ");
        }
        Serial.println("");
        Serial.print("Rolling Code  : "); Serial.println(code);
    }
    
    EEPROM.put(EEPROM_ADDRESS, code + 1);   //  We store the value of the rolling code in the
                                            // EEPROM. It should take up to 2 adresses but the
                                            // Arduino function takes care of it.
}

void SomfyRts::sendCommand(unsigned char *frame, unsigned char sync) {
    if(sync == 2) { // Only with the first frame.
        // Wake-up pulse & Silence
        digitalWrite(_tx_pin, HIGH);
        delayMicroseconds(9415);
        digitalWrite(_tx_pin, LOW);
        delayMicroseconds(89565);
    }

    // Hardware sync: two sync for the first frame, seven for the following ones.
    for (int i = 0; i < sync; i++) {
        digitalWrite(_tx_pin, HIGH);
        delayMicroseconds(4*SYMBOL);
        digitalWrite(_tx_pin, LOW);
        delayMicroseconds(4*SYMBOL);
    }

    // Software sync
    digitalWrite(_tx_pin, HIGH);
    delayMicroseconds(4550);
    digitalWrite(_tx_pin, LOW);
    delayMicroseconds(SYMBOL);
  
  
    //Data: bits are sent one by one, starting with the MSB.
    for(byte i = 0; i < 56; i++) {
        if(((frame[i/8] >> (7 - (i%8))) & 1) == 1) {
            digitalWrite(_tx_pin, LOW);
            delayMicroseconds(SYMBOL);
            // PORTD ^= 1<<5;
            digitalWrite(_tx_pin, HIGH);
            delayMicroseconds(SYMBOL);
        }
        else {
            digitalWrite(_tx_pin, HIGH);
            delayMicroseconds(SYMBOL);
            // PORTD ^= 1<<5;
            digitalWrite(_tx_pin, LOW);
            delayMicroseconds(SYMBOL);
        }
    }
    
    digitalWrite(_tx_pin, LOW);
    delayMicroseconds(30415); // Inter-frame silence
}

