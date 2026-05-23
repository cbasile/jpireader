#include "src/bit_set.h"
#include "src/sensor_parser.h"
#include <iostream>
#include <cassert>

void TestBitSet() {
    std::cout << "Testing BitSet..." << std::endl;
    jpireader::BitSet bs(4);
    
    bs.SetByte(0, 0x01);
    assert(bs.TestBit(0) == true);
    assert(bs.TestBit(1) == false);
    
    bs.SetWord(2, 0x1234);
    // 0x34 is 00110100, 0x12 is 00010010
    // Word at 2: index 2 is 0x34 (bits 16-23), index 3 is 0x12 (bits 24-31)
    // bits in 0x34 (index 2): bits 18, 20, 21
    // bits in 0x12 (index 3): bits 25, 28
    assert(bs.TestBit(18) == true);
    assert(bs.TestBit(20) == true);
    assert(bs.TestBit(21) == true);
    assert(bs.TestBit(25) == true);
    assert(bs.TestBit(28) == true);
    
    assert(bs.CountBits(16, 23) == 3);
    std::cout << "  Passed BitSet" << std::endl;
}

void TestSensorParser() {
    std::cout << "Testing SensorParser..." << std::endl;
    // Example: voltage (bit 0), 4 EGTs (bits 11-14), 4 CHTs (bits 2-5)
    // low word: bits 0-15. 
    // bit 0: 1
    // bits 2-5: 1111 -> 0x3c
    // bits 11-14: 1111 -> 0x7800
    // low = 0x783d
    
    // high word: bits 16-31.
    // oil temp (bit 20), rpm (bit 26), fuel flow (bit 27)
    // bit 20: 1 (bit 4 of high word) -> 0x10
    // bit 26: 1 (bit 10 of high word) -> 0x0400
    // bit 27: 1 (bit 11 of high word) -> 0x0800
    // high = 0x0c10
    
    jpireader::SensorParser sp(0x783d, 0x0c10);
    auto sensors = sp.Parse();
    
    assert(sensors.voltage == true);
    assert(sensors.num_exhaust_gas_temperature == 4);
    assert(sensors.num_cylinder_head_temperature == 4);
    assert(sensors.oil_temperature == true);
    assert(sensors.rpm == true);
    assert(sensors.fuel_flow == true);
    assert(sensors.manifold_pressure == false);

    std::cout << "  Passed SensorParser" << std::endl;
}

int main() {
    try {
        TestBitSet();
        TestSensorParser();
        std::cout << "All unit tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unit test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
