#include "src/csv_writer.h"
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iomanip>

namespace jpireader {

namespace {

std::string FormatDate(int64_t timestamp) {
  std::time_t t = timestamp;
  std::tm *tm = std::gmtime(&t);
  return std::to_string(tm->tm_mon + 1) + "/" + std::to_string(tm->tm_mday) +
         "/" + std::to_string(tm->tm_year + 1900);
}

std::string FormatTime(int64_t timestamp) {
  std::time_t t = timestamp;
  std::tm *tm = std::gmtime(&t);
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm->tm_hour, tm->tm_min,
                tm->tm_sec);
  return std::string(buf);
}

} // namespace

void PrintFlightCsvCustom(std::ostream &os, const Flight &flight) {
  // Header
  os << "INDEX,DATE,TIME,LE1,LE2,LE3,LE4,LE5,LE6,LC1,LC2,LC3,LC4,LC5,LC6,OAT,"
        "LDIF,LCLD,LFF,BAT,LOILT,LUSD,LHRS,"
        "RE1,RE2,RE3,RE4,RE5,RE6,RC1,RC2,RC3,RC4,RC5,RC6,RDIF,RCLD,RFF,ROILT,"
        "RUSD,RHRS,MARK"
     << std::endl;

  // Comments (programmatic tach start/end/duration)
  double l_start = 0.0, l_end = 0.0;
  double r_start = 0.0, r_end = 0.0;
  if (!flight.data.empty()) {
    const auto &front = flight.data.front();
    const auto &back = flight.data.back();
    if (front.engine.size() > 0 && front.engine[0].hours) {
      l_start = *front.engine[0].hours;
    }
    if (back.engine.size() > 0 && back.engine[0].hours) {
      l_end = *back.engine[0].hours;
    }
    if (front.engine.size() > 1 && front.engine[1].hours) {
      r_start = *front.engine[1].hours;
    }
    if (back.engine.size() > 1 && back.engine[1].hours) {
      r_end = *back.engine[1].hours;
    }
  }
  double l_duration = l_end - l_start;
  double r_duration = r_end - r_start;

  auto comment_flags = os.flags();
  os << "Left Engine - Tach Start = " << std::fixed << std::setprecision(1)
     << l_start << ",Tach End = " << l_end << ",Tach Duration = " << l_duration
     << std::endl;
  os << "Right Engine - Tach Start = " << r_start << " ,Tach End = " << r_end
     << ",Tach Duration = " << r_duration << std::endl;
  os.flags(comment_flags);

  int64_t current_time = flight.start_timestamp;
  bool lean_find_active = false;
  for (size_t idx = 0; idx < flight.data.size(); ++idx) {
    const auto &record = flight.data[idx];
    os << idx << ",";
    os << FormatDate(current_time) << ",";
    os << FormatTime(current_time) << ",";

    // Engine 1
    const auto &eng1 =
        (record.engine.size() > 0) ? record.engine[0] : EngineDataRecord();
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
    if (record.engine.size() > 0 && record.engine[0].hours) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << *record.engine[0].hours;
      os.flags(old_flags);
    } else {
      os << "0.0";
    }
    os << ",";

    // Engine 2
    const auto &eng2 =
        (record.engine.size() > 1) ? record.engine[1] : EngineDataRecord();
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
    if (record.engine.size() > 1 && record.engine[1].hours) {
      auto old_flags = os.flags();
      os << std::fixed << std::setprecision(1) << *record.engine[1].hours;
      os.flags(old_flags);
    } else {
      os << "0.0";
    }
    os << ",";

    // MARK
    int mark_val = static_cast<int>(record.mark);
    if (mark_val == 2 || mark_val == 4 || mark_val == 242 || mark_val == 244) {
      os << "[";
      lean_find_active = true;
    } else if (mark_val == 3 || mark_val == 5 || mark_val == 243 ||
               mark_val == 245) {
      os << "]";
      lean_find_active = false;
    } else if (record.mark != Mark::NOT_MARKED) {
      int base_mark = mark_val % 240;
      if (base_mark != 0) {
        os << " " << base_mark;
      }
    }
    os << std::endl;

    if (lean_find_active) {
      current_time += 1;
    } else {
      current_time += flight.recording_interval_secs;
    }
  }
}

void PrintFlightCsv(std::ostream &os, const Flight &flight, int max_engines,
                    int max_egt, int max_cht, bool is_oat_fahrenheit) {
  // Header
  os << "Flight,Timestamp,Interval,Voltage,OAT";
  for (int e = 0; e < max_engines; ++e) {
    std::string prefix =
        (max_engines > 1) ? ("E" + std::to_string(e + 1) + "_") : "";
    for (int i = 0; i < max_egt; ++i)
      os << "," << prefix << "EGT" << (i + 1);
    for (int i = 0; i < max_cht; ++i)
      os << "," << prefix << "CHT" << (i + 1);
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
    if (!record.voltage.empty())
      os << record.voltage[0];
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
      os << ","; // separator before each engine field group
      if (e < static_cast<int>(record.engine.size())) {
        const auto &engine = record.engine[e];

        // EGT
        for (int i = 0; i < max_egt; ++i) {
          if (i < static_cast<int>(engine.exhaust_gas_temperature.size()))
            os << engine.exhaust_gas_temperature[i];
          if (i < max_egt - 1)
            os << ",";
        }
        os << ",";

        // CHT
        for (int i = 0; i < max_cht; ++i) {
          if (i < static_cast<int>(engine.cylinder_head_temperature.size()))
            os << engine.cylinder_head_temperature[i];
          if (i < max_cht - 1)
            os << ",";
        }
        os << ",";

        // Other Engine metrics
        if (engine.rpm)
          os << *engine.rpm;
        os << ",";
        if (engine.manifold_pressure)
          os << *engine.manifold_pressure;
        os << ",";
        if (!engine.fuel_flow.empty())
          os << engine.fuel_flow[0];
        os << ",";
        if (!engine.fuel_used.empty())
          os << engine.fuel_used[0];
        os << ",";
        if (engine.oil_temperature)
          os << *engine.oil_temperature;
        os << ",";
        if (engine.oil_pressure)
          os << *engine.oil_pressure;
      } else {
        for (int i = 0; i < max_egt + max_cht + 5; ++i)
          os << ",";
      }
    }

    os << std::endl;
    current_time += flight.recording_interval_secs;
  }
}

void PrintFlightCsvEdm830(std::ostream &os, const Flight &flight) {
  // Header
  os << "INDEX,DATE,TIME,E1,E2,E3,E4,E5,E6,C1,C2,C3,C4,C5,C6,OAT,DIF,CLD,MAP,"
        "RPM,"
        "HP,FF,FF2,OILP,BAT,OILT,USD,USD2,HRS,MARK"
     << std::endl;

  double start_hrs = 0.0;
  double end_hrs = 0.0;
  if (!flight.data.empty()) {
    if (!flight.data.front().engine.empty() &&
        flight.data.front().engine[0].hours) {
      start_hrs = *flight.data.front().engine[0].hours;
    }
    if (!flight.data.back().engine.empty() &&
        flight.data.back().engine[0].hours) {
      end_hrs = *flight.data.back().engine[0].hours;
    }
  }
  double duration = end_hrs - start_hrs;

  auto old_flags = os.flags();
  os << "Engine - Tach Start = " << std::fixed << std::setprecision(1)
     << start_hrs << ",Tach End = " << end_hrs
     << ",Tach Duration = " << duration << std::endl;
  os.flags(old_flags);

  int64_t current_time = flight.start_timestamp;
  bool lean_find_active = false;

  for (size_t idx = 0; idx < flight.data.size(); ++idx) {
    const auto &record = flight.data[idx];
    os << idx << ",";
    os << FormatDate(current_time) << ",";
    os << FormatTime(current_time) << ",";

    const auto &eng =
        (record.engine.size() > 0) ? record.engine[0] : EngineDataRecord();

    // E1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng.exhaust_gas_temperature.size())) {
        os << " " << eng.exhaust_gas_temperature[i];
      }
      os << ",";
    }

    // C1..6
    for (int i = 0; i < 6; ++i) {
      if (i < static_cast<int>(eng.cylinder_head_temperature.size())) {
        os << " " << eng.cylinder_head_temperature[i];
      }
      os << ",";
    }

    // OAT
    if (record.outside_air_temperature) {
      os << " " << *record.outside_air_temperature;
    }
    os << ",";

    // DIF
    if (eng.max_exhaust_gas_temperature_difference) {
      os << " " << *eng.max_exhaust_gas_temperature_difference;
    }
    os << ",";

    // CLD
    if (eng.cylinder_head_temperature_cooling_rate) {
      os << " " << *eng.cylinder_head_temperature_cooling_rate;
    } else {
      os << " 0";
    }
    os << ",";

    // MAP
    if (eng.manifold_pressure) {
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << *eng.manifold_pressure;
      os.precision(old_prec);
    }
    os << ",";

    // RPM
    if (eng.rpm) {
      os << " " << *eng.rpm;
    }
    os << ",";

    // HP
    if (eng.horsepower) {
      os << " " << *eng.horsepower;
    }
    os << ",";

    // FF
    if (!eng.fuel_flow.empty()) {
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << eng.fuel_flow[0];
      os.precision(old_prec);
    }
    os << ",";

    // FF2
    os << "NA,";

    // OILP
    if (eng.oil_pressure) {
      os << " " << *eng.oil_pressure;
    }
    os << ",";

    // BAT
    if (!record.voltage.empty()) {
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << record.voltage[0];
      os.precision(old_prec);
    }
    os << ",";

    // OILT
    if (eng.oil_temperature) {
      os << " " << *eng.oil_temperature;
    }
    os << ",";

    // USD
    if (!eng.fuel_used.empty()) {
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << eng.fuel_used[0];
      os.precision(old_prec);
    }
    os << ",";

    // USD2
    os << "NA,";

    // HRS
    if (eng.hours) {
      auto old_prec = os.precision();
      os << std::fixed << std::setprecision(1) << *eng.hours;
      os.precision(old_prec);
    } else {
      os << "0.0";
    }
    os << ",";

    // MARK
    int mark_val = static_cast<int>(record.mark);
    if (mark_val == 2 || mark_val == 4 || mark_val == 242 || mark_val == 244) {
      os << "[";
      lean_find_active = true;
    } else if (mark_val == 3 || mark_val == 5 || mark_val == 243 ||
               mark_val == 245) {
      os << "]";
      lean_find_active = false;
    }
    os << std::endl;

    if (lean_find_active) {
      current_time += 1;
    } else {
      current_time += flight.recording_interval_secs;
    }
  }
}

} // namespace jpireader
