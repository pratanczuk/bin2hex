#include "SRecordConverter.hpp"
#include "BinaryUtils.hpp"
#include <algorithm>
#include <iostream>

SRecordConverter::SRecordConverter(size_t bytes_per_line)
    : bytes_per_line_(bytes_per_line) {
    // Reasonable limits for S-Record format
    bytes_per_line_ = std::max(size_t(1), std::min(size_t(252), bytes_per_line_));
}

bool SRecordConverter::convert_to_srec(const std::vector<uint8_t>& binary_data,
                                      uint32_t start_address,
                                      const std::string& output_file,
                                      int address_size,
                                      const std::string& header) {
    std::ofstream out(output_file);
    if (!out.is_open()) {
        last_error_ = "Failed to open output file: " + output_file;
        return false;
    }

    try {
        // Validate address size
        if (address_size != 16 && address_size != 24 && address_size != 32) {
            last_error_ = "Invalid address size. Must be 16, 24, or 32 bits.";
            return false;
        }

        // Write header record if provided
        if (!header.empty()) {
            std::vector<uint8_t> header_data(header.begin(), header.end());
            std::string header_record = generate_record(S0_HEADER, 0, header_data);
            out << header_record << std::endl;
        }

        // Get record types for this address size
        RecordType data_type = get_data_record_type(address_size);
        RecordType end_type = get_end_record_type(address_size);
        
        uint32_t current_address = start_address;
        size_t data_offset = 0;
        uint32_t record_count = 0;

        // Write data records
        while (data_offset < binary_data.size()) {
            // Calculate bytes for this record
            size_t bytes_remaining = binary_data.size() - data_offset;
            size_t bytes_this_line = std::min(bytes_per_line_, bytes_remaining);
            
            // Account for address and checksum overhead
            int addr_bytes = get_address_bytes(data_type);
            size_t max_data_bytes = 255 - addr_bytes - 1; // 1 byte for checksum
            bytes_this_line = std::min(bytes_this_line, max_data_bytes);

            // Extract data for this record
            std::vector<uint8_t> line_data(
                binary_data.begin() + data_offset,
                binary_data.begin() + data_offset + bytes_this_line
            );

            // Generate and write the data record
            std::string record = generate_record(data_type, current_address, line_data);
            out << record << std::endl;

            // Update counters
            data_offset += bytes_this_line;
            current_address += bytes_this_line;
            record_count++;
        }

        // Write record count (optional)
        if (record_count <= 0xFFFF) {
            std::vector<uint8_t> count_data = {
                static_cast<uint8_t>(record_count >> 8),
                static_cast<uint8_t>(record_count & 0xFF)
            };
            std::string count_record = generate_record(S5_COUNT_16, record_count, count_data);
            out << count_record << std::endl;
        }

        // Write end record
        std::string end_record = generate_record(end_type, start_address, {});
        out << end_record << std::endl;

        out.close();
        return true;

    } catch (const std::exception& e) {
        last_error_ = "Exception during conversion: " + std::string(e.what());
        return false;
    }
}

std::string SRecordConverter::generate_record(RecordType type,
                                             uint32_t address,
                                             const std::vector<uint8_t>& data) {
    std::string record = "S";
    
    // Record type
    record += std::to_string(static_cast<int>(type));
    
    // Calculate byte count (address bytes + data bytes + checksum byte)
    int addr_bytes = get_address_bytes(type);
    uint8_t byte_count = static_cast<uint8_t>(addr_bytes + data.size() + 1);
    record += BinaryUtils::byte_to_hex(byte_count);
    
    // Address bytes (big endian)
    if (addr_bytes >= 2) {
        record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address >> 8));
        record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address & 0xFF));
    }
    if (addr_bytes >= 3) {
        record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address >> 16));
    }
    if (addr_bytes >= 4) {
        record += BinaryUtils::byte_to_hex(static_cast<uint8_t>(address >> 24));
    }
    
    // Data bytes
    for (uint8_t byte : data) {
        record += BinaryUtils::byte_to_hex(byte);
    }
    
    // Checksum
    uint8_t checksum = calculate_checksum(byte_count, address, addr_bytes, data);
    record += BinaryUtils::byte_to_hex(checksum);
    
    return record;
}

uint8_t SRecordConverter::calculate_checksum(uint8_t byte_count,
                                            uint32_t address,
                                            int address_bytes,
                                            const std::vector<uint8_t>& data) {
    uint32_t sum = byte_count;
    
    // Add address bytes
    if (address_bytes >= 2) {
        sum += (address >> 8) & 0xFF;
        sum += address & 0xFF;
    }
    if (address_bytes >= 3) {
        sum += (address >> 16) & 0xFF;
    }
    if (address_bytes >= 4) {
        sum += (address >> 24) & 0xFF;
    }
    
    // Add data bytes
    for (uint8_t byte : data) {
        sum += byte;
    }
    
    // Return one's complement of least significant byte
    return static_cast<uint8_t>(~sum & 0xFF);
}

int SRecordConverter::get_address_bytes(RecordType type) {
    switch (type) {
        case S0_HEADER:
        case S1_DATA_16:
        case S5_COUNT_16:
        case S9_END_16:
            return 2;
        case S2_DATA_24:
        case S6_COUNT_24:
        case S8_END_24:
            return 3;
        case S3_DATA_32:
        case S7_END_32:
            return 4;
        default:
            return 2; // Default to 16-bit
    }
}

SRecordConverter::RecordType SRecordConverter::get_data_record_type(int address_size) {
    switch (address_size) {
        case 16: return S1_DATA_16;
        case 24: return S2_DATA_24;
        case 32: return S3_DATA_32;
        default: return S3_DATA_32;
    }
}

SRecordConverter::RecordType SRecordConverter::get_end_record_type(int address_size) {
    switch (address_size) {
        case 16: return S9_END_16;
        case 24: return S8_END_24;
        case 32: return S7_END_32;
        default: return S7_END_32;
    }
}