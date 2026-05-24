#ifndef SRC_CSV_WRITER_H_
#define SRC_CSV_WRITER_H_

#include <ostream>
#include "src/jpi_types.h"

namespace jpireader {

void PrintFlightCsv(std::ostream &os, const Flight &flight, int max_engines,
                    int max_egt, int max_cht, bool is_oat_fahrenheit);

void PrintFlightCsvCustom(std::ostream &os, const Flight &flight);

}  // namespace jpireader

#endif  // SRC_CSV_WRITER_H_
