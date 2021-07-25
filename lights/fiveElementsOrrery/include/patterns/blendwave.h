#include <Arduino.h>
#include <FastLED.h>
#include "patterns/pattern.h"

class Blendwave : public Pattern {
    public:
        Blendwave(int8_t s, uint8_t l, uint8_t b, bool e) : Pattern(s, l, b, e, false) {}
        void update() {
            uint8_t blendSpeed = sin8(4 * mainState.patternStep / 6);
            CRGB clr1 = blend(ColorFromPalette(currentPalette, sin8(2 * mainState.patternStep / 6), 255), ColorFromPalette(currentPalette, sin8(3 * mainState.patternStep / 6), 255), blendSpeed); 
            CRGB clr2 = blend(ColorFromPalette(currentPalette, sin8(3 * mainState.patternStep / 6), 255), ColorFromPalette(currentPalette, sin8(2 * mainState.patternStep / 6), 255), blendSpeed);
            uint8_t loc1 = map(sin8(6 * mainState.patternStep / 6), 0, 255, 0, num_leds-1);
            fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
            fill_gradient_RGB(leds, loc1, clr2, num_leds - 1, clr1);
        }
        void print(){
            Serial.println("Blendwave");
        }
};