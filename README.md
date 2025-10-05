# bin2hex - Binary to HEX/S-Record Converters

This project provides utilities to convert binary files to Intel HEX and Motorola S-Record formats, which are commonly used for programming microcontrollers and embedded systems.

## Programs Included

- **bin2hex** - Converts binary files to Intel HEX format
- **bin2mot** - Converts binary files to Motorola S-Record format

These tools provide the reverse functionality of the popular hex2bin and mot2bin utilities.

## Features

- **Multiple Address Formats**: Support for 16-bit, 24-bit, and 32-bit addressing
- **Configurable Output**: Adjustable bytes per line and record formatting
- **Extended Addressing**: Intel HEX extended linear addressing for files > 64KB
- **Header Support**: Optional header records for S-Record files
- **Error Checking**: Comprehensive validation and error reporting
- **Cross-Platform**: Builds on Linux, macOS, and Windows

## Building

### Prerequisites
- C++11 compatible compiler (GCC 4.8+, Clang 3.3+, or MSVC 2013+)
- Make utility

### Compilation
```bash
# Build both utilities
make

# Build specific utility
make bin2hex
make bin2mot

# Build with debug information
make debug

# Build optimized release
make release
```

### Cross-Compilation for Windows
```bash
make windows
```

## Usage

### bin2hex - Intel HEX Converter

```bash
bin2hex [options] input_file

Options:
  -o FILE       Output file (default: input with .hex extension)
  -a ADDRESS    Starting address in hex (default: 0x0000)
  -l LENGTH     Bytes per line (1-255, default: 32)
  -e EXTENSION  Output file extension (default: hex)
  -s            Use segmented addressing (disable extended addressing)
  -v            Verbose output
  -V            Show version information
  -h, --help    Show help message
```

**Examples:**
```bash
# Basic conversion
bin2hex firmware.bin

# Specify starting address and line length
bin2hex -a 0x8000 -l 16 bootloader.bin

# Verbose output with custom filename
bin2hex -v -o output.hex program.bin
```

### bin2mot - Motorola S-Record Converter

```bash
bin2mot [options] input_file

Options:
  -o FILE       Output file (default: input with .s## extension)
  -a ADDRESS    Starting address in hex (default: 0x0000)
  -l LENGTH     Bytes per line (1-252, default: 32)
  -e EXTENSION  Output file extension (default: auto-select)
  -w WIDTH      Address width: 16, 24, or 32 bits (default: 32)
  -H HEADER     Header string for S0 record
  -v            Verbose output
  -V            Show version information
  -h, --help    Show help message
```

**Examples:**
```bash
# Basic conversion (32-bit addressing)
bin2mot firmware.bin

# 16-bit addressing with header
bin2mot -w 16 -H "Firmware v1.2" bootloader.bin

# Custom output with verbose logging
bin2mot -v -o output.s37 -a 0x10000 program.bin
```

## File Format Support

### Intel HEX Format
- **Data Records (00)**: Contains program data
- **End of File (01)**: Marks end of hex file
- **Extended Linear Address (04)**: For addresses > 64KB
- **Start Linear Address (05)**: Entry point specification

### Motorola S-Record Format
- **S0**: Header record with optional identification
- **S1/S2/S3**: Data records (16/24/32-bit addressing)
- **S5/S6**: Record count (16/24-bit count)
- **S7/S8/S9**: End records (32/24/16-bit addressing)

## Testing

Run the built-in test suite to verify functionality:

```bash
make test
```

This creates test binary files and validates the conversion process for both formats.

## Installation

Install to system directories (requires root/administrator privileges):

```bash
# Install to /usr/local/bin
make install

# Install to custom location
make install PREFIX=/opt/local
```

## Advanced Features

### Static Analysis
```bash
make analyze  # Requires cppcheck
```

### Memory Checking
```bash
make memcheck  # Requires valgrind
```

### Performance Profiling
```bash
make profile  # Enables gprof profiling
```

## Project Structure

```
bin2hex/
├── bin2hex.cpp           # Intel HEX converter main program
├── bin2mot.cpp           # Motorola S-Record converter main program
├── HexConverter.hpp/cpp  # Intel HEX format implementation
├── SRecordConverter.hpp/cpp  # S-Record format implementation
├── BinaryUtils.hpp/cpp   # Common binary utilities and CRC functions
├── Makefile              # Build system
└── README.md             # This file
```

## Technical Details

### Address Space Handling
- **16-bit**: Addresses 0x0000 - 0xFFFF
- **24-bit**: Addresses 0x000000 - 0xFFFFFF  
- **32-bit**: Addresses 0x00000000 - 0xFFFFFFFF

### Record Length Limits
- **Intel HEX**: 1-255 bytes per record (typically 32 or 16)
- **S-Record**: 1-252 bytes per record (accounting for address overhead)

### Checksum Algorithms
- **Intel HEX**: Two's complement checksum
- **S-Record**: One's complement checksum

## Compatibility

This implementation is designed to be compatible with:
- Standard Intel HEX format (INHX32)
- Motorola S-Record specification

## License

This project follows the same BSD-style license as the original hex2bin utilities.

## Contributing

Contributions are welcome! Please ensure:
- Code follows existing style conventions
- All tests pass (`make test`)
- Static analysis is clean (`make analyze`)
- Memory checks pass (`make memcheck`)

## See Also

- **hex2bin**: Convert Intel HEX to binary
- **mot2bin**: Convert Motorola S-Record to binary
- **srec_cat**: Swiss Army knife for firmware files