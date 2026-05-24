#include "src/metadata_parser.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "src/bit_set.h"
#include "src/metadata_util.h"
#include "src/sensor_parser.h"

namespace jpireader {

namespace {

std::vector<std::string> Split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    // Trim whitespace
    token.erase(token.begin(),
                std::find_if(token.begin(), token.end(), [](unsigned char ch) {
                  return !std::isspace(ch);
                }));
    token.erase(std::find_if(token.rbegin(), token.rend(),
                             [](unsigned char ch) { return !std::isspace(ch); })
                    .base(),
                token.end());
    tokens.push_back(token);
  }
  // If the string ends with a delimiter, add an empty string
  if (!s.empty() && s.back() == delimiter) {
    tokens.push_back("");
  }
  return tokens;
}

} // namespace

MetadataParser::HeaderStream::HeaderStream(JpiStream &stream)
    : stream_(stream) {
  stream_.ResetCounter();
}

std::string MetadataParser::HeaderStream::ReadLine() {
  std::string result;
  for (int i = 0; i < 128; ++i) {
    char c = static_cast<char>(stream_.Read());
    result += c;
    if (result.size() >= 2 && result.substr(result.size() - 2) == "\r\n") {
      return result.substr(0, result.size() - 2);
    }
  }
  throw std::runtime_error("Header input too large");
}

std::vector<std::string> MetadataParser::HeaderStream::NextHeader() {
  current_failure_message_ = std::nullopt;
  std::string line = ReadLine();

  size_t star_pos = line.find('*');
  if (star_pos == std::string::npos) {
    throw std::runtime_error("Expected a checksum denoted with * in line: " +
                             line);
  }

  std::string data = line.substr(0, star_pos);
  std::string actual_checksum_string = line.substr(star_pos + 1);

  if (data.empty() || data[0] != '$') {
    throw std::runtime_error("Expected line to begin with $: " + data);
  }

  std::string payload = data.substr(1);
  int computed_checksum = 0;
  for (char c : payload) {
    computed_checksum ^= static_cast<uint8_t>(c);
  }

  try {
    int actual_checksum = std::stoi(actual_checksum_string, nullptr, 16);
    if (computed_checksum != actual_checksum) {
      std::stringstream ss;
      ss << "Checksum mismatch actual " << std::hex << std::uppercase
         << actual_checksum << " vs expected " << computed_checksum << ":\n"
         << payload;
      current_failure_message_ = ss.str();
    }
    return Split(payload, ',');
  } catch (const std::exception &e) {
    throw std::runtime_error("Checksum byte malformed: " +
                             std::string(e.what()) + " in line: " + line);
  }
}

std::optional<std::string>
MetadataParser::HeaderStream::GetChecksumFailureMessage() const {
  return current_failure_message_;
}

int MetadataParser::HeaderStream::GetCounter() const {
  return stream_.GetCounter();
}

MetadataParser::MetadataParser(JpiStream &stream) : header_stream_(stream) {}

Metadata MetadataParser::Parse() {
  Metadata metadata;
  int num_headers = 0;
  while (num_headers++ < 128) {
    std::vector<std::string> header = header_stream_.NextHeader();
    if (auto msg = header_stream_.GetChecksumFailureMessage()) {
      metadata.parse_warnings.push_back(*msg);
    }
    if (!ParseHeaderToMetadata(header, metadata)) {
      break;
    }
  }
  metadata.length = header_stream_.GetCounter();
  return metadata;
}

bool MetadataParser::ParseHeaderToMetadata(
    const std::vector<std::string> &header, Metadata &metadata) {
  if (header.empty())
    return true;

  const std::string &prefix = header[0];
  if (prefix == "A") {
    ParseAlarmThresholds(header, metadata.alarm_thresholds);
  } else if (prefix == "C") {
    ParseFeatures(header, metadata.features);
  } else if (prefix == "D") {
    FlightMetadata fm;
    fm.flight_number = std::stoi(header[1]);
    fm.flight_data_length_words = std::stoi(header[2]);
    metadata.flight_metadata.push_back(fm);
  } else if (prefix == "F") {
    ParseFuelConfiguration(header, metadata.fuel);
  } else if (prefix == "L") {
    return false;
  } else if (prefix == "P") {
    metadata.protocol_version = std::stoi(header[1]);
  } else if (prefix == "T") {
    metadata.download_timestamp = ParseUnixTimestamp(header);
  } else if (prefix == "U") {
    std::string reg = header[1];
    std::replace(reg.begin(), reg.end(), '_', ' ');
    // Simple trim
    reg.erase(reg.begin(),
              std::find_if(reg.begin(), reg.end(),
                           [](unsigned char ch) { return !std::isspace(ch); }));
    reg.erase(std::find_if(reg.rbegin(), reg.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              reg.end());
    metadata.registration = reg;
  }
  return true;
}

void MetadataParser::ParseAlarmThresholds(const std::vector<std::string> &parts,
                                          AlarmThresholds &thresholds) {
  if (parts.size() < 9)
    return;
  thresholds.max_volts = std::stod(parts[1]) / 10.0;
  thresholds.min_volts = std::stod(parts[2]) / 10.0;
  thresholds.max_exhaust_gas_temperature_difference = std::stoi(parts[3]);
  thresholds.max_cylinder_head_temperature = std::stoi(parts[4]);
  thresholds.max_cylinder_head_temperature_cooling_rate = std::stoi(parts[5]);
  thresholds.max_exhaust_gas_temperature = std::stoi(parts[6]);
  thresholds.max_oil_temperature = std::stoi(parts[7]);
  thresholds.min_oil_temperature = std::stoi(parts[8]);
}

void MetadataParser::ParseFuelConfiguration(
    const std::vector<std::string> &parts, Fuel &fuel) {
  if (parts.size() < 6)
    return;
  fuel.fuel_flow_units = static_cast<FuelFlowUnits>(std::stoi(parts[1]) + 1);
  fuel.full_quantity = std::stoi(parts[2]);
  fuel.warning_quantity = std::stoi(parts[3]);
  fuel.k_factor1 = std::stoi(parts[4]);
  fuel.k_factor2 = std::stoi(parts[5]);
}

int64_t
MetadataParser::ParseUnixTimestamp(const std::vector<std::string> &parts) {
  if (parts.size() < 6)
    return 0;
  int month = std::stoi(parts[1]);
  int day = std::stoi(parts[2]);
  int year = std::stoi(parts[3]) + 2000;
  int hour = std::stoi(parts[4]);
  int minute = std::stoi(parts[5]);

  std::tm tm = {};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_isdst = -1;

  return MetadataUtil::Timegm(tm);
}

void MetadataParser::ParseFeatures(const std::vector<std::string> &parts,
                                   Features &features) {
  if (parts.size() < 4)
    return;
  features.model_number = std::stoi(parts[1]);
  int low = std::stoi(parts[2]);
  int high = std::stoi(parts[3]);
  features.sensors = SensorParser(low, high).Parse();

  BitSet units(2);
  units.SetWord(0, high);
  features.engine_temperature_unit = units.TestBit(12)
                                         ? TemperatureUnit::FAHRENHEIT
                                         : TemperatureUnit::CELSIUS;
  features.oat_temperature_unit = units.TestBit(10)
                                      ? TemperatureUnit::FAHRENHEIT
                                      : TemperatureUnit::CELSIUS;

  // Tail parsing for versions
  if (parts.size() > 4) {
    std::vector<std::string> tail;
    for (size_t i = 4; i < parts.size(); ++i) {
      tail.push_back(parts[i]);
    }
    std::reverse(tail.begin(), tail.end());
    if (tail.size() > 3) {
      features.beta_number = std::stoi(tail[0]);
      features.build_number = std::stoi(tail[1]);
      features.firmware_version = std::stoi(tail[2]);
    } else if (tail.size() >= 1) {
      features.firmware_version = std::stoi(tail[0]);
    }
  }
}

} // namespace jpireader
