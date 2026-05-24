#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "src/jpi_decoder.h"
#include "src/jpi_stream.h"

namespace fs = std::filesystem;

std::string FindTestData(const std::string& filename) {
  std::vector<std::string> search_paths = {"testdata/" + filename,
                                           "../testdata/" + filename,
                                           "../../testdata/" + filename};
  for (const auto& path : search_paths) {
    if (fs::exists(path)) return path;
  }
  return "";
}

void TestEdm830() {
  std::cout << "Testing edm830.jpi..." << std::endl;
  std::string path = FindTestData("edm830.jpi");
  if (path.empty()) {
    std::cerr << "Skip: edm830.jpi not found" << std::endl;
    return;
  }

  jpireader::JpiStream stream(path);
  jpireader::JpiFile jpi = jpireader::JpiDecoder::Decode(stream);

  assert(jpi.metadata.registration == "edm830");
  assert(jpi.metadata.protocol_version == 2);
  assert(jpi.flights.size() == 2);

  // Flight 45
  assert(jpi.flights[0].flight_number == 45);
  assert(jpi.flights[0].data.size() == 120);

  // Flight 72
  assert(jpi.flights[1].flight_number == 72);
  assert(jpi.flights[1].data.size() == 21);

  std::cout << "  Passed edm830.jpi" << std::endl;
}

std::string Trim(const std::string& s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start)) {
    start++;
  }
  auto end = s.end();
  if (start == s.end()) return "";
  do {
    end--;
  } while (std::distance(start, end) > 0 && std::isspace(*end));
  return std::string(start, end + 1);
}

std::vector<std::string> SplitCsvLine(const std::string& line) {
  std::vector<std::string> result;
  std::string current;
  for (char c : line) {
    if (c == ',') {
      result.push_back(Trim(current));
      current.clear();
    } else {
      current += c;
    }
  }
  result.push_back(Trim(current));
  return result;
}

void VerifyFlight158AgainstCsv(const jpireader::Flight& flight,
                               const std::string& csv_path) {
  std::ifstream ifs(csv_path);
  assert(ifs.is_open());
  std::string line;
  std::vector<std::vector<std::string>> csv_rows;
  while (std::getline(ifs, line)) {
    if (line.starts_with("Left Engine") || line.starts_with("Right Engine") ||
        line.starts_with("INDEX")) {
      continue;
    }
    if (line.empty()) continue;
    csv_rows.push_back(SplitCsvLine(line));
  }

  assert(csv_rows.size() == flight.data.size());

  for (size_t i = 0; i < flight.data.size(); ++i) {
    const auto& record = flight.data[i];
    const auto& row = csv_rows[i];

    // BAT (Voltage)
    double bat = std::stod(row[19]);
    assert(!record.voltage.empty());
    assert(std::abs(record.voltage[0] - bat) < 0.051);

    // OAT
    double oat = std::stod(row[15]);
    assert(record.outside_air_temperature.has_value());
    assert(std::abs(*record.outside_air_temperature - oat) < 0.051);

    // LE1..6 (exhaust_gas_temperature)
    assert(record.engine.size() == 2);
    const auto& engine1 = record.engine[0];
    assert(engine1.exhaust_gas_temperature.size() == 6);
    for (int c = 0; c < 6; ++c) {
      int ref_egt = std::stoi(row[3 + c]);
      assert(engine1.exhaust_gas_temperature[c] == ref_egt);
    }

    // LC1..6 (cylinder_head_temperature)
    assert(engine1.cylinder_head_temperature.size() == 6);
    for (int c = 0; c < 6; ++c) {
      int ref_cht = std::stoi(row[9 + c]);
      assert(engine1.cylinder_head_temperature[c] == ref_cht);
    }

    // LFF (FuelFlow)
    double lff = std::stod(row[18]);
    assert(!engine1.fuel_flow.empty());
    assert(std::abs(engine1.fuel_flow[0] - lff) < 0.051);

    // LUSD (FuelUsed)
    double lusd = std::stod(row[21]);
    assert(!engine1.fuel_used.empty());
    assert(std::abs(engine1.fuel_used[0] - lusd) < 0.051);

    // LOILT (OilTemp)
    int loilt = std::stoi(row[20]);
    assert(engine1.oil_temperature.has_value());
    assert(*engine1.oil_temperature == loilt);

    // RE1..6
    const auto& engine2 = record.engine[1];
    assert(engine2.exhaust_gas_temperature.size() == 6);
    for (int c = 0; c < 6; ++c) {
      int ref_egt = std::stoi(row[23 + c]);
      assert(engine2.exhaust_gas_temperature[c] == ref_egt);
    }

    // RC1..6
    assert(engine2.cylinder_head_temperature.size() == 6);
    for (int c = 0; c < 6; ++c) {
      int ref_cht = std::stoi(row[29 + c]);
      assert(engine2.cylinder_head_temperature[c] == ref_cht);
    }

    // RFF
    double rff = std::stod(row[37]);
    assert(!engine2.fuel_flow.empty());
    assert(std::abs(engine2.fuel_flow[0] - rff) < 0.051);

    // RUSD
    double rusd = std::stod(row[39]);
    assert(!engine2.fuel_used.empty());
    assert(std::abs(engine2.fuel_used[0] - rusd) < 0.051);

    // ROILT
    int roilt = std::stoi(row[38]);
    assert(engine2.oil_temperature.has_value());
    assert(*engine2.oil_temperature == roilt);
  }
}

void TestU260523() {
  std::cout << "Testing U260523.JPI..." << std::endl;
  std::string path = FindTestData("U260523.JPI");
  if (path.empty()) {
    std::cerr << "Skip: U260523.JPI not found" << std::endl;
    return;
  }

  jpireader::JpiStream stream(path);
  jpireader::JpiFile jpi = jpireader::JpiDecoder::Decode(stream);

  assert(jpi.metadata.registration == "N111XX");
  assert(jpi.metadata.protocol_version == 2);
  assert(jpi.flights.size() == 21);

  // Verify some specific flight record counts
  assert(jpi.flights[0].flight_number == 138);
  assert(jpi.flights[0].data.size() == 1249);

  assert(jpi.flights[20].flight_number == 158);
  assert(jpi.flights[20].data.size() == 4208);

  std::string csv_path = FindTestData("Flt158.csv");
  if (csv_path.empty()) {
    std::cerr << "Skip: Flt158.csv not found" << std::endl;
    return;
  }
  VerifyFlight158AgainstCsv(jpi.flights[20], csv_path);

  std::cout << "  Passed U260523.JPI" << std::endl;
}

int main() {
  try {
    TestEdm830();
    TestU260523();
    std::cout << "All tests passed!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
