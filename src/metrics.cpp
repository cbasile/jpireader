#include "metrics.h"

namespace jpireader {

int Metrics::GetVersionSelector(const MetadataUtil& util) {
  if (util.IsModelNumber(760)) return V2;
  if (util.IsModelNumber(960)) return V5;
  if (util.IsModelNumberAtLeast(900)) {
    return util.IsFirmwareVersionAtLeast(108) ? V4 : V3;
  } else {
    return util.HasProtocolHeader() ? V4 : V1;
  }
}

std::map<int, const Metric*> Metrics::GetBitToMetricMap(const MetadataUtil& util) {
  int selector = GetVersionSelector(util);
  std::map<int, const Metric*> result;
  for (const auto& metric : METRICS) {
    if ((metric.version_mask & selector) > 0) {
      result[metric.low_byte_bit] = &metric;
      if (metric.high_byte_bit) {
        result[*metric.high_byte_bit] = &metric;
      }
    }
  }
  return result;
}

const std::vector<Metric> Metrics::METRICS = {
    Metric(V1|V2|V3|V4|V5,   0,  48, "engine[0].exhaust_gas_temperature[0]"),
    Metric(V1|V2|V3|V4|V5,   1,  49, "engine[0].exhaust_gas_temperature[1]"),
    Metric(V1|V2|V3|V4|V5,   2,  50, "engine[0].exhaust_gas_temperature[2]"),
    Metric(V1|V2|V3|V4|V5,   3,  51, "engine[0].exhaust_gas_temperature[3]"),
    Metric(V1|V2|V3|V4|V5,   4,  52, "engine[0].exhaust_gas_temperature[4]"),
    Metric(V1|V2|V3|V4|V5,   5,  53, "engine[0].exhaust_gas_temperature[5]"),
    Metric(V1|V2|V3|V4|V5,   6,  54, "engine[0].turbine_inlet_temperature[0]"),
    Metric(V1|V2|V3|V4|V5,   7,  55, "engine[0].turbine_inlet_temperature[1]"),

    Metric(V1|V2|V3|V4|V5,   8,      "engine[0].cylinder_head_temperature[0]"),
    Metric(V1|V2|V3|V4|V5,   9,      "engine[0].cylinder_head_temperature[1]"),
    Metric(V1|V2|V3|V4|V5,  10,      "engine[0].cylinder_head_temperature[2]"),
    Metric(V1|V2|V3|V4|V5,  11,      "engine[0].cylinder_head_temperature[3]"),
    Metric(V1|V2|V3|V4|V5,  12,      "engine[0].cylinder_head_temperature[4]"),
    Metric(V1|V2|V3|V4|V5,  13,      "engine[0].cylinder_head_temperature[5]"),
    Metric(V1|V2|V3|V4|V5,  14,      "engine[0].cylinder_head_temperature_cooling_rate"),
    Metric(V1|V2|V3|V4|V5,  15,      "engine[0].oil_temperature"),

    Metric(V1|V2|V3|V4|V5,  16,      "mark"),
    Metric(V1   |V3|V4|V5,  17,      "engine[0].oil_pressure"),
    Metric(V1|V2|V3|V4|V5,  18,      "engine[0].compressor_discharge_temperature"),
    Metric(V1   |V3|V4|V5,  19,      "engine[0].induction_air_temperature"),
    Metric(   V2         ,  19,      "engine[1].manifold_pressure", ScaleFactor::TEN),
    Metric(V1|V2|V3|V4|V5,  20,      "voltage[0]", ScaleFactor::TEN),
    Metric(V1|V2|V3|V4|V5,  21,      "outside_air_temperature"),
    Metric(V1|V2|V3|V4|V5,  22,      "engine[0].fuel_used[0]", ScaleFactor::TEN_IF_GPH),
    Metric(V1|V2|V3|V4|V5,  23,      "engine[0].fuel_flow[0]", ScaleFactor::TEN_IF_GPH),

    Metric(V1   |V3|V4   ,  24,  56, "engine[0].exhaust_gas_temperature[6]"),
    Metric(   V2      |V5,  24,  56, "engine[1].exhaust_gas_temperature[0]"),
    Metric(V1   |V3|V4   ,  25,  57, "engine[0].exhaust_gas_temperature[7]"),
    Metric(   V2      |V5,  25,  57, "engine[1].exhaust_gas_temperature[1]"),
    Metric(V1   |V3|V4   ,  26,  58, "engine[0].exhaust_gas_temperature[8]"),
    Metric(   V2      |V5,  26,  58, "engine[1].exhaust_gas_temperature[2]"),
    Metric(V1   |V3|V4   ,  27,      "engine[0].cylinder_head_temperature[6]"),
    Metric(   V2      |V5,  27,  59, "engine[1].exhaust_gas_temperature[3]"),
    Metric(V1   |V3|V4   ,  28,      "engine[0].cylinder_head_temperature[7]"),
    Metric(   V2      |V5,  28,  60, "engine[1].exhaust_gas_temperature[4]"),
    Metric(V1   |V3|V4   ,  29,      "engine[0].cylinder_head_temperature[8]"),
    Metric(   V2      |V5,  29,  61, "engine[1].exhaust_gas_temperature[5]"),
    Metric(V1   |V3|V4   ,  30,      "engine[0].horsepower"),
    Metric(   V2      |V5,  30,  62, "engine[1].turbine_inlet_temperature[0]"),
    Metric(   V2      |V5,  31,  63, "engine[1].turbine_inlet_temperature[1]"),

    Metric(   V2      |V5,  32,      "engine[1].cylinder_head_temperature[0]"),
    Metric(   V2      |V5,  33,      "engine[1].cylinder_head_temperature[1]"),
    Metric(   V2      |V5,  34,      "engine[1].cylinder_head_temperature[2]"),
    Metric(   V2      |V5,  35,      "engine[1].cylinder_head_temperature[3]"),
    Metric(   V2      |V5,  36,      "engine[1].cylinder_head_temperature[4]"),
    Metric(   V2      |V5,  37,      "engine[1].cylinder_head_temperature[5]"),
    Metric(   V2      |V5,  38,      "engine[1].cylinder_head_temperature_cooling_rate"),
    Metric(   V2      |V5,  39,      "engine[1].oil_temperature"),

    Metric(V1|V2|V3|V4|V5,  40,      "engine[0].manifold_pressure", ScaleFactor::TEN),
    Metric(V1|V2|V3|V4|V5,  41,  42, "engine[0].rpm"),
    Metric(   V2      |V5,  43,  44, "engine[1].rpm"),
    Metric(         V4   ,  44,      "engine[0].hydraulic_pressure[1]"),
    Metric(   V2      |V5,  45,      "engine[1].compressor_discharge_temperature"),
    Metric(         V4   ,  45,      "engine[0].hydraulic_pressure[0]"),
    Metric(   V2      |V5,  46,      "engine[1].fuel_used[0]", ScaleFactor::TEN_IF_GPH),
    Metric(         V4   ,  46,      "engine[0].fuel_flow[1]", ScaleFactor::TEN_IF_GPH),
    Metric(         V4   ,  47,      "engine[0].fuel_used[1]", ScaleFactor::TEN_IF_GPH),
    Metric(   V2      |V5,  47,      "engine[1].fuel_flow[0]", ScaleFactor::TEN_IF_GPH),

    Metric(      V3|V4|V5,  64,      "amperage[0]"),
    Metric(      V3|V4|V5,  65,      "voltage[1]", ScaleFactor::TEN),
    Metric(      V3|V4|V5,  66,      "amperage[1]"),
    Metric(      V3|V4   ,  67,      "engine[1].fuel_level[0]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5,  67,      "engine[0].fuel_level[0]", ScaleFactor::TEN_IF_GPH),
    Metric(      V3|V4   ,  68,      "engine[0].fuel_level[0]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5,  68,      "engine[0].fuel_level[1]", ScaleFactor::TEN_IF_GPH),
    Metric(      V3|V4|V5,  69,      "engine[0].fuel_pressure", ScaleFactor::TEN),
    Metric(            V5,  70,      "engine[0].horsepower"),
    Metric(         V4   ,  71,      "", ScaleFactor::TEN_IF_GPH),
    Metric(            V5,  71,      "engine[0].fuel_level[2]", ScaleFactor::TEN_IF_GPH),

    Metric(         V4|V5,  72,  76, "", ScaleFactor::TEN),
    Metric(         V4|V5,  73,  77, ""),
    Metric(         V4|V5,  74,      "engine[0].torque"),
    Metric(         V4|V5,  75,      ""),
    Metric(         V4|V5,  78,  79, "engine[0].hours", ScaleFactor::TEN),

    Metric(         V4   ,  84,      "", ScaleFactor::TEN_IF_GPH),

    Metric(            V5,  88,      "engine[1].manifold_pressure", ScaleFactor::TEN),
    Metric(            V5,  89,      "engine[1].horsepower"),
    Metric(            V5,  90,      "engine[1].induction_air_temperature"),
    Metric(            V5,  91,      "engine[1].fuel_level[0]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5,  92,      "engine[1].fuel_level[1]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5,  93,      "engine[1].fuel_pressure", ScaleFactor::TEN),
    Metric(            V5,  94,      "engine[1].oil_pressure", ScaleFactor::TEN),
    Metric(            V5,  95,      "engine[1].fuel_level[2]", ScaleFactor::TEN_IF_GPH),

    Metric(            V5,  96, 100, "", ScaleFactor::TEN),
    Metric(            V5,  97, 101, ""),
    Metric(            V5,  98,      "engine[1].torque"),
    Metric(            V5,  99,      ""),
    Metric(            V5, 102, 103, "engine[1].hours", ScaleFactor::TEN),

    Metric(            V5, 104, 108, "engine[0].exhaust_gas_temperature[6]"),
    Metric(            V5, 105, 109, "engine[0].exhaust_gas_temperature[7]"),
    Metric(            V5, 106, 110, "engine[0].exhaust_gas_temperature[8]"),
    Metric(            V5, 107,      "engine[1].fuel_flow[1]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5, 111,      "engine[0].hydraulic_pressure[0]"),

    Metric(            V5, 112, 116, "engine[1].exhaust_gas_temperature[6]"),
    Metric(            V5, 113, 117, "engine[1].exhaust_gas_temperature[7]"),
    Metric(            V5, 114, 118, "engine[1].exhaust_gas_temperature[8]"),
    Metric(            V5, 115,      "engine[1].fuel_flow[1]", ScaleFactor::TEN_IF_GPH),
    Metric(            V5, 119,      "engine[1].hydraulic_pressure[0]"),

    Metric(            V5, 120,      "engine[0].cylinder_head_temperature[6]"),
    Metric(            V5, 121,      "engine[0].cylinder_head_temperature[7]"),
    Metric(            V5, 122,      "engine[0].cylinder_head_temperature[8]"),
    Metric(            V5, 123,      "engine[0].hydraulic_pressure[1]"),
    Metric(            V5, 124,      "engine[1].cylinder_head_temperature[6]"),
    Metric(            V5, 125,      "engine[1].cylinder_head_temperature[7]"),
    Metric(            V5, 126,      "engine[1].cylinder_head_temperature[8]"),
    Metric(            V5, 127,      "engine[1].hydraulic_pressure[1]"),
};

}  // namespace jpireader
