#include "bit_set.h"

namespace jpireader {

BitSet::BitSet(int num_bytes) : bytes_(num_bytes, 0) {}

void BitSet::Clear() {
  std::fill(bytes_.begin(), bytes_.end(), 0);
}

void BitSet::SetByte(int index, uint8_t value) {
  if (index >= 0 && index < static_cast<int>(bytes_.size())) {
    bytes_[index] = value;
  }
}

void BitSet::SetWord(int index, uint16_t value) {
  SetByte(index, value & 0xff);
  SetByte(index + 1, (value >> 8) & 0xff);
}

bool BitSet::TestBit(int bit_index) const {
  int byte_index = bit_index / 8;
  int bit_in_byte = bit_index % 8;
  if (byte_index >= 0 && byte_index < static_cast<int>(bytes_.size())) {
    return (bytes_[byte_index] & (1 << bit_in_byte)) != 0;
  }
  return false;
}

int BitSet::CountBits(int start_bit, int end_bit) const {
  int count = 0;
  for (int i = start_bit; i <= end_bit; ++i) {
    if (TestBit(i)) {
      count++;
    }
  }
  return count;
}

int BitSet::NumBits() const {
  return bytes_.size() * 8;
}

int BitSet::NumBytes() const {
  return bytes_.size();
}

}  // namespace jpireader
