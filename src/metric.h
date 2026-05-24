#ifndef JPIREADER_METRIC_H_
#define JPIREADER_METRIC_H_

#include "metadata_util.h"
#include "metric_id.h"
#include <optional>

namespace jpireader {

enum class ScaleFactor {
  TEN,
  TEN_IF_GPH
};

struct Metric {
  int version_mask;
  int low_byte_bit;
  std::optional<int> high_byte_bit;
  MetricId id;
  int engine_index = 0;
  int field_index = 0;
  std::optional<ScaleFactor> scale_factor;

  Metric(int v, int low, MetricId id, int engine_idx = 0, int field_idx = 0)
      : version_mask(v),
        low_byte_bit(low),
        id(id),
        engine_index(engine_idx),
        field_index(field_idx) {}

  Metric(int v, int low, int high, MetricId id, int engine_idx = 0,
         int field_idx = 0)
      : version_mask(v),
        low_byte_bit(low),
        high_byte_bit(high),
        id(id),
        engine_index(engine_idx),
        field_index(field_idx) {}

  Metric(int v, int low, int high, MetricId id, ScaleFactor scale,
         int engine_idx = 0, int field_idx = 0)
      : version_mask(v),
        low_byte_bit(low),
        high_byte_bit(high),
        id(id),
        engine_index(engine_idx),
        field_index(field_idx),
        scale_factor(scale) {}

  Metric(int v, int low, MetricId id, ScaleFactor scale, int engine_idx = 0,
         int field_idx = 0)
      : version_mask(v),
        low_byte_bit(low),
        id(id),
        engine_index(engine_idx),
        field_index(field_idx),
        scale_factor(scale) {}

  float Scale(const MetadataUtil& util, float value) const {
    if (!scale_factor) return value;
    if (*scale_factor == ScaleFactor::TEN_IF_GPH && !util.IsGallonsPerHour())
      return value;
    return value / 10.0f;
  }

  bool IsHighByteBit(int index) const {
    return high_byte_bit.has_value() && *high_byte_bit == index;
  }
};

}  // namespace jpireader

#endif  // JPIREADER_METRIC_H_
