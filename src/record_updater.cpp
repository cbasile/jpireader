#include "src/record_updater.h"

#include <regex>
#include <string>
#include <vector>
#include <unordered_map>

namespace jpireader {

std::vector<RecordUpdater::PathInfo> RecordUpdater::ParsePath(
    const std::string& path) {
  static std::unordered_map<std::string, std::vector<PathInfo>> cache;
  auto it = cache.find(path);
  if (it != cache.end()) {
    return it->second;
  }

  std::vector<PathInfo> result;
  std::regex re("([a-z_]+)(?:\\[(\\d+)\\])?");
  auto words_begin = std::sregex_iterator(path.begin(), path.end(), re);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    PathInfo info;
    info.component = match[1].str();
    if (match[2].matched) {
      info.index = std::stoi(match[2].str());
    }
    result.push_back(info);
  }
  cache[path] = result;
  return result;
}

void RecordUpdater::Update(DataRecord& record, const std::string& path,
                           float delta) {
  if (path == "mark") {
    record.mark = static_cast<Mark>(static_cast<int>(record.mark) +
                                    static_cast<int>(delta));
    return;
  }

  auto components = ParsePath(path);
  if (components.empty()) return;

  if (components[0].component == "voltage") {
    int idx = components[0].index;
    if (record.voltage.size() <= static_cast<size_t>(idx))
      record.voltage.resize(idx + 1, 0.0f);
    record.voltage[idx] += delta;
  } else if (components[0].component == "amperage") {
    int idx = components[0].index;
    if (record.amperage.size() <= static_cast<size_t>(idx))
      record.amperage.resize(idx + 1, 0);
    record.amperage[idx] += static_cast<int32_t>(delta);
  } else if (components[0].component == "outside_air_temperature") {
    record.outside_air_temperature =
        record.outside_air_temperature.value_or(0) +
        static_cast<int32_t>(delta);
  } else if (components[0].component == "engine") {
    if (components.size() < 2) return;
    int e_idx = components[0].index;
    if (record.engine.size() <= static_cast<size_t>(e_idx))
      record.engine.resize(e_idx + 1);
    auto& engine = record.engine[e_idx];
    const std::string& field = components[1].component;
    int f_idx = components[1].index;

    if (field == "exhaust_gas_temperature") {
      if (engine.exhaust_gas_temperature.size() <= static_cast<size_t>(f_idx))
        engine.exhaust_gas_temperature.resize(f_idx + 1, 0);
      engine.exhaust_gas_temperature[f_idx] += static_cast<int32_t>(delta);
    } else if (field == "cylinder_head_temperature") {
      if (engine.cylinder_head_temperature.size() <= static_cast<size_t>(f_idx))
        engine.cylinder_head_temperature.resize(f_idx + 1, 0);
      engine.cylinder_head_temperature[f_idx] += static_cast<int32_t>(delta);
    } else if (field == "turbine_inlet_temperature") {
      if (engine.turbine_inlet_temperature.size() <= static_cast<size_t>(f_idx))
        engine.turbine_inlet_temperature.resize(f_idx + 1, 0);
      engine.turbine_inlet_temperature[f_idx] += static_cast<int32_t>(delta);
    } else if (field == "cylinder_head_temperature_cooling_rate") {
      engine.cylinder_head_temperature_cooling_rate =
          engine.cylinder_head_temperature_cooling_rate.value_or(0) +
          static_cast<int32_t>(delta);
    } else if (field == "oil_temperature") {
      engine.oil_temperature =
          engine.oil_temperature.value_or(0) + static_cast<int32_t>(delta);
    } else if (field == "oil_pressure") {
      engine.oil_pressure =
          engine.oil_pressure.value_or(0) + static_cast<int32_t>(delta);
    } else if (field == "compressor_discharge_temperature") {
      engine.compressor_discharge_temperature =
          engine.compressor_discharge_temperature.value_or(0) +
          static_cast<int32_t>(delta);
    } else if (field == "induction_air_temperature") {
      engine.induction_air_temperature =
          engine.induction_air_temperature.value_or(0) +
          static_cast<int32_t>(delta);
    } else if (field == "manifold_pressure") {
      engine.manifold_pressure =
          engine.manifold_pressure.value_or(0.0f) + delta;
    } else if (field == "rpm") {
      engine.rpm = engine.rpm.value_or(0) + static_cast<int32_t>(delta);
    } else if (field == "fuel_flow") {
      if (engine.fuel_flow.size() <= static_cast<size_t>(f_idx))
        engine.fuel_flow.resize(f_idx + 1, 0.0f);
      engine.fuel_flow[f_idx] += delta;
    } else if (field == "fuel_used") {
      if (engine.fuel_used.size() <= static_cast<size_t>(f_idx))
        engine.fuel_used.resize(f_idx + 1, 0.0f);
      engine.fuel_used[f_idx] += delta;
    } else if (field == "fuel_pressure") {
      engine.fuel_pressure = engine.fuel_pressure.value_or(0.0f) + delta;
    } else if (field == "fuel_level") {
      if (engine.fuel_level.size() <= static_cast<size_t>(f_idx))
        engine.fuel_level.resize(f_idx + 1, 0.0f);
      engine.fuel_level[f_idx] += delta;
    } else if (field == "hydraulic_pressure") {
      if (engine.hydraulic_pressure.size() <= static_cast<size_t>(f_idx))
        engine.hydraulic_pressure.resize(f_idx + 1, 0);
      engine.hydraulic_pressure[f_idx] += static_cast<int32_t>(delta);
    } else if (field == "horsepower") {
      engine.horsepower =
          engine.horsepower.value_or(0) + static_cast<int32_t>(delta);
    } else if (field == "torque") {
      engine.torque = engine.torque.value_or(0) + static_cast<int32_t>(delta);
    } else if (field == "hours") {
      engine.hours = engine.hours.value_or(0.0f) + delta;
    }
  }
}

void RecordUpdater::Clear(DataRecord& record, const std::string& path) {
  if (path == "mark") {
    record.mark = Mark::NOT_MARKED;
    return;
  }

  auto components = ParsePath(path);
  if (components.empty()) return;

  if (components[0].component == "voltage") {
    int idx = components[0].index;
    if (record.voltage.size() > static_cast<size_t>(idx))
      record.voltage[idx] = 0;
  } else if (components[0].component == "amperage") {
    int idx = components[0].index;
    if (record.amperage.size() > static_cast<size_t>(idx))
      record.amperage[idx] = 0;
  } else if (components[0].component == "outside_air_temperature") {
    record.outside_air_temperature = std::nullopt;
  } else if (components[0].component == "engine") {
    if (components.size() < 2) return;
    int e_idx = components[0].index;
    if (record.engine.size() <= static_cast<size_t>(e_idx)) return;
    auto& engine = record.engine[e_idx];
    const std::string& field = components[1].component;
    int f_idx = components[1].index;

    if (field == "exhaust_gas_temperature") {
      if (engine.exhaust_gas_temperature.size() > static_cast<size_t>(f_idx))
        engine.exhaust_gas_temperature[f_idx] = 0;
    } else if (field == "cylinder_head_temperature") {
      if (engine.cylinder_head_temperature.size() > static_cast<size_t>(f_idx))
        engine.cylinder_head_temperature[f_idx] = 0;
    } else if (field == "turbine_inlet_temperature") {
      if (engine.turbine_inlet_temperature.size() > static_cast<size_t>(f_idx))
        engine.turbine_inlet_temperature[f_idx] = 0;
    } else if (field == "cylinder_head_temperature_cooling_rate") {
      engine.cylinder_head_temperature_cooling_rate = std::nullopt;
    } else if (field == "oil_temperature") {
      engine.oil_temperature = std::nullopt;
    } else if (field == "oil_pressure") {
      engine.oil_pressure = std::nullopt;
    } else if (field == "compressor_discharge_temperature") {
      engine.compressor_discharge_temperature = std::nullopt;
    } else if (field == "induction_air_temperature") {
      engine.induction_air_temperature = std::nullopt;
    } else if (field == "manifold_pressure") {
      engine.manifold_pressure = std::nullopt;
    } else if (field == "rpm") {
      engine.rpm = std::nullopt;
    } else if (field == "fuel_flow") {
      if (engine.fuel_flow.size() > static_cast<size_t>(f_idx))
        engine.fuel_flow[f_idx] = 0;
    } else if (field == "fuel_used") {
      if (engine.fuel_used.size() > static_cast<size_t>(f_idx))
        engine.fuel_used[f_idx] = 0;
    } else if (field == "fuel_pressure") {
      engine.fuel_pressure = std::nullopt;
    } else if (field == "fuel_level") {
      if (engine.fuel_level.size() > static_cast<size_t>(f_idx))
        engine.fuel_level[f_idx] = 0;
    } else if (field == "hydraulic_pressure") {
      if (engine.hydraulic_pressure.size() > static_cast<size_t>(f_idx))
        engine.hydraulic_pressure[f_idx] = 0;
    } else if (field == "horsepower") {
      engine.horsepower = std::nullopt;
    } else if (field == "torque") {
      engine.torque = std::nullopt;
    } else if (field == "hours") {
      engine.hours = std::nullopt;
    }
  }
}

bool RecordUpdater::HasField(const DataRecord& record,
                             const std::string& path) {
  if (path == "mark") return record.mark != Mark::NOT_MARKED;

  auto components = ParsePath(path);
  if (components.empty()) return false;

  if (components[0].component == "voltage") {
    int idx = components[0].index;
    return record.voltage.size() > static_cast<size_t>(idx) &&
           record.voltage[idx] != 0;
  } else if (components[0].component == "amperage") {
    int idx = components[0].index;
    return record.amperage.size() > static_cast<size_t>(idx) &&
           record.amperage[idx] != 0;
  } else if (components[0].component == "outside_air_temperature") {
    return record.outside_air_temperature.has_value();
  } else if (components[0].component == "engine") {
    if (components.size() < 2) return false;
    int e_idx = components[0].index;
    if (record.engine.size() <= static_cast<size_t>(e_idx)) return false;
    auto& engine = record.engine[e_idx];
    const std::string& field = components[1].component;
    int f_idx = components[1].index;

    if (field == "exhaust_gas_temperature") {
      return engine.exhaust_gas_temperature.size() >
                 static_cast<size_t>(f_idx) &&
             engine.exhaust_gas_temperature[f_idx] != 0;
    } else if (field == "cylinder_head_temperature") {
      return engine.cylinder_head_temperature.size() >
                 static_cast<size_t>(f_idx) &&
             engine.cylinder_head_temperature[f_idx] != 0;
    } else if (field == "turbine_inlet_temperature") {
      return engine.turbine_inlet_temperature.size() >
                 static_cast<size_t>(f_idx) &&
             engine.turbine_inlet_temperature[f_idx] != 0;
    } else if (field == "cylinder_head_temperature_cooling_rate") {
      return engine.cylinder_head_temperature_cooling_rate.has_value();
    } else if (field == "oil_temperature") {
      return engine.oil_temperature.has_value();
    } else if (field == "oil_pressure") {
      return engine.oil_pressure.has_value();
    } else if (field == "compressor_discharge_temperature") {
      return engine.compressor_discharge_temperature.has_value();
    } else if (field == "induction_air_temperature") {
      return engine.induction_air_temperature.has_value();
    } else if (field == "manifold_pressure") {
      return engine.manifold_pressure.has_value();
    } else if (field == "rpm") {
      return engine.rpm.has_value();
    } else if (field == "fuel_flow") {
      return engine.fuel_flow.size() > static_cast<size_t>(f_idx) &&
             engine.fuel_flow[f_idx] != 0;
    } else if (field == "fuel_used") {
      return engine.fuel_used.size() > static_cast<size_t>(f_idx) &&
             engine.fuel_used[f_idx] != 0;
    } else if (field == "fuel_pressure") {
      return engine.fuel_pressure.has_value();
    } else if (field == "fuel_level") {
      return engine.fuel_level.size() > static_cast<size_t>(f_idx) &&
             engine.fuel_level[f_idx] != 0;
    } else if (field == "hydraulic_pressure") {
      return engine.hydraulic_pressure.size() > static_cast<size_t>(f_idx) &&
             engine.hydraulic_pressure[f_idx] != 0;
    } else if (field == "horsepower") {
      return engine.horsepower.has_value();
    } else if (field == "torque") {
      return engine.torque.has_value();
    } else if (field == "hours") {
      return engine.hours.has_value();
    }
  }
  return false;
}

float RecordUpdater::GetValue(const DataRecord& record,
                              const std::string& path) {
  if (path == "mark") return static_cast<float>(record.mark);

  auto components = ParsePath(path);
  if (components.empty()) return 0;

  if (components[0].component == "voltage") {
    int idx = components[0].index;
    return record.voltage.size() > static_cast<size_t>(idx)
               ? record.voltage[idx]
               : 0;
  } else if (components[0].component == "amperage") {
    int idx = components[0].index;
    return record.amperage.size() > static_cast<size_t>(idx)
               ? static_cast<float>(record.amperage[idx])
               : 0;
  } else if (components[0].component == "outside_air_temperature") {
    return static_cast<float>(record.outside_air_temperature.value_or(0));
  } else if (components[0].component == "engine") {
    if (components.size() < 2) return 0;
    int e_idx = components[0].index;
    if (record.engine.size() <= static_cast<size_t>(e_idx)) return 0;
    auto& engine = record.engine[e_idx];
    const std::string& field = components[1].component;
    int f_idx = components[1].index;

    if (field == "exhaust_gas_temperature") {
      return engine.exhaust_gas_temperature.size() > static_cast<size_t>(f_idx)
                 ? static_cast<float>(engine.exhaust_gas_temperature[f_idx])
                 : 0;
    } else if (field == "cylinder_head_temperature") {
      return engine.cylinder_head_temperature.size() >
                     static_cast<size_t>(f_idx)
                 ? static_cast<float>(engine.cylinder_head_temperature[f_idx])
                 : 0;
    } else if (field == "turbine_inlet_temperature") {
      return engine.turbine_inlet_temperature.size() >
                     static_cast<size_t>(f_idx)
                 ? static_cast<float>(engine.turbine_inlet_temperature[f_idx])
                 : 0;
    } else if (field == "cylinder_head_temperature_cooling_rate") {
      return static_cast<float>(
          engine.cylinder_head_temperature_cooling_rate.value_or(0));
    } else if (field == "oil_temperature") {
      return static_cast<float>(engine.oil_temperature.value_or(0));
    } else if (field == "oil_pressure") {
      return static_cast<float>(engine.oil_pressure.value_or(0));
    } else if (field == "compressor_discharge_temperature") {
      return static_cast<float>(
          engine.compressor_discharge_temperature.value_or(0));
    } else if (field == "induction_air_temperature") {
      return static_cast<float>(engine.induction_air_temperature.value_or(0));
    } else if (field == "manifold_pressure") {
      return engine.manifold_pressure.value_or(0.0f);
    } else if (field == "rpm") {
      return static_cast<float>(engine.rpm.value_or(0));
    } else if (field == "fuel_flow") {
      return engine.fuel_flow.size() > static_cast<size_t>(f_idx)
                 ? engine.fuel_flow[f_idx]
                 : 0;
    } else if (field == "fuel_used") {
      return engine.fuel_used.size() > static_cast<size_t>(f_idx)
                 ? engine.fuel_used[f_idx]
                 : 0;
    } else if (field == "fuel_pressure") {
      return engine.fuel_pressure.value_or(0.0f);
    } else if (field == "fuel_level") {
      return engine.fuel_level.size() > static_cast<size_t>(f_idx)
                 ? engine.fuel_level[f_idx]
                 : 0;
    } else if (field == "hydraulic_pressure") {
      return engine.hydraulic_pressure.size() > static_cast<size_t>(f_idx)
                 ? static_cast<float>(engine.hydraulic_pressure[f_idx])
                 : 0;
    } else if (field == "horsepower") {
      return static_cast<float>(engine.horsepower.value_or(0));
    } else if (field == "torque") {
      return static_cast<float>(engine.torque.value_or(0));
    } else if (field == "hours") {
      return engine.hours.value_or(0.0f);
    }
  }
  return 0;
}

}  // namespace jpireader
