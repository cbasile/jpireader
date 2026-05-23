#ifndef JPIREADER_METRIC_H_
#define JPIREADER_METRIC_H_

#include "metadata_util.h"
#include <string>
#include <optional>
#include <functional>

namespace jpireader {

enum class ScaleFactor {
  TEN,
  TEN_IF_GPH
};

struct Metric {
  int version_mask;
  int low_byte_bit;
  std::optional<int> high_byte_bit;
  std::string proto_path;
  std::optional<ScaleFactor> scale_factor;

  Metric(int v, int low, std::string path)
      : version_mask(v), low_byte_bit(low), proto_path(path) {}
  Metric(int v, int low, int high, std::string path)
      : version_mask(v), low_byte_bit(low), high_byte_bit(high), proto_path(path) {}
  Metric(int v, int low, int high, std::string path, ScaleFactor scale)
      : version_mask(v), low_byte_bit(low), high_byte_bit(high), proto_path(path), scale_factor(scale) {}
  Metric(int v, int low, std::string path, ScaleFactor scale)
      : version_mask(v), low_byte_bit(low), proto_path(path), scale_factor(scale) {}

  float Scale(const MetadataUtil& util, float value) const {
    if (!scale_factor) return value;
    if (*scale_factor == ScaleFactor::TEN_IF_GPH && !util.IsGallonsPerHour()) return value;
    return value / 10.0f;
  }

  bool IsHighByteBit(int index) const {
    return high_byte_bit.has_value() && *high_byte_bit == index;
  }
};

}  // namespace jpireader

#endif  // JPIREADER_METRIC_H_
