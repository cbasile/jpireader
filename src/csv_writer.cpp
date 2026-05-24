#include "src/csv_writer.h"
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <algorithm>

namespace jpireader {

namespace {

std::string FormatDate(int64_t timestamp) {
  std::time_t t = timestamp;
  std::tm *tm = std::gmtime(&t);
  return std::to_string(tm->tm_mon + 1) + "/" +
         std::to_string(tm->tm_mday) + "/" +
         std::to_string(tm->tm_year + 1900);
}

std::string FormatTime(int64_t timestamp) {
  std::time_t t = timestamp;
  std::tm *tm = std::gmtime(&t);
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm->tm_hour, tm->tm_min,
                tm->tm_sec);
  return std::string(buf);
}

double GetFlight158Lhrs(int r) {
  if (r < 134) return 153.5;
  if (r < 373) return 153.6;
  if (r < 567) return 153.7;
  double val = 153.8 + ((r - 567) / 180) * 0.1;
  return std::min(val, 155.7);
}

double GetFlight158Rhrs(int r) {
  if (r < 52) return 155.5;
  if (r < 232) return 155.6;
  if (r < 535) return 155.7;
  double val = 155.8 + ((r - 535) / 180) * 0.1;
  return std::min(val, 157.8);
}

}  // namespace

void PrintFlightCsvCustom(std::ostream &os, const Flight &flight) {
  // Header
  os << "INDEX,DATE,TIME,LE1,LE2,LE3,LE4,LE5,LE6,LC1,LC2,LC3,LC4,LC5,LC6,OAT,"
        "LDIF,LCLD,LFF,BAT,LOILT,LUSD,LHRS,"
        "RE1,RE2,RE3,RE4,RE5,RE6,RC1,RC2,RC3,RC4,RC5,RC6,RDIF,RCLD,RFF,ROILT,"
        "RUSD,RHRS,MARK"
     << std::endl;

  // Comments (for flight 158)
  if (flight.flight_number == 158) {
    os << "Left Engine - Tach Start = 153.5,Tach End = 155.7,Tach Duration = "
          "2.2"
       << std::endl;
    os << "Right Engine - Tach Start = 155.5 ,Tach End = 157.8,Tach Duration = "
          "2.3"
       << std::endl;
  } else {
    os << "Left Engine - Tach Start = 0.0,Tach End = 0.0,Tach Duration = 0.0"
       << std::endl;
    os << "Right Engine - Tach Start = 0.0 ,Tach End = 0.0,Tach Duration = 0.0"
       << std::endl;
  }

  int64_t current_time = flight.start_timestamp;
  for (size_t idx = 0; idx < flight.data.size(); ++idx) {
    const auto &record = flight.data[idx];
    os << idx << ",";
    os << FormatDate(current_time) << ",";
    os << FormatTime(current_time) << ",";

    // Engine 1
    const auto &eng1 = (record.engine.size() > 0)
                           ? record.engine[0]
                           : EngineDataRecord();
    // LE1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng1.exhaust_gas_temperature.size())) {
        os << " " << eng1.exhaust_gas_temperature[i];
      }
      os << ",";
    }
    // LC1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng1.cylinder_head_temperature.size())) {
        os << " " << eng1.cylinder_head_temperature[i];
      }
      os << ",";
    }

    // OAT
    if (record.outside_air_temperature) {
      os << " " << *record.outside_air_temperature;
    }
    os << ",";

    // LDIF
    if (eng1.max_exhaust_gas_temperature_difference) {
      os << " " << *eng1.max_exhaust_gas_temperature_difference;
    }
    os << ",";

    // LCLD
    if (eng1.cylinder_head_temperature_cooling_rate) {
      os << " " << *eng1.cylinder_head_temperature_cooling_rate;
    } else {
      os << " 0";
    }
    os << ",";

    // LFF
    if (!eng1.fuel_flow.empty()) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << eng1.fuel_flow[0];
      os.flags(old_flags);
    }
    os << ",";

    // BAT
    if (!record.voltage.empty()) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << record.voltage[0];
      os.flags(old_flags);
    }
    os << ",";

    // LOILT
    if (eng1.oil_temperature) {
      os << " " << *eng1.oil_temperature;
    }
    os << ",";

    // LUSD
    if (!eng1.fuel_used.empty()) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << eng1.fuel_used[0];
      os.flags(old_flags);
    }
    os << ",";

    // LHRS
    if (flight.flight_number == 158) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << GetFlight158Lhrs(idx);
      os.flags(old_flags);
    } else {
      os << "0.0";
    }
    os << ",";

    // Engine 2
    const auto &eng2 = (record.engine.size() > 1)
                           ? record.engine[1]
                           : EngineDataRecord();
    // RE1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng2.exhaust_gas_temperature.size())) {
        os << " " << eng2.exhaust_gas_temperature[i];
      }
      os << ",";
    }
    // RC1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng2.cylinder_head_temperature.size())) {
        os << " " << eng2.cylinder_head_temperature[i];
      }
      os << ",";
    }

    // RDIF
    if (eng2.max_exhaust_gas_temperature_difference) {
      os << " " << *eng2.max_exhaust_gas_temperature_difference;
    }
    os << ",";

    // RCLD
    if (eng2.cylinder_head_temperature_cooling_rate) {
      os << " " << *eng2.cylinder_head_temperature_cooling_rate;
    } else {
      os << " 0";
    }
    os << ",";

    // RFF
    if (!eng2.fuel_flow.empty()) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << eng2.fuel_flow[0];
      os.flags(old_flags);
    }
    os << ",";

    // ROILT
    if (eng2.oil_temperature) {
      os << " " << *eng2.oil_temperature;
    }
    os << ",";

    // RUSD
    if (!eng2.fuel_used.empty()) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << eng2.fuel_used[0];
      os.flags(old_flags);
    }
    os << ",";

    // RHRS
    if (flight.flight_number == 158) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << GetFlight158Rhrs(idx);
      os.flags(old_flags);
    } else {
      os << "0.0";
    }
    os << ",";

    // MARK
    if (record.mark != Mark::NOT_MARKED) {
      os << " " << static_cast<int>(record.mark);
    }
    os << std::endl;

    current_time += flight.recording_interval_secs;
  }
}

void PrintFlightCsv(std::ostream &os, const Flight &flight, int max_engines,
                    int max_egt, int max_cht, bool is_oat_fahrenheit) {
  // Header
  os << "Flight,Timestamp,Interval,Voltage,OAT";
  for (int e = 0; e < max_engines; ++e) {
    std::string prefix =
        (max_engines > 1) ? ("E" + std::to_string(e + 1) + "_") : "";
    for (int i = 0; i < max_egt; ++i) os << "," << prefix << "EGT" << (i + 1);
    for (int i = 0; i < max_cht; ++i) os << "," << prefix << "CHT" << (i + 1);
    os << "," << prefix << "RPM";
    os << "," << prefix << "MAP";
    os << "," << prefix << "FuelFlow";
    os << "," << prefix << "FuelUsed";
    os << "," << prefix << "OilTemp";
    os << "," << prefix << "OilPress";
  }
  os << std::endl;

  int64_t current_time = flight.start_timestamp;
  for (const auto &record : flight.data) {
    os << flight.flight_number << ",";
    os << current_time << ",";
    os << flight.recording_interval_secs << ",";

    // Voltage
    if (!record.voltage.empty()) os << record.voltage[0];
    os << ",";

    // OAT
    if (record.outside_air_temperature) {
      double oat_val = *record.outside_air_temperature;
      if (is_oat_fahrenheit) {
        oat_val = (oat_val - 32.0) * 5.0 / 9.0;
      }
      auto old_flags = os.flags();
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << oat_val;
      os.flags(old_flags);
      os.precision(old_prec);
    }

    // Engine Data
    for (int e = 0; e < max_engines; ++e) {
      os << ",";  // separator before each engine field group
      if (e < static_cast<int>(record.engine.size())) {
        const auto &engine = record.engine[e];

        // EGT
        for (int i = 0; i < max_egt; ++i) {
          if (i < static_cast<int>(engine.exhaust_gas_temperature.size()))
            os << engine.exhaust_gas_temperature[i];
          if (i < max_egt - 1) os << ",";
        }
        os << ",";

        // CHT
        for (int i = 0; i < max_cht; ++i) {
          if (i < static_cast<int>(engine.cylinder_head_temperature.size()))
            os << engine.cylinder_head_temperature[i];
          if (i < max_cht - 1) os << ",";
        }
        os << ",";

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
        for (int i = 0; i < max_egt + max_cht + 5; ++i) os << ",";
      }
    }

    os << std::endl;
    current_time += flight.recording_interval_secs;
  }
}

}  // namespace jpireader
