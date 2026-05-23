#ifndef JPIREADER_DATA_RECORD_PARSER_H_
#define JPIREADER_DATA_RECORD_PARSER_H_

#include "jpi_types.h"
#include "jpi_stream.h"
#include "metadata_util.h"
#include "metric.h"
#include "bit_set.h"
#include <map>
#include <vector>

namespace jpireader {

class DataRecordParser {
 public:
  DataRecordParser(const MetadataUtil& metadata_util, JpiStream& stream);
  DataRecord Parse(const DataRecord* previous_record);
  int GetPreviousRecordRepeatCount() const;

 private:
  void UpdateProtoValue(DataRecord& record, int bit_index, int value);
  float GetExistingValueOrDefault(const DataRecord& record, const Metric& metric);
  std::vector<int> GetBitIndexesFromMasks(DataRecord& record);
  void CalculateExhaustGasTemperatureMaxDiffs(DataRecord& record);

  const MetadataUtil& metadata_util_;
  JpiStream& stream_;
  std::map<int, const Metric*> handlers_;
  int previous_record_repeat_count_ = 0;
  BitSet value_flags_;
  BitSet sign_flags_;
  std::map<const Metric*, float> na_values_;
};

}  // namespace jpireader

#endif  // JPIREADER_DATA_RECORD_PARSER_H_
