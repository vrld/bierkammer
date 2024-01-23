#include "temperature.hpp"

namespace Temperature {

void Temperature::Sensor::setup()
{
  sensors.begin();
}

void Temperature::Sensor::loop()
{
  sensors.requestTemperatures();

  mean = .0f;
  int num_measurements = 0;

  float t = sensors.getTempCByIndex(0);
  if (is_valid_reading(t)) {
    mean += sample[0] = t;
    num_measurements++;
  }

  t = sensors.getTempCByIndex(1);
  if (is_valid_reading(t)) {
    mean += sample[1] = t;
    num_measurements++;
  }

  if (num_measurements > 0) {
    mean /= float(num_measurements);
  } else {
    mean = -127.0f;
  }
}

}
