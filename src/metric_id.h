#ifndef JPIREADER_METRIC_ID_H_
#define JPIREADER_METRIC_ID_H_

namespace jpireader {

enum class MetricId {
  kNone,
  kMark,
  kVoltage,
  kAmperage,
  kOutsideAirTemperature,
  kExhaustGasTemperature,
  kCylinderHeadTemperature,
  kTurbineInletTemperature,
  kCylinderHeadTemperatureCoolingRate,
  kOilTemperature,
  kOilPressure,
  kCompressorDischargeTemperature,
  kInductionAirTemperature,
  kManifoldPressure,
  kRpm,
  kFuelFlow,
  kFuelUsed,
  kFuelPressure,
  kFuelLevel,
  kHydraulicPressure,
  kHorsepower,
  kTorque,
  kHours
};

}  // namespace jpireader

#endif  // JPIREADER_METRIC_ID_H_
