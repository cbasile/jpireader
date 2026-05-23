#include "sensor_parser.h"

namespace jpireader {

SensorParser::SensorParser(int low, int high) : mask_(4) {
  mask_.SetWord(0, low);
  mask_.SetWord(2, high);
}

Sensors SensorParser::Parse() {
  Sensors sensors;

  if (mask_.TestBit(0)) {
    sensors.voltage = true;
  }

  int num_egt = mask_.CountBits(11, 19);
  if (num_egt > 0) {
    sensors.num_exhaust_gas_temperature = num_egt;
  }

  int num_cht = mask_.CountBits(2, 10);
  if (num_cht > 0) {
    sensors.num_cylinder_head_temperature = num_cht;
  }

  for (int i = 20; i < 28; ++i) {
    if (mask_.TestBit(i)) {
      switch (i) {
        case 20: sensors.oil_temperature = true; break;
        case 21: sensors.turbine_inlet_temperature1 = true; break;
        case 22: sensors.turbine_inlet_temperature2 = true; break;
        case 23: sensors.compressor_discharge_temperature = true; break;
        case 24: sensors.induction_air_temperature = true; break;
        case 25: sensors.outside_air_temperature = true; break;
        case 26: sensors.rpm = true; break;
        case 27: sensors.fuel_flow = true; break;
      }
    }
  }

  if (mask_.TestBit(30)) {
    sensors.manifold_pressure = true;
  }

  return sensors;
}

}  // namespace jpireader
