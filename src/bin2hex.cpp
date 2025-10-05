/**
 * bin2hex - Binary to Intel HEX converter
 * 
 * This utility converts binary files to Intel HEX format, which is commonly
 * used for programming microcontrollers and embedded systems.
 * 
 * After working with hex2bin for years, I finally got tired of manually
 * creating hex files, so I wrote the reverse utilities. The Intel HEX format
 * is pretty straightforward once you understand the checksum calculation.
 * 
 * Features:
 * - Supports extended linear addressing for files > 64KB
 * - Configurable record length (bytes per line) 
 * - Multiple output formats and options
 * - Comprehensive error checking and validation
 * 
 * Note: This implementation is compatible with the hex2bin v2.5 specification.
 * 
 * Author: Created to complement the hex2bin utilities
 * License: BSD-style (same as original hex2bin)
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include "HexConverter.hpp"
#include "BinaryUtils.hpp"

#define PROGRAM_NAME "bin2hex"
#define VERSION_STRING "1.0"

struct ProgramOptions {
    std::string input_file;
    std::string output_file;
    uint32_t start_address = 0;
    size_t bytes_per_line = 32;
    bool use_extended_addressing = true;
    bool verbose = false;
    bool version_info = false;
    bool help = false;
    std::string extension = "hex";
};

void show_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] input_file\n\n";
    std::cout << "Convert binary files to Intel HEX format\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o FILE       Output file (default: input with .hex extension)\n";
    std::cout << "  -a ADDRESS    Starting address in hex (default: 0x0000)\n";
    std::cout << "  -l LENGTH     Bytes per line (1-255, default: 32)\n";
    std::cout << "  -e EXTENSION  Output file extension (default: hex)\n";
    std::cout << "  -s            Use segmented addressing (disable extended addressing)\n";
    std::cout << "  -v            Verbose output\n";
    std::cout << "  -V            Show version information\n";
    std::cout << "  -h, --help    Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " firmware.bin\n";
    std::cout << "  " << program_name << " -a 0x8000 -l 16 bootloader.bin\n";
    std::cout << "  " << program_name << " -o output.hex program.bin\n\n";
}

void show_version() {
    std::cout << PROGRAM_NAME << " v" << VERSION_STRING << "\n";
    std::cout << "Binary to Intel HEX format converter\n";
    std::cout << "Compatible with hex2bin v2.5 specification\n\n";
    std::cout << "Copyright (C) 2024 - Binary conversion utilities\n";
    std::cout << "This is free software; see the source for copying conditions.\n";
}

bool parse_hex_value(const std::string& str, uint32_t& value) {
    try {
        size_t pos;
        if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X") {
            value = std::stoul(str.substr(2), &pos, 16);
        } else {
            value = std::stoul(str, &pos, 16);
        }
        return pos == str.length() - (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X" ? 2 : 0);
    } catch (const std::exception&) {
        return false;
    }
}

std::string get_base_filename(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    size_t start = (last_slash == std::string::npos) ? 0 : last_slash + 1;
    
    size_t last_dot = path.find_last_of('.');
    size_t end = (last_dot == std::string::npos || last_dot < start) ? path.length() : last_dot;
    
    return path.substr(start, end - start);
}

bool parse_arguments(int argc, char* argv[], ProgramOptions& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options.help = true;
            return true;
        } else if (arg == "-V") {
            options.version_info = true;
            return true;
        } else if (arg == "-v") {
            options.verbose = true;
        } else if (arg == "-s") {
            options.use_extended_addressing = false;
        } else if (arg == "-o" && i + 1 < argc) {
            options.output_file = argv[++i];
        } else if (arg == "-a" && i + 1 < argc) {
            if (!parse_hex_value(argv[++i], options.start_address)) {
                std::cerr << "Error: Invalid address format: " << argv[i] << std::endl;
                return false;
            }
        } else if (arg == "-l" && i + 1 < argc) {
            int length = std::atoi(argv[++i]);
            if (length < 1 || length > 255) {
                std::cerr << "Error: Invalid line length. Must be 1-255." << std::endl;
                return false;
            }
            options.bytes_per_line = static_cast<size_t>(length);
        } else if (arg == "-e" && i + 1 < argc) {
            options.extension = argv[++i];
        } else if (arg.front() == '-') {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            return false;
        } else {
            // Assume it's the input file
            if (options.input_file.empty()) {
                options.input_file = arg;
            } else {
                std::cerr << "Error: Multiple input files specified." << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

std::vector<uint8_t> read_binary_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (size < 0) {
        throw std::runtime_error("Cannot determine file size: " + filename);
    }
    
    // Read entire file
    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Error reading file: " + filename);
    }
    
    return buffer;
}

int main(int argc, char* argv[]) {
    ProgramOptions options;
    
    std::cout << PROGRAM_NAME << " v" << VERSION_STRING 
              << " - Binary to Intel HEX converter\n\n";
    
    if (argc == 1) {
        show_usage(argv[0]);
        return 1;
    }
    
    if (!parse_arguments(argc, argv, options)) {
        return 1;
    }
    
    if (options.help) {
        show_usage(argv[0]);
        return 0;
    }
    
    if (options.version_info) {
        show_version();
        return 0;
    }
    
    if (options.input_file.empty()) {
        std::cerr << "Error: No input file specified.\n";
        show_usage(argv[0]);
        return 1;
    }
    
    // Generate output filename if not specified
    if (options.output_file.empty()) {
        std::string base = get_base_filename(options.input_file);
        options.output_file = base + "." + options.extension;
    }
    
    try {
        // Read binary file
        if (options.verbose) {
            std::cout << "Reading binary file: " << options.input_file << std::endl;
        }
        
        std::vector<uint8_t> binary_data = read_binary_file(options.input_file);
        
        if (options.verbose) {
            std::cout << "File size: " << binary_data.size() << " bytes" << std::endl;
            std::cout << "Start address: 0x" << std::hex << std::uppercase 
                     << std::setfill('0') << std::setw(8) << options.start_address << std::endl;
            std::cout << "Bytes per line: " << std::dec << options.bytes_per_line << std::endl;
            std::cout << "Extended addressing: " 
                     << (options.use_extended_addressing ? "enabled" : "disabled") << std::endl;
        }
        
        // Convert to Intel HEX
        IntelHexConverter converter(options.bytes_per_line);
        
        if (options.verbose) {
            std::cout << "Converting to Intel HEX format..." << std::endl;
        }
        
        bool success = converter.convert_to_hex(
            binary_data,
            options.start_address,
            options.output_file,
            options.use_extended_addressing
        );
        
        if (!success) {
            std::cerr << "Error: " << converter.get_last_error() << std::endl;
            return 1;
        }
        
        if (options.verbose) {
            std::cout << "Successfully wrote: " << options.output_file << std::endl;
            
            // Calculate and show some statistics
            uint32_t end_address = options.start_address + binary_data.size() - 1;
            std::cout << "Address range: 0x" << std::hex << std::uppercase 
                     << std::setfill('0') << std::setw(8) << options.start_address
                     << " - 0x" << std::setw(8) << end_address << std::endl;
        } else {
            std::cout << "Output written to: " << options.output_file << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}