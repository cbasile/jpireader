#ifndef JPIREADER_FLIGHT_PARSER_H_
#define JPIREADER_FLIGHT_PARSER_H_

#include "jpi_types.h"
#include "jpi_stream.h"
#include "metadata_util.h"

namespace jpireader {

class FlightParser {
 public:
  FlightParser(JpiStream& stream, const FlightMetadata& metadata, const MetadataUtil& metadata_util);
  Flight Parse();

 private:
  void ParseFlightHeader(Flight& flight);
  void ParseFlightData(Flight& flight);
  int64_t ParseUnixTimestamp(int packed_date, int packed_time);

  JpiStream& stream_;
  int flight_number_;
  int estimated_flight_length_bytes_;
  const MetadataUtil& metadata_util_;
};

}  // namespace jpireader

#endif  // JPIREADER_FLIGHT_PARSER_H_
