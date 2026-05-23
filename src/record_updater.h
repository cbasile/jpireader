#ifndef JPIREADER_RECORD_UPDATER_H_
#define JPIREADER_RECORD_UPDATER_H_

#include "jpi_types.h"
#include <string>
#include <vector>

namespace jpireader {

class RecordUpdater {
 public:
  static void Update(DataRecord& record, const std::string& path, float delta);
  static void Clear(DataRecord& record, const std::string& path);
  static bool HasField(const DataRecord& record, const std::string& path);
  static float GetValue(const DataRecord& record, const std::string& path);

 private:
  struct PathInfo {
    std::string component;
    int index = -1;
  };
  static std::vector<PathInfo> ParsePath(const std::string& path);
};

}  // namespace jpireader

#endif  // JPIREADER_RECORD_UPDATER_H_
