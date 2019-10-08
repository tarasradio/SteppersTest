#include "sensors.hpp"

int16_t Sensors::getSensorValue(int sensorNumber)
{
  return analogRead(A0 + sensorNumber);
}