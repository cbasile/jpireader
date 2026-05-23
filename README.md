# JpiReader

A lightweight, high-performance C++17 library and command-line interface (CLI) to parse JPI engine monitor flight data logs (typically `.JPI` or `.jpi` files from EDM-700, EDM-800, EDM-830, EDM-900, and EDM-960 units) and export them into structured, timezone-independent CSV files.

## Features

- **Robust Decoding**: Decodes JPI binary format records utilizing delta-encoding.
- **Flight Metadata Extraction**: Parses alarm thresholds, fuel settings, engine configuration flags, tail numbers, and flight counts.
- **Twin-Engine Support**: Supports single and multi-engine (twin-engine) data layouts (e.g., EDM-760/960 models), exporting dedicated columns per engine.
- **Timezone-Neutral Epochs**: Generates deterministic UTC timestamps regardless of the timezone of the system running the tool.
- **Google C++ Style compliant**: Adheres to modern C++ coding standards.

## Project Structure

- `main.cpp`: Entry point for the CLI tool which parses JPI files and writes output CSV files.
- `src/`: Decoder library sources:
  - `jpi_decoder.h`/`.cpp`: Orchestrates the decoding process.
  - `jpi_stream.h`/`.cpp`: Wraps stream read/peek operations.
  - `flight_parser.h`/`.cpp`: Extracts flight headers and records.
  - `metadata_parser.h`/`.cpp`: Decodes text-based metadata headers.
  - `metadata_util.h`/`.cpp`: Utility classes for engine features and timezone-neutral epoch formatting.
  - `record_updater.h`/`.cpp` & `metrics.h`/`.cpp`: Handles data updates using a flexible path-based delta resolution.
  - `bit_set.h`/`.cpp` & `sensor_parser.h`/`.cpp`: Decodes bitmasks representing sensor packages.
- `tests/`: Integration and unit tests.
- `testdata/`: Test logs for verification.

## Getting Started

### Prerequisites

A C++17 compiler (GCC, Clang, or MSVC) and CMake 3.10+.

### Building

Initialize and compile the project using CMake:

```bash
cmake -B build -S .
cmake --build build
```

### Running the CLI Tool

Convert any `.jpi` file to CSV outputs:

```bash
./build/jpireader <path_to_jpi_file> <output_directory>
```

This generates a CSV file per flight (e.g., `flight_1.csv`, `flight_2.csv`) inside the specified directory.

### Running Tests

Run the unit and integration tests using CTest:

```bash
ctest --test-dir build
```

## Style and Linter Checks

This codebase is formatted according to the **Google C++ Style Guide** using `clang-format`. You can re-verify style and format with:

```bash
clang-format -i -style=Google src/*.cpp src/*.h main.cpp tests/*.cpp
cpplint --filter=-legal/copyright src/*.cpp src/*.h main.cpp
```

## License

This project is open-source and available under the MIT License.
