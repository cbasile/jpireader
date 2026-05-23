#ifndef JPIREADER_METADATA_PARSER_H_
#define JPIREADER_METADATA_PARSER_H_

#include "jpi_types.h"
#include "jpi_stream.h"
#include <vector>
#include <string>
#include <optional>

namespace jpireader {

class MetadataParser {
 public:
  explicit MetadataParser(JpiStream& stream);
  Metadata Parse();

 private:
  class HeaderStream {
   public:
    explicit HeaderStream(JpiStream& stream);
    std::vector<std::string> NextHeader();
    std::optional<std::string> GetChecksumFailureMessage() const;
    int GetCounter() const;

   private:
    std::string ReadLine();
    JpiStream& stream_;
    std::optional<std::string> current_failure_message_;
  };

  bool ParseHeaderToMetadata(const std::vector<std::string>& header, Metadata& metadata);
  void ParseAlarmThresholds(const std::vector<std::string>& parts, AlarmThresholds& thresholds);
  void ParseFeatures(const std::vector<std::string>& parts, Features& features);
  void ParseFuelConfiguration(const std::vector<std::string>& parts, Fuel& fuel);
  int64_t ParseUnixTimestamp(const std::vector<std::string>& parts);

  HeaderStream header_stream_;
};

}  // namespace jpireader

#endif  // JPIREADER_METADATA_PARSER_H_
