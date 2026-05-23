#ifndef SRC_METADATA_UTIL_H_
#define SRC_METADATA_UTIL_H_

#include <algorithm>
#include <ctime>
#include <vector>

#include "src/jpi_types.h"

namespace jpireader {

class MetadataUtil {
 public:
  explicit MetadataUtil(const Metadata& metadata);

  static int64_t Timegm(const std::tm& tm);

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

#endif  // SRC_METADATA_UTIL_H_
