#ifndef JPIREADER_JPI_DECODER_H_
#define JPIREADER_JPI_DECODER_H_

#include "jpi_types.h"
#include "jpi_stream.h"

namespace jpireader {

class JpiDecoder {
 public:
  static JpiFile Decode(JpiStream& stream);
};

}  // namespace jpireader

#endif  // JPIREADER_JPI_DECODER_H_
