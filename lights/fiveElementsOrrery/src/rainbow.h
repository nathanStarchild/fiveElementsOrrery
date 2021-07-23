#include <Arduino.h>
// #include <pattern.h>

class Rainbow : public Pattern {
    public:
        void update() {
            for (int i=0; i<NUM_LEDS; i++) {
                leds[i] = ColorFromPalette(RainbowColors_p, mainState.patternStep * speed + i * length, brightness);
            }
        }
};