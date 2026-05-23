#ifndef JPIREADER_METADATA_UTIL_H_
#define JPIREADER_METADATA_UTIL_H_

#include "jpi_types.h"
#include <vector>
#include <algorithm>

namespace jpireader {

class MetadataUtil {
 public:
  explicit MetadataUtil(const Metadata& metadata);

  bool HasProtocolHeader() const;
  bool HasExtraFlightHeaderConfiguration() const;
  bool IsDecodeMaskSingleByte() const;
  bool IsModelNumber(int model_number) const;
  bool IsModelNumberAtLeast(int model_number) const;
  bool IsBuildNumberAtLeast(int build_number) const;
  bool IsFirmwareVersionAtLeast(int version_number) const;
  bool IsTwinEngine() const;
  bool IsGallonsPerHour() const;
  int GetNextFlightNumber(int flight_number) const;
  bool IsLastFlight(int flight_number) const;

 private:
  const Metadata& metadata_;
  std::vector<int> flight_numbers_;
};

}  // namespace jpireader

#endif  // JPIREADER_METADATA_UTIL_H_
