#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/csv_writer.h"
#include "src/jpi_decoder.h"
#include "src/jpi_stream.h"

namespace fs = std::filesystem;

void VerifyFlight158AgainstCsv(const jpireader::Flight& flight,
                               const std::string& csv_path);
void VerifyFlight72AgainstCsv(const jpireader::Flight& flight,
                              const std::string& csv_path);

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

  std::string csv_path = FindTestData("Flt72.csv");
  if (csv_path.empty()) {
    std::cerr << "Skip: Flt72.csv not found" << std::endl;
    return;
  }
  VerifyFlight72AgainstCsv(jpi.flights[1], csv_path);

  std::cout << "  Passed edm830.jpi" << std::endl;
}

void VerifyFlight158AgainstCsv(const jpireader::Flight& flight,
                               const std::string& csv_path) {
  std::ifstream ifs(csv_path);
  assert(ifs.is_open());

  std::stringstream ss;
  jpireader::PrintFlightCsvCustom(ss, flight);

  std::string ref_line;
  std::string gen_line;
  int line_num = 1;
  while (std::getline(ifs, ref_line)) {
    // Strip trailing \r if present
    if (!ref_line.empty() && ref_line.back() == '\r') {
      ref_line.pop_back();
    }

    bool got_gen = static_cast<bool>(std::getline(ss, gen_line));
    if (!got_gen) {
      std::cerr << "Gen stream ended early at line " << line_num << std::endl;
      assert(false);
    }
    if (!gen_line.empty() && gen_line.back() == '\r') {
      gen_line.pop_back();
    }

    if (ref_line != gen_line) {
      std::cerr << "Mismatch at line " << line_num << std::endl;
      std::cerr << "Ref: [" << ref_line << "]" << std::endl;
      std::cerr << "Gen: [" << gen_line << "]" << std::endl;
      assert(false);
    }
    line_num++;
  }
  // Ensure ss is also exhausted
  bool got_extra = static_cast<bool>(std::getline(ss, gen_line));
  if (got_extra) {
    std::cerr << "Gen has extra lines starting with: [" << gen_line << "]"
              << std::endl;
    assert(false);
  }
}

void VerifyFlight72AgainstCsv(const jpireader::Flight& flight,
                              const std::string& csv_path) {
  std::ifstream ifs(csv_path);
  assert(ifs.is_open());

  std::stringstream ss;
  jpireader::PrintFlightCsvEdm830(ss, flight);

  std::string ref_line;
  std::string gen_line;
  int line_num = 1;
  while (std::getline(ifs, ref_line)) {
    // Strip trailing \r if present
    if (!ref_line.empty() && ref_line.back() == '\r') {
      ref_line.pop_back();
    }

    bool got_gen = static_cast<bool>(std::getline(ss, gen_line));
    if (!got_gen) {
      std::cerr << "Gen stream ended early at line " << line_num << std::endl;
      assert(false);
    }
    if (!gen_line.empty() && gen_line.back() == '\r') {
      gen_line.pop_back();
    }

    if (ref_line != gen_line) {
      std::cerr << "Mismatch at line " << line_num << std::endl;
      std::cerr << "Ref: [" << ref_line << "]" << std::endl;
      std::cerr << "Gen: [" << gen_line << "]" << std::endl;
      assert(false);
    }
    line_num++;
  }
  // Ensure ss is also exhausted
  bool got_extra = static_cast<bool>(std::getline(ss, gen_line));
  if (got_extra) {
    std::cerr << "Gen has extra lines starting with: [" << gen_line << "]"
              << std::endl;
    assert(false);
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
