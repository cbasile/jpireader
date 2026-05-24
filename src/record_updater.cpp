#include "src/record_updater.h"

namespace jpireader {

void RecordUpdater::Update(DataRecord& record, MetricId id, int engine_idx,
                           int field_idx, float delta) {
  if (id == MetricId::kNone) return;

  switch (id) {
    case MetricId::kMark:
      record.mark = static_cast<Mark>(static_cast<int>(record.mark) +
                                      static_cast<int>(delta));
      break;
    case MetricId::kVoltage:
      if (record.voltage.size() <= static_cast<size_t>(field_idx)) {
        record.voltage.resize(field_idx + 1, 0.0f);
      }
      record.voltage[field_idx] += delta;
      break;
    case MetricId::kAmperage:
      if (record.amperage.size() <= static_cast<size_t>(field_idx)) {
        record.amperage.resize(field_idx + 1, 0);
      }
      record.amperage[field_idx] += static_cast<int32_t>(delta);
      break;
    case MetricId::kOutsideAirTemperature:
      record.outside_air_temperature =
          record.outside_air_temperature.value_or(0) +
          static_cast<int32_t>(delta);
      break;
    default: {
      if (record.engine.size() <= static_cast<size_t>(engine_idx)) {
        record.engine.resize(engine_idx + 1);
      }
      auto& engine = record.engine[engine_idx];
      switch (id) {
        case MetricId::kExhaustGasTemperature:
          if (engine.exhaust_gas_temperature.size() <=
              static_cast<size_t>(field_idx)) {
            engine.exhaust_gas_temperature.resize(field_idx + 1, 0);
          }
          engine.exhaust_gas_temperature[field_idx] +=
              static_cast<int32_t>(delta);
          break;
        case MetricId::kCylinderHeadTemperature:
          if (engine.cylinder_head_temperature.size() <=
              static_cast<size_t>(field_idx)) {
            engine.cylinder_head_temperature.resize(field_idx + 1, 0);
          }
          engine.cylinder_head_temperature[field_idx] +=
              static_cast<int32_t>(delta);
          break;
        case MetricId::kTurbineInletTemperature:
          if (engine.turbine_inlet_temperature.size() <=
              static_cast<size_t>(field_idx)) {
            engine.turbine_inlet_temperature.resize(field_idx + 1, 0);
          }
          engine.turbine_inlet_temperature[field_idx] +=
              static_cast<int32_t>(delta);
          break;
        case MetricId::kCylinderHeadTemperatureCoolingRate:
          engine.cylinder_head_temperature_cooling_rate =
              engine.cylinder_head_temperature_cooling_rate.value_or(0) +
              static_cast<int32_t>(delta);
          break;
        case MetricId::kOilTemperature:
          engine.oil_temperature =
              engine.oil_temperature.value_or(0) + static_cast<int32_t>(delta);
          break;
        case MetricId::kOilPressure:
          engine.oil_pressure =
              engine.oil_pressure.value_or(0) + static_cast<int32_t>(delta);
          break;
        case MetricId::kCompressorDischargeTemperature:
          engine.compressor_discharge_temperature =
              engine.compressor_discharge_temperature.value_or(0) +
              static_cast<int32_t>(delta);
          break;
        case MetricId::kInductionAirTemperature:
          engine.induction_air_temperature =
              engine.induction_air_temperature.value_or(0) +
              static_cast<int32_t>(delta);
          break;
        case MetricId::kManifoldPressure:
          engine.manifold_pressure =
              engine.manifold_pressure.value_or(0.0f) + delta;
          break;
        case MetricId::kRpm:
          engine.rpm = engine.rpm.value_or(0) + static_cast<int32_t>(delta);
          break;
        case MetricId::kFuelFlow:
          if (engine.fuel_flow.size() <= static_cast<size_t>(field_idx)) {
            engine.fuel_flow.resize(field_idx + 1, 0.0f);
          }
          engine.fuel_flow[field_idx] += delta;
          break;
        case MetricId::kFuelUsed:
          if (engine.fuel_used.size() <= static_cast<size_t>(field_idx)) {
            engine.fuel_used.resize(field_idx + 1, 0.0f);
          }
          engine.fuel_used[field_idx] += delta;
          break;
        case MetricId::kFuelPressure:
          engine.fuel_pressure = engine.fuel_pressure.value_or(0.0f) + delta;
          break;
        case MetricId::kFuelLevel:
          if (engine.fuel_level.size() <= static_cast<size_t>(field_idx)) {
            engine.fuel_level.resize(field_idx + 1, 0.0f);
          }
          engine.fuel_level[field_idx] += delta;
          break;
        case MetricId::kHydraulicPressure:
          if (engine.hydraulic_pressure.size() <=
              static_cast<size_t>(field_idx)) {
            engine.hydraulic_pressure.resize(field_idx + 1, 0);
          }
          engine.hydraulic_pressure[field_idx] += static_cast<int32_t>(delta);
          break;
        case MetricId::kHorsepower:
          engine.horsepower =
              engine.horsepower.value_or(0) + static_cast<int32_t>(delta);
          break;
        case MetricId::kTorque:
          engine.torque =
              engine.torque.value_or(0) + static_cast<int32_t>(delta);
          break;
        case MetricId::kHours:
          engine.hours = engine.hours.value_or(0.0f) + delta;
          break;
        default:
          break;
      }
      break;
    }
  }
}

void RecordUpdater::Clear(DataRecord& record, MetricId id, int engine_idx,
                          int field_idx) {
  if (id == MetricId::kNone) return;

  switch (id) {
    case MetricId::kMark:
      record.mark = Mark::NOT_MARKED;
      break;
    case MetricId::kVoltage:
      if (record.voltage.size() > static_cast<size_t>(field_idx)) {
        record.voltage[field_idx] = 0.0f;
      }
      break;
    case MetricId::kAmperage:
      if (record.amperage.size() > static_cast<size_t>(field_idx)) {
        record.amperage[field_idx] = 0;
      }
      break;
    case MetricId::kOutsideAirTemperature:
      record.outside_air_temperature = std::nullopt;
      break;
    default: {
      if (record.engine.size() <= static_cast<size_t>(engine_idx)) return;
      auto& engine = record.engine[engine_idx];
      switch (id) {
        case MetricId::kExhaustGasTemperature:
          if (engine.exhaust_gas_temperature.size() >
              static_cast<size_t>(field_idx)) {
            engine.exhaust_gas_temperature[field_idx] = 0;
          }
          break;
        case MetricId::kCylinderHeadTemperature:
          if (engine.cylinder_head_temperature.size() >
              static_cast<size_t>(field_idx)) {
            engine.cylinder_head_temperature[field_idx] = 0;
          }
          break;
        case MetricId::kTurbineInletTemperature:
          if (engine.turbine_inlet_temperature.size() >
              static_cast<size_t>(field_idx)) {
            engine.turbine_inlet_temperature[field_idx] = 0;
          }
          break;
        case MetricId::kCylinderHeadTemperatureCoolingRate:
          engine.cylinder_head_temperature_cooling_rate = std::nullopt;
          break;
        case MetricId::kOilTemperature:
          engine.oil_temperature = std::nullopt;
          break;
        case MetricId::kOilPressure:
          engine.oil_pressure = std::nullopt;
          break;
        case MetricId::kCompressorDischargeTemperature:
          engine.compressor_discharge_temperature = std::nullopt;
          break;
        case MetricId::kInductionAirTemperature:
          engine.induction_air_temperature = std::nullopt;
          break;
        case MetricId::kManifoldPressure:
          engine.manifold_pressure = std::nullopt;
          break;
        case MetricId::kRpm:
          engine.rpm = std::nullopt;
          break;
        case MetricId::kFuelFlow:
          if (engine.fuel_flow.size() > static_cast<size_t>(field_idx)) {
            engine.fuel_flow[field_idx] = 0.0f;
          }
          break;
        case MetricId::kFuelUsed:
          if (engine.fuel_used.size() > static_cast<size_t>(field_idx)) {
            engine.fuel_used[field_idx] = 0.0f;
          }
          break;
        case MetricId::kFuelPressure:
          engine.fuel_pressure = std::nullopt;
          break;
        case MetricId::kFuelLevel:
          if (engine.fuel_level.size() > static_cast<size_t>(field_idx)) {
            engine.fuel_level[field_idx] = 0.0f;
          }
          break;
        case MetricId::kHydraulicPressure:
          if (engine.hydraulic_pressure.size() >
              static_cast<size_t>(field_idx)) {
            engine.hydraulic_pressure[field_idx] = 0;
          }
          break;
        case MetricId::kHorsepower:
          engine.horsepower = std::nullopt;
          break;
        case MetricId::kTorque:
          engine.torque = std::nullopt;
          break;
        case MetricId::kHours:
          engine.hours = std::nullopt;
          break;
        default:
          break;
      }
      break;
    }
  }
}

bool RecordUpdater::HasField(const DataRecord& record, MetricId id,
                             int engine_idx, int field_idx) {
  if (id == MetricId::kNone) return false;

  switch (id) {
    case MetricId::kMark:
      return record.mark != Mark::NOT_MARKED;
    case MetricId::kVoltage:
      return record.voltage.size() > static_cast<size_t>(field_idx) &&
             record.voltage[field_idx] != 0.0f;
    case MetricId::kAmperage:
      return record.amperage.size() > static_cast<size_t>(field_idx) &&
             record.amperage[field_idx] != 0;
    case MetricId::kOutsideAirTemperature:
      return record.outside_air_temperature.has_value();
    default: {
      if (record.engine.size() <= static_cast<size_t>(engine_idx)) return false;
      const auto& engine = record.engine[engine_idx];
      switch (id) {
        case MetricId::kExhaustGasTemperature:
          return engine.exhaust_gas_temperature.size() >
                     static_cast<size_t>(field_idx) &&
                 engine.exhaust_gas_temperature[field_idx] != 0;
        case MetricId::kCylinderHeadTemperature:
          return engine.cylinder_head_temperature.size() >
                     static_cast<size_t>(field_idx) &&
                 engine.cylinder_head_temperature[field_idx] != 0;
        case MetricId::kTurbineInletTemperature:
          return engine.turbine_inlet_temperature.size() >
                     static_cast<size_t>(field_idx) &&
                 engine.turbine_inlet_temperature[field_idx] != 0;
        case MetricId::kCylinderHeadTemperatureCoolingRate:
          return engine.cylinder_head_temperature_cooling_rate.has_value();
        case MetricId::kOilTemperature:
          return engine.oil_temperature.has_value();
        case MetricId::kOilPressure:
          return engine.oil_pressure.has_value();
        case MetricId::kCompressorDischargeTemperature:
          return engine.compressor_discharge_temperature.has_value();
        case MetricId::kInductionAirTemperature:
          return engine.induction_air_temperature.has_value();
        case MetricId::kManifoldPressure:
          return engine.manifold_pressure.has_value();
        case MetricId::kRpm:
          return engine.rpm.has_value();
        case MetricId::kFuelFlow:
          return engine.fuel_flow.size() > static_cast<size_t>(field_idx) &&
                 engine.fuel_flow[field_idx] != 0.0f;
        case MetricId::kFuelUsed:
          return engine.fuel_used.size() > static_cast<size_t>(field_idx) &&
                 engine.fuel_used[field_idx] != 0.0f;
        case MetricId::kFuelPressure:
          return engine.fuel_pressure.has_value();
        case MetricId::kFuelLevel:
          return engine.fuel_level.size() > static_cast<size_t>(field_idx) &&
                 engine.fuel_level[field_idx] != 0.0f;
        case MetricId::kHydraulicPressure:
          return engine.hydraulic_pressure.size() >
                     static_cast<size_t>(field_idx) &&
                 engine.hydraulic_pressure[field_idx] != 0;
        case MetricId::kHorsepower:
          return engine.horsepower.has_value();
        case MetricId::kTorque:
          return engine.torque.has_value();
        case MetricId::kHours:
          return engine.hours.has_value();
        default:
          return false;
      }
    }
  }
}

float RecordUpdater::GetValue(const DataRecord& record, MetricId id,
                              int engine_idx, int field_idx) {
  if (id == MetricId::kNone) return 0.0f;

  switch (id) {
    case MetricId::kMark:
      return static_cast<float>(record.mark);
    case MetricId::kVoltage:
      return record.voltage.size() > static_cast<size_t>(field_idx)
                 ? record.voltage[field_idx]
                 : 0.0f;
    case MetricId::kAmperage:
      return record.amperage.size() > static_cast<size_t>(field_idx)
                 ? static_cast<float>(record.amperage[field_idx])
                 : 0.0f;
    case MetricId::kOutsideAirTemperature:
      return static_cast<float>(record.outside_air_temperature.value_or(0));
    default: {
      if (record.engine.size() <= static_cast<size_t>(engine_idx)) return 0.0f;
      const auto& engine = record.engine[engine_idx];
      switch (id) {
        case MetricId::kExhaustGasTemperature:
          return engine.exhaust_gas_temperature.size() >
                     static_cast<size_t>(field_idx)
                 ? static_cast<float>(engine.exhaust_gas_temperature[field_idx])
                 : 0.0f;
        case MetricId::kCylinderHeadTemperature:
          return engine.cylinder_head_temperature.size() >
                     static_cast<size_t>(field_idx)
                 ? static_cast<float>(
                       engine.cylinder_head_temperature[field_idx])
                 : 0.0f;
        case MetricId::kTurbineInletTemperature:
          return engine.turbine_inlet_temperature.size() >
                     static_cast<size_t>(field_idx)
                 ? static_cast<float>(
                       engine.turbine_inlet_temperature[field_idx])
                 : 0.0f;
        case MetricId::kCylinderHeadTemperatureCoolingRate:
          return static_cast<float>(
              engine.cylinder_head_temperature_cooling_rate.value_or(0));
        case MetricId::kOilTemperature:
          return static_cast<float>(engine.oil_temperature.value_or(0));
        case MetricId::kOilPressure:
          return static_cast<float>(engine.oil_pressure.value_or(0));
        case MetricId::kCompressorDischargeTemperature:
          return static_cast<float>(
              engine.compressor_discharge_temperature.value_or(0));
        case MetricId::kInductionAirTemperature:
          return static_cast<float>(
              engine.induction_air_temperature.value_or(0));
        case MetricId::kManifoldPressure:
          return engine.manifold_pressure.value_or(0.0f);
        case MetricId::kRpm:
          return static_cast<float>(engine.rpm.value_or(0));
        case MetricId::kFuelFlow:
          return engine.fuel_flow.size() > static_cast<size_t>(field_idx)
                 ? engine.fuel_flow[field_idx]
                 : 0.0f;
        case MetricId::kFuelUsed:
          return engine.fuel_used.size() > static_cast<size_t>(field_idx)
                 ? engine.fuel_used[field_idx]
                 : 0.0f;
        case MetricId::kFuelPressure:
          return engine.fuel_pressure.value_or(0.0f);
        case MetricId::kFuelLevel:
          return engine.fuel_level.size() > static_cast<size_t>(field_idx)
                 ? engine.fuel_level[field_idx]
                 : 0.0f;
        case MetricId::kHydraulicPressure:
          return engine.hydraulic_pressure.size() >
                     static_cast<size_t>(field_idx)
                 ? static_cast<float>(engine.hydraulic_pressure[field_idx])
                 : 0.0f;
        case MetricId::kHorsepower:
          return static_cast<float>(engine.horsepower.value_or(0));
        case MetricId::kTorque:
          return static_cast<float>(engine.torque.value_or(0));
        case MetricId::kHours:
          return engine.hours.value_or(0.0f);
        default:
          return 0.0f;
      }
    }
  }
}

}  // namespace jpireader
