#ifndef SRC_JPI_STREAM_H_
#define SRC_JPI_STREAM_H_

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jpireader {

class JpiStream {
 public:
  explicit JpiStream(const std::string& filename);
  ~JpiStream();

  uint8_t Read();
  uint16_t ReadWord();
  void Skip(size_t num_bytes);
  std::vector<uint8_t> Peek(size_t num_bytes);
  void ResetCounter();
  int GetCounter() const;
  void ClearCurrentRecord();
  int GetCurrentRecordSize() const;
  std::string GetCurrentRecordAsHexString() const;
  std::optional<std::string> GetChecksumFailureMessage();

 private:
  std::ifstream stream_;
  std::vector<uint8_t> current_record_;
  int counter_ = 0;
};

}  // namespace jpireader

#endif  // SRC_JPI_STREAM_H_
