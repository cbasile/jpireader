#ifndef JPIREADER_METRICS_H_
#define JPIREADER_METRICS_H_

#include "metric.h"
#include <map>
#include <vector>

namespace jpireader {

class Metrics {
 public:
  static std::map<int, const Metric*> GetBitToMetricMap(const MetadataUtil& util);

 private:
  static int GetVersionSelector(const MetadataUtil& util);

  static const int V1 = 0x1;
  static const int V2 = 0x2;
  static const int V3 = 0x4;
  static const int V4 = 0x8;
  static const int V5 = 0x10;

  static const std::vector<Metric> METRICS;
};

}  // namespace jpireader

#endif  // JPIREADER_METRICS_H_
