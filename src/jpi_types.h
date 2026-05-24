#ifndef JPIREADER_JPI_TYPES_H_
#define JPIREADER_JPI_TYPES_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace jpireader {

struct AlarmThresholds {
  std::optional<double> min_volts;
  std::optional<double> max_volts;
  std::optional<int32_t> max_exhaust_gas_temperature_difference;
  std::optional<int32_t> max_cylinder_head_temperature;
  std::optional<int32_t> max_cylinder_head_temperature_cooling_rate;
  std::optional<int32_t> max_exhaust_gas_temperature;
  std::optional<int32_t> min_oil_temperature;
  std::optional<int32_t> max_oil_temperature;
};

enum class FuelFlowUnits { GPH = 1, PPH = 2, LPH = 3, KPH = 4 };

struct Fuel {
  std::optional<FuelFlowUnits> fuel_flow_units;
  std::optional<int32_t> full_quantity;
  std::optional<int32_t> warning_quantity;
  std::optional<int32_t> k_factor1;
  std::optional<int32_t> k_factor2;
};

enum class TemperatureUnit { FAHRENHEIT = 1, CELSIUS = 2 };

struct Sensors {
  int32_t num_exhaust_gas_temperature = 0;
  int32_t num_cylinder_head_temperature = 0;
  bool voltage = false;
  bool oil_temperature = false;
  bool turbine_inlet_temperature1 = false;
  bool turbine_inlet_temperature2 = false;
  bool outside_air_temperature = false;
  bool fuel_flow = false;
  bool induction_air_temperature = false;
  bool compressor_discharge_temperature = false;
  bool manifold_pressure = false;
  bool rpm = false;
};

struct Features {
  std::optional<int32_t> model_number;
  std::optional<int32_t> firmware_version;
  Sensors sensors;
  std::optional<TemperatureUnit> engine_temperature_unit;
  std::optional<TemperatureUnit> oat_temperature_unit;
  std::optional<int32_t> beta_number;
  std::optional<int32_t> build_number;
};

struct FlightMetadata {
  int32_t flight_number;
  int32_t flight_data_length_words;
};

struct Metadata {
  std::string registration;
  AlarmThresholds alarm_thresholds;
  Fuel fuel;
  int64_t download_timestamp = 0;
  int32_t protocol_version = 0;
  Features features;
  std::vector<FlightMetadata> flight_metadata;
  int32_t length = 0;
  std::vector<std::string> parse_warnings;
};

enum class Mark {
  NOT_MARKED = 0,
  MARKED = 1,
  RICH_START = 2,
  RICH_END = 3,
  LEAN_START = 4,
  LEAN_END = 5
};

struct EngineDataRecord {
  std::vector<int32_t> exhaust_gas_temperature;
  std::optional<int32_t> max_exhaust_gas_temperature_difference;
  std::vector<int32_t> turbine_inlet_temperature;
  std::vector<int32_t> cylinder_head_temperature;
  std::optional<int32_t> cylinder_head_temperature_cooling_rate;
  std::optional<int32_t> oil_temperature;
  std::optional<int32_t> compressor_discharge_temperature;
  std::optional<int32_t> induction_air_temperature;
  std::optional<int32_t> interstage_turbine_temperature;

  std::vector<float> fuel_used;
  std::vector<float> fuel_flow;
  std::optional<float> fuel_pressure;
  std::vector<float> fuel_level;

  std::optional<int32_t> oil_pressure;
  std::optional<float> manifold_pressure;
  std::vector<int32_t> hydraulic_pressure;

  std::optional<int32_t> horsepower;
  std::optional<int32_t> rpm;
  std::optional<int32_t> torque;

  std::optional<float> hours;
};

struct DataRecord {
  std::vector<float> voltage;
  std::vector<int32_t> amperage;
  std::optional<int32_t> outside_air_temperature;
  Mark mark = Mark::NOT_MARKED;
  std::vector<EngineDataRecord> engine;
  std::vector<std::string> parse_warnings;
};

struct Flight {
  int32_t flight_number;
  int64_t start_timestamp = 0;
  int32_t recording_interval_secs = 0;
  Sensors sensors;
  std::vector<DataRecord> data;
  int32_t header_length = 0;
  int32_t data_length = 0;
  std::vector<std::string> parse_warnings;
};

struct JpiFile {
  Metadata metadata;
  std::vector<Flight> flights;
};

} // namespace jpireader

#endif // JPIREADER_JPI_TYPES_H_
