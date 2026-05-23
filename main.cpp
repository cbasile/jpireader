#include "src/jpi_stream.h"
#include "src/jpi_decoder.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void PrintFlightCsv(std::ostream& os, const jpireader::Flight& flight, int max_egt, int max_cht) {
    // Header
    os << "Flight,Timestamp,Interval,Voltage,OAT";
    for (int i = 0; i < max_egt; ++i) os << ",EGT" << (i + 1);
    for (int i = 0; i < max_cht; ++i) os << ",CHT" << (i + 1);
    os << ",RPM,MAP,FuelFlow,FuelUsed,OilTemp,OilPress";
    os << std::endl;

    int64_t current_time = flight.start_timestamp;
    for (const auto& record : flight.data) {
        os << flight.flight_number << ",";
        os << current_time << ",";
        os << flight.recording_interval_secs << ",";
        
        // Voltage
        if (!record.voltage.empty()) os << record.voltage[0];
        os << ",";

        // OAT
        if (record.outside_air_temperature) os << *record.outside_air_temperature;
        os << ",";

        // Engine Data
        if (!record.engine.empty()) {
            const auto& engine = record.engine[0];
            
            // EGT
            for (int i = 0; i < max_egt; ++i) {
                if (i < (int)engine.exhaust_gas_temperature.size()) os << engine.exhaust_gas_temperature[i];
                os << ",";
            }

            // CHT
            for (int i = 0; i < max_cht; ++i) {
                if (i < (int)engine.cylinder_head_temperature.size()) os << engine.cylinder_head_temperature[i];
                os << ",";
            }

            // Other Engine metrics
            if (engine.rpm) os << *engine.rpm;
            os << ",";
            if (engine.manifold_pressure) os << *engine.manifold_pressure;
            os << ",";
            if (!engine.fuel_flow.empty()) os << engine.fuel_flow[0];
            os << ",";
            if (!engine.fuel_used.empty()) os << engine.fuel_used[0];
            os << ",";
            if (engine.oil_temperature) os << *engine.oil_temperature;
            os << ",";
            if (engine.oil_pressure) os << *engine.oil_pressure;
        } else {
            for (int i = 0; i < max_egt + max_cht + 6; ++i) os << ",";
        }

        os << std::endl;
        current_time += flight.recording_interval_secs;
    }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <jpi_file> <output_directory>" << std::endl;
    return 1;
  }

  std::string jpi_filename = argv[1];
  fs::path output_dir = argv[2];

  try {
    if (!fs::exists(output_dir)) {
        if (!fs::create_directories(output_dir)) {
            throw std::runtime_error("Could not create output directory: " + output_dir.string());
        }
    }

    jpireader::JpiStream stream(jpi_filename);
    jpireader::JpiFile jpi_file = jpireader::JpiDecoder::Decode(stream);

    // Calculate global maxes for consistent headers
    int max_egt = 0;
    int max_cht = 0;
    for (const auto& flight : jpi_file.flights) {
        for (const auto& record : flight.data) {
            for (const auto& engine : record.engine) {
                max_egt = std::max(max_egt, (int)engine.exhaust_gas_temperature.size());
                max_cht = std::max(max_cht, (int)engine.cylinder_head_temperature.size());
            }
        }
    }

    std::cout << "Processing " << jpi_file.flights.size() << " flights from " << jpi_filename << "..." << std::endl;

    for (const auto& flight : jpi_file.flights) {
        std::string out_filename = "flight_" + std::to_string(flight.flight_number) + ".csv";
        fs::path out_path = output_dir / out_filename;
        
        std::ofstream ofs(out_path);
        if (!ofs) {
            std::cerr << "Warning: Could not open file for writing: " << out_path << std::endl;
            continue;
        }
        
        PrintFlightCsv(ofs, flight, max_egt, max_cht);
        std::cout << "  Wrote " << out_path << " (" << flight.data.size() << " records)" << std::endl;
    }

    std::cout << "Done." << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
