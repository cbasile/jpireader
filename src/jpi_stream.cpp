#include "src/jpi_stream.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace jpireader {

JpiStream::JpiStream(const std::string &filename)
    : stream_(filename, std::ios::binary) {
  if (!stream_) {
    throw std::runtime_error("Could not open file: " + filename);
  }
}

JpiStream::~JpiStream() {
  if (stream_.is_open()) {
    stream_.close();
  }
}

uint8_t JpiStream::Read() {
  int c = stream_.get();
  if (c == EOF) {
    throw JpiEofException("Unexpected EOF at counter " +
                          std::to_string(counter_));
  }
  uint8_t b = static_cast<uint8_t>(c);
  counter_++;
  current_record_.push_back(b);
  return b;
}

uint16_t JpiStream::ReadWord() {
  uint16_t high = Read();
  uint16_t low = Read();
  return (high << 8) | low;
}

void JpiStream::Skip(size_t num_bytes) {
  stream_.seekg(num_bytes, std::ios::cur);
  counter_ += num_bytes;
}

std::vector<uint8_t> JpiStream::Peek(size_t num_bytes) {
  std::streampos pos = stream_.tellg();
  std::vector<uint8_t> buffer(num_bytes);
  stream_.read(reinterpret_cast<char *>(buffer.data()), num_bytes);
  if (stream_.gcount() < static_cast<std::streamsize>(num_bytes)) {
    buffer.resize(stream_.gcount());
  }
  if (stream_.fail() || stream_.eof()) {
    stream_.clear();
  }
  stream_.seekg(pos);
  return buffer;
}

void JpiStream::ResetCounter() { counter_ = 0; }

int JpiStream::GetCounter() const { return counter_; }

void JpiStream::ClearCurrentRecord() { current_record_.clear(); }

int JpiStream::GetCurrentRecordSize() const { return current_record_.size(); }

std::string JpiStream::GetCurrentRecordAsHexString() const {
  std::stringstream ss;
  for (size_t i = 0; i < current_record_.size(); ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(current_record_[i]);
    if (i < current_record_.size() - 1) {
      ss << " ";
    }
  }
  return ss.str();
}

std::optional<std::string> JpiStream::GetChecksumFailureMessage() {
  // Read the checksum byte
  Read();
  int sum = 0;
  for (uint8_t b : current_record_) {
    sum += b;
  }
  if ((sum & 0xff) != 0) {
    std::stringstream ss;
    ss << "Checksum mismatch. Sum: " << std::hex << (sum & 0xff)
       << ". Record: " << GetCurrentRecordAsHexString();
    return ss.str();
  }
  return std::nullopt;
}

} // namespace jpireader
