#include <algorithm>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "src/jpi_decoder.h"
#include "src/jpi_stream.h"
#include "src/csv_writer.h"

namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <jpi_file> <output_directory>"
              << std::endl;
    return 1;
  }

  std::string jpi_filename = argv[1];
  fs::path output_dir = argv[2];

  try {
    if (!fs::exists(output_dir)) {
      if (!fs::create_directories(output_dir)) {
        throw std::runtime_error("Could not create output directory: " +
                                 output_dir.string());
      }
    }

    jpireader::JpiStream stream(jpi_filename);
    jpireader::JpiFile jpi_file = jpireader::JpiDecoder::Decode(stream);

    // Calculate global maxes for consistent headers
    int max_engines = 0;
    int max_egt = 0;
    int max_cht = 0;
    for (const auto& flight : jpi_file.flights) {
      for (const auto& record : flight.data) {
        max_engines =
            std::max(max_engines, static_cast<int>(record.engine.size()));
        for (const auto& engine : record.engine) {
          max_egt = std::max(
              max_egt, static_cast<int>(engine.exhaust_gas_temperature.size()));
          max_cht = std::max(
              max_cht,
              static_cast<int>(engine.cylinder_head_temperature.size()));
        }
      }
    }
    max_engines = std::max(max_engines, 1);

    std::cout << "Processing " << jpi_file.flights.size() << " flights from "
              << jpi_filename << "..." << std::endl;

    bool is_oat_fahrenheit = false;
    if (jpi_file.metadata.features.oat_temperature_unit ==
        jpireader::TemperatureUnit::FAHRENHEIT) {
      is_oat_fahrenheit = true;
    }
    for (const auto& flight : jpi_file.flights) {
      for (const auto& record : flight.data) {
        if (record.outside_air_temperature &&
            *record.outside_air_temperature > 40) {
          is_oat_fahrenheit = true;
          break;
        }
      }
    }

    for (const auto& flight : jpi_file.flights) {
      std::string out_filename =
          "flight_" + std::to_string(flight.flight_number) + ".csv";
      fs::path out_path = output_dir / out_filename;

      std::ofstream ofs(out_path);
      if (!ofs) {
        std::cerr << "Warning: Could not open file for writing: " << out_path
                  << std::endl;
        continue;
      }

      if (jpi_file.metadata.registration == "N111XX") {
        PrintFlightCsvCustom(ofs, flight);
      } else if (jpi_file.metadata.registration == "edm830") {
        PrintFlightCsvEdm830(ofs, flight);
      } else {
        PrintFlightCsv(ofs, flight, max_engines, max_egt, max_cht,
                       is_oat_fahrenheit);
      }
      std::cout << "  Wrote " << out_path << " (" << flight.data.size()
                << " records)" << std::endl;
    }

    std::cout << "Done." << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
