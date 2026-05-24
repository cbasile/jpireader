#include "data_record_parser.h"

#include <algorithm>
#include <stdexcept>

#include "metrics.h"
#include "record_updater.h"

namespace jpireader {

const int NOT_AVAILABLE_VALUE_MARKER = 0;
const int MAX_NUM_VALUE_BYTES = 16;
const int DEFAULT_VALUE = 240;

DataRecordParser::DataRecordParser(const MetadataUtil& metadata_util,
                                   JpiStream& stream)
    : metadata_util_(metadata_util),
      stream_(stream),
      value_flags_(MAX_NUM_VALUE_BYTES),
      sign_flags_(MAX_NUM_VALUE_BYTES) {
  handlers_ = Metrics::GetBitToMetricMap(metadata_util);
}

DataRecord DataRecordParser::Parse(const DataRecord* previous_record) {
  value_flags_.Clear();
  sign_flags_.Clear();
  previous_record_repeat_count_ = 0;
  stream_.ClearCurrentRecord();

  DataRecord record;
  if (previous_record) {
    record = *previous_record;
  }
  record.parse_warnings.clear();

  for (int bit_index : GetBitIndexesFromMasks(record)) {
    UpdateProtoValue(record, bit_index, stream_.Read());
  }
  CalculateExhaustGasTemperatureMaxDiffs(record);

  if (auto msg = stream_.GetChecksumFailureMessage()) {
    record.parse_warnings.push_back(*msg);
  }

  return record;
}

void DataRecordParser::UpdateProtoValue(DataRecord& record, int bit_index,
                                        int value) {
  auto it = handlers_.find(bit_index);
  if (it == handlers_.end()) {
    return;
  }
  const Metric& metric = *it->second;
  if (metric.id == MetricId::kNone) {
    record.parse_warnings.push_back(
        "Unexpected value for unsupported metric at bit " +
        std::to_string(bit_index));
    return;
  }

  if (value == NOT_AVAILABLE_VALUE_MARKER) {
    if (na_values_.find(&metric) == na_values_.end()) {
      na_values_[&metric] = GetExistingValueOrDefault(record, metric);
      RecordUpdater::Clear(record, metric.id, metric.engine_index,
                           metric.field_index);
    }
    return;
  } else if (na_values_.find(&metric) != na_values_.end()) {
    RecordUpdater::Update(record, metric.id, metric.engine_index,
                          metric.field_index, na_values_[&metric]);
    na_values_.erase(&metric);
  }

  if (!RecordUpdater::HasField(record, metric.id, metric.engine_index,
                               metric.field_index)) {
    float default_val = GetExistingValueOrDefault(record, metric);
    RecordUpdater::Update(record, metric.id, metric.engine_index,
                          metric.field_index, default_val);
  }

  int adjusted_value = value;
  if (sign_flags_.TestBit(metric.low_byte_bit)) {
    adjusted_value = -adjusted_value;
  }
  if (metric.IsHighByteBit(bit_index)) {
    adjusted_value <<= 8;
  }
  float new_value =
      metric.Scale(metadata_util_, static_cast<float>(adjusted_value));
  RecordUpdater::Update(record, metric.id, metric.engine_index,
                        metric.field_index, new_value);
}

float DataRecordParser::GetExistingValueOrDefault(const DataRecord& record,
                                                  const Metric& metric) {
  if (RecordUpdater::HasField(record, metric.id, metric.engine_index,
                              metric.field_index)) {
    return RecordUpdater::GetValue(record, metric.id, metric.engine_index,
                                   metric.field_index);
  } else {
    if (metric.id == MetricId::kHorsepower && metric.engine_index == 0) return 0;
    return metric.Scale(metadata_util_, static_cast<float>(DEFAULT_VALUE));
  }
}

int DataRecordParser::GetPreviousRecordRepeatCount() const {
  return previous_record_repeat_count_;
}

std::vector<int> DataRecordParser::GetBitIndexesFromMasks(DataRecord& record) {
  int decode_mask;
  int second_decode_mask;
  if (metadata_util_.IsDecodeMaskSingleByte()) {
    decode_mask = stream_.Read();
    second_decode_mask = stream_.Read();
  } else {
    decode_mask = stream_.ReadWord();
    second_decode_mask = stream_.ReadWord();
  }

  if (decode_mask != second_decode_mask) {
    // This is a common error in JPI files, but we should probably just warn
    record.parse_warnings.push_back(
        "Decode mask mismatch: " + std::to_string(decode_mask) + " vs " +
        std::to_string(second_decode_mask));
  }

  previous_record_repeat_count_ = stream_.Read();

  int num_decode_bits = metadata_util_.IsDecodeMaskSingleByte() ? 8 : 16;
  for (int i = 0; i < num_decode_bits; ++i) {
    if ((decode_mask & (1 << i)) > 0) {
      int next_byte = stream_.Read();
      if (next_byte == 0) {
        record.parse_warnings.push_back("Value byte is 00 at index " +
                                        std::to_string(i));
      }
      value_flags_.SetByte(i, next_byte);
    }
  }

  for (int i = 0; i < num_decode_bits; ++i) {
    if (i != 6 && i != 7 && (decode_mask & (1 << i)) > 0) {
      sign_flags_.SetByte(i, stream_.Read());
    }
  }

  std::vector<int> bit_indexes;
  for (int i = 0; i < value_flags_.NumBits(); ++i) {
    if (value_flags_.TestBit(i)) {
      bit_indexes.push_back(i);
    }
  }
  return bit_indexes;
}

void DataRecordParser::CalculateExhaustGasTemperatureMaxDiffs(
    DataRecord& record) {
  for (auto& engine : record.engine) {
    if (engine.exhaust_gas_temperature.empty()) continue;
    int max_egt = *std::max_element(engine.exhaust_gas_temperature.begin(),
                                    engine.exhaust_gas_temperature.end());
    int min_egt = *std::min_element(engine.exhaust_gas_temperature.begin(),
                                    engine.exhaust_gas_temperature.end());
    engine.max_exhaust_gas_temperature_difference = max_egt - min_egt;
  }
}

}  // namespace jpireader
