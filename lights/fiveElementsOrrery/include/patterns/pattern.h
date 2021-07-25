#ifndef PATTERN_DEFINED
#define PATTERN_DEFINED
#include <Arduino.h>

class Pattern {
    public:
        int8_t speed;
        uint8_t length;
        uint8_t brightness;
        bool enabled;
        bool isDark;

        Pattern(int8_t s, uint8_t l, uint8_t b, bool e, bool d) {
            speed = s;
            length = l;
            brightness = b;
            enabled = e;
            isDark = d;
        };

        virtual void update(){};
        virtual void print() {
            Serial.println("Pattern Base Class");
        };

};
#endif