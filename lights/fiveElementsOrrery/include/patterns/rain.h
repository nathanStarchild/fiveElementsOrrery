#include <Arduino.h>
#include <FastLED.h>

class Rain : public Pattern {
    private:
        uint16_t dropPosition[30] = {0};
        uint8_t dropSpeed[30] = {0};
        // uint16_t lastStep = mainState.patternStep;
    public:
        Rain(int8_t s, uint8_t l, uint8_t b, bool e) : Pattern(s, l, b, e, false) {}
        void update() {
            length = max((uint8_t) 3, length);
            //  fadeToBlackBy(leds, num_leds, 100);
            for (int nDrop = 0; nDrop < length; nDrop++) {
                if (dropSpeed[nDrop] == 0) {
                    dropSpeed[nDrop] = random8(3, speed);
                }
                if (dropPosition[nDrop] == 0) {
                    dropPosition[nDrop] = random8(stripLength - 11, stripLength - 1);
                }
                // if (mainState.patternStep != lastStep) {
                if (mainState.patternStep % dropSpeed[nDrop] == 0) {
                    dropPosition[nDrop]--;
                }
                // }
                uint16_t pos = dropPosition[nDrop];
                for (int strip = 0; strip < nStrips; strip++) {
                    uint16_t i = nX(strip, pos);
                    leds[i] += ColorFromPalette(currentPalette, nDrop * 15, brightness);
                }
            }
            // lastStep = mainState.patternStep;
        }

        void print() {
            Serial.println("Rain");
        }
};