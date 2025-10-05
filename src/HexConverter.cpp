#include "HexConverter.hpp"
#include "BinaryUtils.hpp"
#include <algorithm>
#include <iostream>

IntelHexConverter::IntelHexConverter(size_t bytes_per_line)
    : bytes_per_line_(bytes_per_line) {
    // Clamp bytes per line to reasonable values
    bytes_per_line_ = std::max(size_t(1), std::min(size_t(255), bytes_per_line_));
}

bool IntelHexConverter::convert_to_hex(const std::vector<uint8_t>& binary_data,
                                      uint32_t start_address,
                                      const std::string& output_file,
                                      bool use_extended_address) {
    std::ofstream out(output_file);
    if (!out.is_open()) {
        last_error_ = "Failed to open output file: " + output_file;
        return false;
    }

    try {
        uint32_t current_address = start_address;
        uint32_t extended_address = 0xFFFFFFFF;
        size_t data_offset = 0;

        while (data_offset < binary_data.size()) {
            // Check if we need to output an extended address record
            if (use_extended_address && (current_address >> 16) != extended_address) {
                extended_address = current_address >> 16;
                std::string ext_record = generate_extended_address_record(current_address);
                out << ext_record << std::endl;
            }

            // Calculate how many bytes to write in this record
            size_t bytes_remaining = binary_data.size() - data_offset;
            size_t bytes_this_line = std::min(bytes_per_line_, bytes_remaining);
            
            // Handle address wraparound within 64KB boundary
            size_t max_bytes_to_boundary = 0x10000 - (current_address & 0xFFFF);
            bytes_this_line = std::min(bytes_this_line, max_bytes_to_boundary);

            // Extract data for this record
            std::vector<uint8_t> line_data(
                binary_data.begin() + data_offset,
                binary_data.begin() + data_offset + bytes_this_line
            );

            // Generate and write the data record
            std::string record = generate_record(
                static_cast<uint8_t>(bytes_this_line),
                static_cast<uint16_t>(current_address & 0xFFFF),
                DATA_RECORD,
                line_data
            );
            out << record << std::endl;

            // Update counters
            data_offset += bytes_this_line;
            current_address += bytes_this_line;
        }

        // Write end-of-file record
        std::string eof_record = generate_record(0, 0, EOF_RECORD, {});
        out << eof_record << std::endl;

        out.close();
        return true;

    } catch (const std::exception& e) {
        last_error_ = "Exception during conversion: " + std::string(e.what());
        return false;
    }
}

std::string IntelHexConverter::generate_record(uint8_t byte_count,
                                              uint16_t address,
                                              uint8_t record_type,
                                              const std::vector<uint8_t>& data) {
    std::string record = ":";
    
    // Byte count
    record += BinaryUtils::byte_to_hex(byte_count);
    
    // Address (big endian)
    record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address >> 8));
    record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address & 0xFF));
    
    // Record type
    record += BinaryUtils::byte_to_hex(record_type);
    
    // Data bytes
    for (uint8_t byte : data) {
        record += BinaryUtils::byte_to_hex(byte);
    }
    
    // Checksum
    uint8_t checksum = calculate_checksum(byte_count, address, record_type, data);
    record += BinaryUtils::byte_to_hex(checksum);
    
    return record;
}

std::string IntelHexConverter::generate_extended_address_record(uint32_t address) {
    uint16_t extended_addr = static_cast<uint16_t>(address >> 16);
    std::vector<uint8_t> addr_data = {
        static_cast<uint8_t>(extended_addr >> 8),
        static_cast<uint8_t>(extended_addr & 0xFF)
    };
    
    return generate_record(2, 0, EXTENDED_LINEAR_ADDRESS, addr_data);
}

uint8_t IntelHexConverter::calculate_checksum(uint8_t byte_count,
                                             uint16_t address,
                                             uint8_t record_type,
                                             const std::vector<uint8_t>& data) {
    uint32_t sum = byte_count;
    sum += (address >> 8) & 0xFF;
    sum += address & 0xFF;
    sum += record_type;
    
    for (uint8_t byte : data) {
        sum += byte;
    }
    
    return static_cast<uint8_t>((256 - (sum & 0xFF)) & 0xFF);
}