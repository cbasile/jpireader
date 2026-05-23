#ifndef JPIREADER_BIT_SET_H_
#define JPIREADER_BIT_SET_H_

#include <vector>
#include <cstdint>

namespace jpireader {

class BitSet {
 public:
  explicit BitSet(int num_bytes);
  void Clear();
  void SetByte(int index, uint8_t value);
  void SetWord(int index, uint16_t value);
  bool TestBit(int bit_index) const;
  int CountBits(int start_bit, int end_bit) const;
  int NumBits() const;
  int NumBytes() const;

 private:
  std::vector<uint8_t> bytes_;
};

}  // namespace jpireader

#endif  // JPIREADER_BIT_SET_H_
