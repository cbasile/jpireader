#ifndef JPIREADER_SENSOR_PARSER_H_
#define JPIREADER_SENSOR_PARSER_H_

#include "jpi_types.h"
#include "bit_set.h"

namespace jpireader {

class SensorParser {
 public:
  SensorParser(int low, int high);
  Sensors Parse();

 private:
  BitSet mask_;
};

}  // namespace jpireader

#endif  // JPIREADER_SENSOR_PARSER_H_
