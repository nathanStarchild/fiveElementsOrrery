#include <Arduino.h>
// #include <pattern.h>

class Rainbow : public Pattern {
    public:
        Rainbow(int8_t s, uint8_t l, uint8_t b, bool e) : Pattern(s, l, b, e, false) {}
        void update() {
            for (int i=0; i<NUM_LEDS; i++) {
                leds[i] = ColorFromPalette(RainbowColors_p, mainState.patternStep * speed + i * length, brightness);
            }
        }

        void print() {
            Serial.println("Rainbow");
        }
};