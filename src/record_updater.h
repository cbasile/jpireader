#ifndef JPIREADER_RECORD_UPDATER_H_
#define JPIREADER_RECORD_UPDATER_H_

#include "jpi_types.h"
#include "metric_id.h"

namespace jpireader {

class RecordUpdater {
 public:
  static void Update(DataRecord& record, MetricId id, int engine_idx,
                     int field_idx, float delta);
  static void Clear(DataRecord& record, MetricId id, int engine_idx,
                    int field_idx);
  static bool HasField(const DataRecord& record, MetricId id, int engine_idx,
                       int field_idx);
  static float GetValue(const DataRecord& record, MetricId id, int engine_idx,
                        int field_idx);
};

}  // namespace jpireader

#endif  // JPIREADER_RECORD_UPDATER_H_
