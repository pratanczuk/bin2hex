#ifndef HEX_CONVERTER_HPP
#define HEX_CONVERTER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

/**
 * Intel HEX file format converter
 * Converts binary data to Intel HEX format files
 */
class IntelHexConverter {
public:
    enum RecordType {
        DATA_RECORD = 0x00,
        EOF_RECORD = 0x01,
        EXTENDED_SEGMENT_ADDRESS = 0x02,
        START_SEGMENT_ADDRESS = 0x03,
        EXTENDED_LINEAR_ADDRESS = 0x04,
        START_LINEAR_ADDRESS = 0x05
    };

    /**
     * Constructor
     * @param bytes_per_line Number of data bytes per line (typically 16 or 32)
     */
    explicit IntelHexConverter(size_t bytes_per_line = 32);

    /**
     * Convert binary data to Intel HEX format
     * @param binary_data Input binary data
     * @param start_address Starting address for the data
     * @param output_file Output file path
     * @param use_extended_address Use extended linear addressing for addresses > 64KB
     * @return true on success, false on error
     */
    bool convert_to_hex(const std::vector<uint8_t>& binary_data,
                       uint32_t start_address,
                       const std::string& output_file,
                       bool use_extended_address = true);

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
     * Generate a single Intel HEX record
     */
    std::string generate_record(uint8_t byte_count,
                               uint16_t address,
                               uint8_t record_type,
                               const std::vector<uint8_t>& data);

    /**
     * Generate extended linear address record
     */
    std::string generate_extended_address_record(uint32_t address);

    /**
     * Calculate Intel HEX checksum
     */
    uint8_t calculate_checksum(uint8_t byte_count,
                              uint16_t address,
                              uint8_t record_type,
                              const std::vector<uint8_t>& data);
};

#endif // HEX_CONVERTER_HPP