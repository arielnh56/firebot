/*  Analog32ma
 *  wraps an analog pin in an exponential moving average
 *  ma data is held in two byte word and uses five of the extra bits for
 *  precision. Final bit removes rounding oddities.
 *  ma calculations use register shifts for speed
 *  class includes scaling and offsets, and the returned value is a float
 *  For best effect, sample 32 times between each read cycle. Or maybe 64.
 */
#ifndef Analog32_h
#define Analog32_h

#include "Arduino.h"

class Analog32ma
{
  public:
    // rendered result is (accumulator - offset) * scale
    // float for offset as accumulator has 6 bits binary precision (1/64ths)
    // initially accumulator will be set to offset, so 0.0 <= offset <= 1023.0
    // these floats are only used during rendering, so should minimally impact speed
    Analog32ma(uint8_t pin, float offset, float scale);
    // read the pin and update the accumulator
    void update();
    float getVal();
    uint16_t _accumulator;
    float _offset;
    float _scale;    
  private:
    uint8_t _pin;
};

#endif
