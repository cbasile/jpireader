#include "metrics.h"

namespace jpireader {

int Metrics::GetVersionSelector(const MetadataUtil& util) {
  if (util.IsModelNumber(760) || util.IsModelNumber(790)) return V2;
  if (util.IsModelNumber(960)) return V5;
  if (util.IsModelNumberAtLeast(900)) {
    return util.IsFirmwareVersionAtLeast(108) ? V4 : V3;
  } else {
    return util.HasProtocolHeader() ? V4 : V1;
  }
}

std::map<int, const Metric*> Metrics::GetBitToMetricMap(
    const MetadataUtil& util) {
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
    Metric(V1 | V2 | V3 | V4 | V5, 0, 48, MetricId::kExhaustGasTemperature, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 1, 49, MetricId::kExhaustGasTemperature, 0, 1),
    Metric(V1 | V2 | V3 | V4 | V5, 2, 50, MetricId::kExhaustGasTemperature, 0, 2),
    Metric(V1 | V2 | V3 | V4 | V5, 3, 51, MetricId::kExhaustGasTemperature, 0, 3),
    Metric(V1 | V2 | V3 | V4 | V5, 4, 52, MetricId::kExhaustGasTemperature, 0, 4),
    Metric(V1 | V2 | V3 | V4 | V5, 5, 53, MetricId::kExhaustGasTemperature, 0, 5),
    Metric(V1 | V2 | V3 | V4 | V5, 6, 54, MetricId::kTurbineInletTemperature, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 7, 55, MetricId::kTurbineInletTemperature, 0, 1),

    Metric(V1 | V2 | V3 | V4 | V5, 8, MetricId::kCylinderHeadTemperature, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 9, MetricId::kCylinderHeadTemperature, 0, 1),
    Metric(V1 | V2 | V3 | V4 | V5, 10, MetricId::kCylinderHeadTemperature, 0, 2),
    Metric(V1 | V2 | V3 | V4 | V5, 11, MetricId::kCylinderHeadTemperature, 0, 3),
    Metric(V1 | V2 | V3 | V4 | V5, 12, MetricId::kCylinderHeadTemperature, 0, 4),
    Metric(V1 | V2 | V3 | V4 | V5, 13, MetricId::kCylinderHeadTemperature, 0, 5),
    Metric(V1 | V2 | V3 | V4 | V5, 14, MetricId::kCylinderHeadTemperatureCoolingRate, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 15, MetricId::kOilTemperature, 0, 0),

    Metric(V1 | V2 | V3 | V4 | V5, 16, MetricId::kMark),
    Metric(V1 | V3 | V4 | V5, 17, MetricId::kOilPressure, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 18, MetricId::kCompressorDischargeTemperature, 0, 0),
    Metric(V1 | V3 | V4 | V5, 19, MetricId::kInductionAirTemperature, 0, 0),
    Metric(V2, 19, MetricId::kManifoldPressure, ScaleFactor::TEN, 1, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 20, MetricId::kVoltage, ScaleFactor::TEN, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 21, MetricId::kOutsideAirTemperature),
    Metric(V1 | V2 | V3 | V4 | V5, 22, MetricId::kFuelUsed, ScaleFactor::TEN_IF_GPH, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 23, MetricId::kFuelFlow, ScaleFactor::TEN_IF_GPH, 0, 0),

    Metric(V1 | V3 | V4, 24, 56, MetricId::kExhaustGasTemperature, 0, 6),
    Metric(V2 | V5, 24, 56, MetricId::kExhaustGasTemperature, 1, 0),
    Metric(V1 | V3 | V4, 25, 57, MetricId::kExhaustGasTemperature, 0, 7),
    Metric(V2 | V5, 25, 57, MetricId::kExhaustGasTemperature, 1, 1),
    Metric(V1 | V3 | V4, 26, 58, MetricId::kExhaustGasTemperature, 0, 8),
    Metric(V2 | V5, 26, 58, MetricId::kExhaustGasTemperature, 1, 2),
    Metric(V1 | V3 | V4, 27, MetricId::kCylinderHeadTemperature, 0, 6),
    Metric(V2 | V5, 27, 59, MetricId::kExhaustGasTemperature, 1, 3),
    Metric(V1 | V3 | V4, 28, MetricId::kCylinderHeadTemperature, 0, 7),
    Metric(V2 | V5, 28, 60, MetricId::kExhaustGasTemperature, 1, 4),
    Metric(V1 | V3 | V4, 29, MetricId::kCylinderHeadTemperature, 0, 8),
    Metric(V2 | V5, 29, 61, MetricId::kExhaustGasTemperature, 1, 5),
    Metric(V1 | V3 | V4, 30, MetricId::kHorsepower, 0, 0),
    Metric(V2 | V5, 30, 62, MetricId::kTurbineInletTemperature, 1, 0),
    Metric(V2 | V5, 31, 63, MetricId::kTurbineInletTemperature, 1, 1),

    Metric(V2 | V5, 32, MetricId::kCylinderHeadTemperature, 1, 0),
    Metric(V2 | V5, 33, MetricId::kCylinderHeadTemperature, 1, 1),
    Metric(V2 | V5, 34, MetricId::kCylinderHeadTemperature, 1, 2),
    Metric(V2 | V5, 35, MetricId::kCylinderHeadTemperature, 1, 3),
    Metric(V2 | V5, 36, MetricId::kCylinderHeadTemperature, 1, 4),
    Metric(V2 | V5, 37, MetricId::kCylinderHeadTemperature, 1, 5),
    Metric(V2 | V5, 38, MetricId::kCylinderHeadTemperatureCoolingRate, 1, 0),
    Metric(V2 | V5, 39, MetricId::kOilTemperature, 1, 0),

    Metric(V1 | V2 | V3 | V4 | V5, 40, MetricId::kManifoldPressure, ScaleFactor::TEN, 0, 0),
    Metric(V1 | V2 | V3 | V4 | V5, 41, 42, MetricId::kRpm, 0, 0),
    Metric(V2 | V5, 43, 44, MetricId::kRpm, 1, 0),
    Metric(V4, 44, MetricId::kHydraulicPressure, 0, 1),
    Metric(V2 | V5, 45, MetricId::kCompressorDischargeTemperature, 1, 0),
    Metric(V4, 45, MetricId::kHydraulicPressure, 0, 0),
    Metric(V2 | V5, 46, MetricId::kFuelUsed, ScaleFactor::TEN_IF_GPH, 1, 0),
    Metric(V4, 46, MetricId::kFuelFlow, ScaleFactor::TEN_IF_GPH, 0, 1),
    Metric(V4, 47, MetricId::kFuelUsed, ScaleFactor::TEN_IF_GPH, 0, 1),
    Metric(V2 | V5, 47, MetricId::kFuelFlow, ScaleFactor::TEN_IF_GPH, 1, 0),

    Metric(V3 | V4 | V5, 64, MetricId::kAmperage, 0, 0),
    Metric(V3 | V4 | V5, 65, MetricId::kVoltage, ScaleFactor::TEN, 0, 1),
    Metric(V3 | V4 | V5, 66, MetricId::kAmperage, 0, 1),
    Metric(V3 | V4, 67, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 1, 0),
    Metric(V5, 67, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 0, 0),
    Metric(V3 | V4, 68, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 0, 0),
    Metric(V5, 68, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 0, 1),
    Metric(V3 | V4 | V5, 69, MetricId::kFuelPressure, ScaleFactor::TEN, 0, 0),
    Metric(V5, 70, MetricId::kHorsepower, 0, 0),
    Metric(V4, 71, MetricId::kNone, ScaleFactor::TEN_IF_GPH, 0, 0),
    Metric(V5, 71, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 0, 2),

    Metric(V4 | V5, 72, 76, MetricId::kNone, ScaleFactor::TEN, 0, 0),
    Metric(V4 | V5, 73, 77, MetricId::kNone, 0, 0),
    Metric(V4 | V5, 74, MetricId::kTorque, 0, 0),
    Metric(V4 | V5, 75, MetricId::kNone, 0, 0),
    Metric(V2 | V4 | V5, 78, 79, MetricId::kHours, ScaleFactor::TEN, 0, 0),

    Metric(V4, 84, MetricId::kNone, ScaleFactor::TEN_IF_GPH, 0, 0),

    Metric(V5, 88, MetricId::kManifoldPressure, ScaleFactor::TEN, 1, 0),
    Metric(V5, 89, MetricId::kHorsepower, 1, 0),
    Metric(V5, 90, MetricId::kInductionAirTemperature, 1, 0),
    Metric(V5, 91, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 1, 0),
    Metric(V5, 92, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 1, 1),
    Metric(V5, 93, MetricId::kFuelPressure, ScaleFactor::TEN, 1, 0),
    Metric(V5, 94, MetricId::kOilPressure, ScaleFactor::TEN, 1, 0),
    Metric(V5, 95, MetricId::kFuelLevel, ScaleFactor::TEN_IF_GPH, 1, 2),

    Metric(V5, 96, 100, MetricId::kNone, ScaleFactor::TEN, 0, 0),
    Metric(V5, 97, 101, MetricId::kNone, 0, 0),
    Metric(V5, 98, MetricId::kTorque, 1, 0),
    Metric(V5, 99, MetricId::kNone, 0, 0),
    Metric(V2 | V5, 102, 103, MetricId::kHours, ScaleFactor::TEN, 1, 0),

    Metric(V5, 104, 108, MetricId::kExhaustGasTemperature, 0, 6),
    Metric(V5, 105, 109, MetricId::kExhaustGasTemperature, 0, 7),
    Metric(V5, 106, 110, MetricId::kExhaustGasTemperature, 0, 8),
    Metric(V5, 107, MetricId::kFuelFlow, ScaleFactor::TEN_IF_GPH, 1, 1),
    Metric(V5, 111, MetricId::kHydraulicPressure, 0, 0),

    Metric(V5, 112, 116, MetricId::kExhaustGasTemperature, 1, 6),
    Metric(V5, 113, 117, MetricId::kExhaustGasTemperature, 1, 7),
    Metric(V5, 114, 118, MetricId::kExhaustGasTemperature, 1, 8),
    Metric(V5, 115, MetricId::kFuelFlow, ScaleFactor::TEN_IF_GPH, 1, 1),
    Metric(V5, 119, MetricId::kHydraulicPressure, 1, 0),

    Metric(V5, 120, MetricId::kCylinderHeadTemperature, 0, 6),
    Metric(V5, 121, MetricId::kCylinderHeadTemperature, 0, 7),
    Metric(V5, 122, MetricId::kCylinderHeadTemperature, 0, 8),
    Metric(V5, 123, MetricId::kHydraulicPressure, 0, 1),
    Metric(V5, 124, MetricId::kCylinderHeadTemperature, 1, 6),
    Metric(V5, 125, MetricId::kCylinderHeadTemperature, 1, 7),
    Metric(V5, 126, MetricId::kCylinderHeadTemperature, 1, 8),
    Metric(V5, 127, MetricId::kHydraulicPressure, 1, 1),
};

}  // namespace jpireader
