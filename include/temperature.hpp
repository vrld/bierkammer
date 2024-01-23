#pragma once

#include <DallasTemperature.h>
#include <OneWire.h>

namespace Temperature
{
struct Sensor
{
  OneWire oneWire;
  DallasTemperature sensors;
  float sample[2] = {.0f, .0f};
  float mean = .0f;

  Sensor(uint8_t port)
    : oneWire(port)
    , sensors(&oneWire)
  {}

  static constexpr bool is_valid_reading(float t)
  {
    return (t != -127.0f);
  }

  void setup();
  void loop();

};
}
