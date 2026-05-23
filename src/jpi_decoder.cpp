#include "jpi_decoder.h"
#include "metadata_parser.h"
#include "metadata_util.h"
#include "flight_parser.h"

namespace jpireader {

JpiFile JpiDecoder::Decode(JpiStream& stream) {
  JpiFile jpi_file;
  
  MetadataParser metadata_parser(stream);
  jpi_file.metadata = metadata_parser.Parse();
  
  MetadataUtil metadata_util(jpi_file.metadata);
  
  for (const auto& flight_metadata : jpi_file.metadata.flight_metadata) {
    FlightParser flight_parser(stream, flight_metadata, metadata_util);
    jpi_file.flights.push_back(flight_parser.Parse());
  }
  
  return jpi_file;
}

}  // namespace jpireader
