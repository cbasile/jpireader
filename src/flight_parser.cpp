#include "src/flight_parser.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "src/data_record_parser.h"
#include "src/sensor_parser.h"

namespace jpireader {

FlightParser::FlightParser(JpiStream& stream, const FlightMetadata& metadata,
                           const MetadataUtil& metadata_util)
    : stream_(stream),
      flight_number_(metadata.flight_number),
      estimated_flight_length_bytes_(metadata.flight_data_length_words * 2),
      metadata_util_(metadata_util) {}

Flight FlightParser::Parse() {
  Flight flight;
  ParseFlightHeader(flight);
  ParseFlightData(flight);
  return flight;
}

void FlightParser::ParseFlightHeader(Flight& flight) {
  stream_.ResetCounter();
  stream_.ClearCurrentRecord();

  flight.flight_number = stream_.ReadWord();
  if (flight.flight_number != flight_number_) {
    std::stringstream ss;
    ss << "Unexpected flight number " << flight.flight_number << " instead of "
       << flight_number_;
    flight.parse_warnings.push_back(ss.str());
  }

  int low = stream_.ReadWord();
  int high = stream_.ReadWord();
  flight.sensors = SensorParser(low, high).Parse();

  if (metadata_util_.HasExtraFlightHeaderConfiguration()) {
    stream_.ReadWord();  // unusedConfigLow
    stream_.ReadWord();  // unusedConfigHigh
    if (metadata_util_.IsBuildNumberAtLeast(880)) {
      stream_.ReadWord();  // unusedConfig
    }
    if (metadata_util_.IsModelNumber(790)) {
      stream_.ReadWord();  // unknown 790
      stream_.ReadWord();  // unknown 790
      stream_.ReadWord();  // unknown 790
      stream_.ReadWord();  // unknown 790
    }
  }

  stream_.ReadWord();  // unknown
  flight.recording_interval_secs = stream_.ReadWord();

  int packed_date = stream_.ReadWord();
  int packed_time = stream_.ReadWord();
  flight.start_timestamp = ParseUnixTimestamp(packed_date, packed_time);

  if (auto msg = stream_.GetChecksumFailureMessage()) {
    flight.parse_warnings.push_back(*msg);
  }

  flight.header_length = stream_.GetCurrentRecordSize();
}

void FlightParser::ParseFlightData(Flight& flight) {
  DataRecordParser parser(metadata_util_, stream_);
  std::optional<DataRecord> previous_data_record;

  auto get_min_record_size = [&]() {
    return metadata_util_.IsDecodeMaskSingleByte() ? 3 : 5;
  };

  while (stream_.GetCounter() + get_min_record_size() <
         estimated_flight_length_bytes_) {
    try {
      DataRecord data_record =
          parser.Parse(previous_data_record ? &*previous_data_record : nullptr);

      int repeat_count = parser.GetPreviousRecordRepeatCount();
      while (repeat_count-- > 0 && previous_data_record) {
        flight.data.push_back(*previous_data_record);
      }
      flight.data.push_back(data_record);

      flight.data_length = stream_.GetCounter();
      previous_data_record = data_record;
    } catch (const JpiEofException& e) {
      flight.parse_warnings.push_back("Flight data parsing truncated due to EOF: " + std::string(e.what()));
      break;
    }
  }

  // Peek for next flight header
  try {
    if (!metadata_util_.IsLastFlight(flight_number_)) {
      auto peek = stream_.Peek(3);
      if (peek.size() >= 2) {
        int next_flight_number =
            metadata_util_.GetNextFlightNumber(flight_number_);
        if (((peek[0] << 8) | peek[1]) != next_flight_number) {
          if (peek.size() >= 3 &&
              ((peek[1] << 8) | peek[2]) == next_flight_number) {
            stream_.Skip(1);
          }
        }
      }
    }
  } catch (...) {
    // EOF or other error
  }

  flight.data_length = stream_.GetCounter();
}

int64_t FlightParser::ParseUnixTimestamp(int packed_date, int packed_time) {
  int year = (packed_date & 0xfe00) >> 9;
  year += (year >= 75) ? 1900 : 2000;

  std::tm tm = {};
  tm.tm_year = year - 1900;
  tm.tm_mon = ((packed_date & 0x01e0) >> 5) - 1;
  tm.tm_mday = packed_date & 0x001f;
  tm.tm_hour = (packed_time & 0xf800) >> 11;
  tm.tm_min = (packed_time & 0x07e0) >> 5;
  tm.tm_sec = (packed_time & 0x001f) * 2;
  tm.tm_isdst = -1;

  return MetadataUtil::Timegm(tm);
}

}  // namespace jpireader
