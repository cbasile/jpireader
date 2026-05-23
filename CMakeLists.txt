cmake_minimum_required(VERSION 3.10)
project(jpireader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(jpireader
    main.cpp
    src/bit_set.cpp
    src/data_record_parser.cpp
    src/flight_parser.cpp
    src/jpi_decoder.cpp
    src/jpi_stream.cpp
    src/metadata_parser.cpp
    src/metadata_util.cpp
    src/metrics.cpp
    src/record_updater.cpp
    src/sensor_parser.cpp
)

target_include_directories(jpireader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(jpireader PRIVATE -Wall -Wextra)

enable_testing()

add_executable(integration_test
    tests/integration_test.cpp
    src/bit_set.cpp
    src/data_record_parser.cpp
    src/flight_parser.cpp
    src/jpi_decoder.cpp
    src/jpi_stream.cpp
    src/metadata_parser.cpp
    src/metadata_util.cpp
    src/metrics.cpp
    src/record_updater.cpp
    src/sensor_parser.cpp
)
target_include_directories(integration_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(unit_test
    tests/unit_test.cpp
    src/bit_set.cpp
    src/sensor_parser.cpp
)
target_include_directories(unit_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME IntegrationTest COMMAND integration_test)
add_test(NAME UnitTest COMMAND unit_test)
