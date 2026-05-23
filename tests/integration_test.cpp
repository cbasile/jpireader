#include "src/jpi_stream.h"
#include "src/jpi_decoder.h"
#include <iostream>
#include <cassert>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

std::string FindTestData(const std::string& filename) {
    std::vector<std::string> search_paths = {
        "testdata/" + filename,
        "../testdata/" + filename,
        "../../testdata/" + filename
    };
    for (const auto& path : search_paths) {
        if (fs::exists(path)) return path;
    }
    return "";
}

void TestEdm830() {
    std::cout << "Testing edm830.jpi..." << std::endl;
    std::string path = FindTestData("edm830.jpi");
    if (path.empty()) {
        std::cerr << "Skip: edm830.jpi not found" << std::endl;
        return;
    }

    jpireader::JpiStream stream(path);
    jpireader::JpiFile jpi = jpireader::JpiDecoder::Decode(stream);

    assert(jpi.metadata.registration == "edm830");
    assert(jpi.metadata.protocol_version == 2);
    assert(jpi.flights.size() == 2);
    
    // Flight 45
    assert(jpi.flights[0].flight_number == 45);
    assert(jpi.flights[0].data.size() == 120);
    
    // Flight 72
    assert(jpi.flights[1].flight_number == 72);
    assert(jpi.flights[1].data.size() == 21);

    std::cout << "  Passed edm830.jpi" << std::endl;
}

void TestU260523() {
    std::cout << "Testing U260523.JPI..." << std::endl;
    std::string path = FindTestData("U260523.JPI");
    if (path.empty()) {
        std::cerr << "Skip: U260523.JPI not found" << std::endl;
        return;
    }

    jpireader::JpiStream stream(path);
    jpireader::JpiFile jpi = jpireader::JpiDecoder::Decode(stream);

    assert(jpi.metadata.registration == "N111XX");
    assert(jpi.metadata.protocol_version == 2);
    assert(jpi.flights.size() == 21);

    // Verify some specific flight record counts
    assert(jpi.flights[0].flight_number == 138);
    assert(jpi.flights[0].data.size() == 1249);
    
    assert(jpi.flights[20].flight_number == 158);
    assert(jpi.flights[20].data.size() == 4208);

    std::cout << "  Passed U260523.JPI" << std::endl;
}

int main() {
    try {
        TestEdm830();
        TestU260523();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
