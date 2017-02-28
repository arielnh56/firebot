#include "Arduino.h"
#include "Analog32ma.h"

Analog32ma::Analog32ma(uint8_t pin, float offset, float scale) {
  _pin = pin;
  // accumulator is left shifted 6 bits
  // update values left shift 1 bit
  // this one bit loss eliminates some odd accumulator behavior
  _accumulator = (uint16_t)(constrain(offset, 0.0, 1023.0) * 64.0);
  _offset = offset;
  _scale = scale;    
}


void Analog32ma::update() {
  // this is the standard exponential moving average calc moved around a bit
  // the stored accumulator value is left shifted 6 bits so we have higher resolution
  // sample is left shifted one bit
  // we will right shift 6 bits on read
  // without the lost bit on the end, an oscillation between 0 and 1 will grow the accumulator
  // until it flips, however infrequent the 1s are. That extra bit of precision allows the zeroes
  // to pull the accumulator back down before it becomes visible.
  _accumulator += (analogRead(_pin) << 1) - (_accumulator >> 5);  
}

float Analog32ma::getVal() {
  return ((_accumulator / 64.0) - _offset) * _scale;
}



