#ifndef BINARY_UTILS_HPP
#define BINARY_UTILS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace BinaryUtils {

/**
 * Reflection table for 8-bit values - used for checksum calculations
 */
extern const uint8_t reflect_table[256];

/**
 * Convert a nibble (4 bits) to ASCII hex character
 */
inline char nibble_to_hex(uint8_t nibble) {
    return (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
}

/**
 * Convert byte to two hex characters
 */
inline std::string byte_to_hex(uint8_t value) {
    std::string result(2, '0');
    result[0] = nibble_to_hex((value >> 4) & 0x0F);
    result[1] = nibble_to_hex(value & 0x0F);
    return result;
}

/**
 * Convert 16-bit word to four hex characters
 */
inline std::string word_to_hex(uint16_t value) {
    return byte_to_hex(static_cast<uint8_t>(value >> 8)) + 
           byte_to_hex(static_cast<uint8_t>(value & 0xFF));
}

/**
 * Convert 32-bit value to eight hex characters  
 */
inline std::string dword_to_hex(uint32_t value) {
    return word_to_hex(static_cast<uint16_t>(value >> 16)) +
           word_to_hex(static_cast<uint16_t>(value & 0xFFFF));
}

/**
 * Calculate 8-bit checksum (two's complement)
 */
uint8_t calculate_checksum8(const std::vector<uint8_t>& data);

/**
 * Calculate 16-bit CRC using specified polynomial
 */
uint16_t calculate_crc16(const std::vector<uint8_t>& data, 
                        uint16_t polynomial = 0x8005,
                        uint16_t initial = 0x0000);

/**
 * Calculate 32-bit CRC using specified polynomial
 */
uint32_t calculate_crc32(const std::vector<uint8_t>& data,
                        uint32_t polynomial = 0x04C11DB7,
                        uint32_t initial = 0xFFFFFFFF);

/**
 * Reflect bits in a byte (reverse bit order)
 */
uint8_t reflect_byte(uint8_t value);

/**
 * Reflect bits in a 16-bit word
 */
uint16_t reflect_word(uint16_t value);

/**
 * Reflect bits in a 32-bit value
 */
uint32_t reflect_dword(uint32_t value);

} // namespace BinaryUtils

#endif // BINARY_UTILS_HPP