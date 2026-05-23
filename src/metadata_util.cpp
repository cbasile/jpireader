#include "metadata_util.h"
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
  return metadata_.fuel.fuel_flow_units.value_or(FuelFlowUnits::GPH) == FuelFlowUnits::GPH;
}

int MetadataUtil::GetNextFlightNumber(int flight_number) const {
  auto it = std::find(flight_numbers_.begin(), flight_numbers_.end(), flight_number);
  if (it == flight_numbers_.end() || std::next(it) == flight_numbers_.end()) {
    throw std::runtime_error("No next flight number");
  }
  return *std::next(it);
}

bool MetadataUtil::IsLastFlight(int flight_number) const {
  if (flight_numbers_.empty()) return true;
  return flight_numbers_.back() == flight_number;
}

}  // namespace jpireader
