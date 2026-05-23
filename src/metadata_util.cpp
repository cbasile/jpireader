#include "src/metadata_util.h"

#include <stdexcept>

namespace jpireader {

MetadataUtil::MetadataUtil(const Metadata& metadata) : metadata_(metadata) {
  for (const auto& flight : metadata.flight_metadata) {
    flight_numbers_.push_back(flight.flight_number);
  }
}

bool MetadataUtil::HasProtocolHeader() const {
  return metadata_.protocol_version > 0;
}

bool MetadataUtil::HasExtraFlightHeaderConfiguration() const {
  return HasProtocolHeader() || IsModelNumberAtLeast(900);
}

bool MetadataUtil::IsDecodeMaskSingleByte() const {
  return !HasProtocolHeader() && !IsModelNumberAtLeast(900);
}

bool MetadataUtil::IsModelNumber(int model_number) const {
  return metadata_.features.model_number.value_or(0) == model_number;
}

bool MetadataUtil::IsModelNumberAtLeast(int model_number) const {
  return metadata_.features.model_number.value_or(0) >= model_number;
}

bool MetadataUtil::IsBuildNumberAtLeast(int build_number) const {
  return metadata_.features.build_number.value_or(0) >= build_number;
}

bool MetadataUtil::IsFirmwareVersionAtLeast(int version_number) const {
  return metadata_.features.firmware_version.value_or(0) >= version_number;
}

bool MetadataUtil::IsTwinEngine() const {
  return IsModelNumber(760) || IsModelNumber(960);
}

bool MetadataUtil::IsGallonsPerHour() const {
  return metadata_.fuel.fuel_flow_units.value_or(FuelFlowUnits::GPH) ==
         FuelFlowUnits::GPH;
}

int MetadataUtil::GetNextFlightNumber(int flight_number) const {
  auto it =
      std::find(flight_numbers_.begin(), flight_numbers_.end(), flight_number);
  if (it == flight_numbers_.end() || std::next(it) == flight_numbers_.end()) {
    throw std::runtime_error("No next flight number");
  }
  return *std::next(it);
}

bool MetadataUtil::IsLastFlight(int flight_number) const {
  if (flight_numbers_.empty()) return true;
  return flight_numbers_.back() == flight_number;
}

int64_t MetadataUtil::Timegm(const std::tm& tm) {
  static const int days_before_month[] = {0,   31,  59,  90,  120, 151,
                                          181, 212, 243, 273, 304, 334};

  int year = tm.tm_year + 1900;
  int month = tm.tm_mon;     // 0-11
  int day = tm.tm_mday - 1;  // 0-based

  // Calculate leap years before this year
  int leap_days = (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400 -
                  (1969 / 4 - 1969 / 100 + 1969 / 400);

  int64_t epoch_days =
      (year - 1970) * 365 + leap_days + days_before_month[month] + day;

  // Add a leap day for the current year if we are past February and it's a leap
  // year
  if (month > 1) {
    bool is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (is_leap) {
      epoch_days += 1;
    }
  }

  return epoch_days * 86400 + tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;
}

}  // namespace jpireader
