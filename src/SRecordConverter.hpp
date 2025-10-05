#ifndef SRECORD_CONVERTER_HPP
#define SRECORD_CONVERTER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

/**
 * Motorola S-Record format converter
 * Converts binary data to Motorola S-Record format files
 */
class SRecordConverter {
public:
    enum RecordType {
        S0_HEADER = 0,          // Header record
        S1_DATA_16 = 1,         // Data record with 16-bit address
        S2_DATA_24 = 2,         // Data record with 24-bit address
        S3_DATA_32 = 3,         // Data record with 32-bit address
        S4_RESERVED = 4,        // Reserved
        S5_COUNT_16 = 5,        // Record count with 16-bit count
        S6_COUNT_24 = 6,        // Record count with 24-bit count
        S7_END_32 = 7,          // End record with 32-bit address
        S8_END_24 = 8,          // End record with 24-bit address
        S9_END_16 = 9           // End record with 16-bit address
    };

    /**
     * Constructor
     * @param bytes_per_line Number of data bytes per line
     */
    explicit SRecordConverter(size_t bytes_per_line = 32);

    /**
     * Convert binary data to Motorola S-Record format
     * @param binary_data Input binary data
     * @param start_address Starting address for the data
     * @param output_file Output file path
     * @param address_size Address size (16, 24, or 32 bits)
     * @param header Optional header string
     * @return true on success, false on error
     */
    bool convert_to_srec(const std::vector<uint8_t>& binary_data,
                        uint32_t start_address,
                        const std::string& output_file,
                        int address_size = 32,
                        const std::string& header = "");

    /**
     * Set bytes per line for output formatting
     */
    void set_bytes_per_line(size_t bytes_per_line) {
        bytes_per_line_ = bytes_per_line;
    }

    /**
     * Get the last error message
     */
    const std::string& get_last_error() const { return last_error_; }

private:
    size_t bytes_per_line_;
    std::string last_error_;

    /**
     * Generate a single S-Record
     */
    std::string generate_record(RecordType type,
                               uint32_t address,
                               const std::vector<uint8_t>& data);

    /**
     * Calculate S-Record checksum
     */
    uint8_t calculate_checksum(uint8_t byte_count,
                              uint32_t address,
                              int address_bytes,
                              const std::vector<uint8_t>& data);

    /**
     * Get number of address bytes for record type
     */
    int get_address_bytes(RecordType type);

    /**
     * Get data record type based on address size
     */
    RecordType get_data_record_type(int address_size);

    /**
     * Get end record type based on address size
     */
    RecordType get_end_record_type(int address_size);
};

#endif // SRECORD_CONVERTER_HPP