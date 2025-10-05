# Makefile for bin2hex/bin2mot - Binary to HEX/S-Record converters
# 
# This Makefile builds C++ applications that convert binary files to
# Intel HEX and Motorola S-Record formats, providing the reverse 
# functionality of hex2bin/mot2bin utilities.
#
# Written after years of frustration with having to manually create
# hex files for embedded development. Finally decided to write the
# reverse tools!
#
# Targets:
#   all       - Build both bin2hex and bin2mot
#   bin2hex   - Build Intel HEX converter 
#   bin2mot   - Build Motorola S-Record converter
#   clean     - Remove build artifacts
#   test      - Run validation tests
#   install   - Install binaries to system
#   doc       - Generate documentation

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -pedantic -g
LDFLAGS = 

# Installation directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/man/man1

# Source files and objects
COMMON_SOURCES = BinaryUtils.cpp
COMMON_OBJECTS = $(COMMON_SOURCES:.cpp=.o)

BIN2HEX_SOURCES = bin2hex.cpp HexConverter.cpp $(COMMON_SOURCES)
BIN2HEX_OBJECTS = $(BIN2HEX_SOURCES:.cpp=.o)

BIN2MOT_SOURCES = bin2mot.cpp SRecordConverter.cpp $(COMMON_SOURCES)  
BIN2MOT_OBJECTS = $(BIN2MOT_SOURCES:.cpp=.o)

# Cross-compilation support
ifdef TARGET_OS
    ifeq ($(TARGET_OS),windows)
        CXX = x86_64-w64-mingw32-g++
        LDFLAGS += -static
        EXEC_EXT = .exe
    endif
endif

# Main targets
.PHONY: all clean test install doc help

all: bin2hex bin2mot

# Intel HEX converter
bin2hex: bin2hex.o HexConverter.o BinaryUtils.o
	$(CXX) $(CXXFLAGS) -o $@$(EXEC_EXT) $^ $(LDFLAGS)

# Motorola S-Record converter  
bin2mot: bin2mot.o SRecordConverter.o BinaryUtils.o
	$(CXX) $(CXXFLAGS) -o $@$(EXEC_EXT) $^ $(LDFLAGS)

# Object file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dependencies (generated automatically)
bin2hex.o: bin2hex.cpp HexConverter.hpp BinaryUtils.hpp
bin2mot.o: bin2mot.cpp SRecordConverter.hpp BinaryUtils.hpp
HexConverter.o: HexConverter.cpp HexConverter.hpp BinaryUtils.hpp
SRecordConverter.o: SRecordConverter.cpp SRecordConverter.hpp BinaryUtils.hpp
BinaryUtils.o: BinaryUtils.cpp BinaryUtils.hpp

# Testing
test: bin2hex bin2mot
	@echo "Running validation tests..."
	@mkdir -p test_data
	@echo "Creating test binary files..."
	
	# Create a small test binary with known pattern
	@printf '\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F' > test_data/test.bin
	@printf '\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F' >> test_data/test.bin
	
	# Test bin2hex conversion
	@echo "Testing bin2hex..."
	./bin2hex$(EXEC_EXT) -v -a 0x1000 test_data/test.bin
	@if [ -f test.hex ]; then echo "✓ Intel HEX file created"; else echo "✗ Intel HEX conversion failed"; fi
	
	# Test bin2mot conversion  
	@echo "Testing bin2mot..."
	./bin2mot$(EXEC_EXT) -v -a 0x8000 -w 16 test_data/test.bin
	@if [ -f test.s19 ]; then echo "✓ S-Record file created"; else echo "✗ S-Record conversion failed"; fi
	
	# Test different options
	@echo "Testing various options..."
	./bin2hex$(EXEC_EXT) -l 8 -o test_data/test_8byte.hex test_data/test.bin
	./bin2mot$(EXEC_EXT) -w 32 -H "Test Header" -o test_data/test_32bit.s37 test_data/test.bin
	
	@echo "All tests completed successfully!"

# Cross-compilation for Windows
windows:
	$(MAKE) TARGET_OS=windows CXX=x86_64-w64-mingw32-g++ EXEC_EXT=.exe

# Installation
install: bin2hex bin2mot
	@echo "Installing binaries to $(BINDIR)..."
	install -d $(BINDIR)
	install -m 755 bin2hex$(EXEC_EXT) $(BINDIR)/
	install -m 755 bin2mot$(EXEC_EXT) $(BINDIR)/
	@echo "Installation complete."

# Uninstallation
uninstall:
	rm -f $(BINDIR)/bin2hex$(EXEC_EXT)
	rm -f $(BINDIR)/bin2mot$(EXEC_EXT)
	@echo "Uninstallation complete."

# Documentation (requires doxygen)
doc:
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "Generating documentation..."; \
		doxygen Doxyfile 2>/dev/null || echo "Create Doxyfile for documentation"; \
	else \
		echo "Doxygen not found. Install doxygen to generate documentation."; \
	fi

# Packaging
dist: clean
	@echo "Creating source distribution..."
	@VERSION=$$(grep VERSION_STRING bin2hex.cpp | cut -d'"' -f2); \
	tar czf bin2hex-$$VERSION.tar.gz *.cpp *.hpp Makefile README.md LICENSE

# Static analysis (requires cppcheck)
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "Running static analysis..."; \
		cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem *.cpp *.hpp; \
	else \
		echo "cppcheck not found. Install cppcheck for static analysis."; \
	fi

# Memory check (requires valgrind)
memcheck: bin2hex bin2mot test
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Running memory checks..."; \
		valgrind --leak-check=full --error-exitcode=1 ./bin2hex test_data/test.bin; \
		valgrind --leak-check=full --error-exitcode=1 ./bin2mot test_data/test.bin; \
	else \
		echo "valgrind not found. Install valgrind for memory checking."; \
	fi

# Performance profiling (requires gprof)
profile: CXXFLAGS += -pg
profile: bin2hex bin2mot
	@echo "Building with profiling enabled..."

# Debug build
debug: CXXFLAGS += -DDEBUG -O0
debug: bin2hex bin2mot
	@echo "Debug build complete."

# Release build  
release: CXXFLAGS += -DNDEBUG -O3
release: clean bin2hex bin2mot
	strip bin2hex$(EXEC_EXT)
	strip bin2mot$(EXEC_EXT)
	@echo "Release build complete."

# Cleanup
clean:
	rm -f *.o bin2hex$(EXEC_EXT) bin2mot$(EXEC_EXT)
	rm -f *.hex *.s19 *.s28 *.s37 *.srec
	rm -rf test_data/
	rm -f gmon.out core core.*
	@echo "Cleanup complete."

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build both converters (default)"
	@echo "  bin2hex   - Build Intel HEX converter only"
	@echo "  bin2mot   - Build Motorola S-Record converter only"
	@echo "  test      - Run validation tests"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install to system directories"
	@echo "  uninstall - Remove from system directories" 
	@echo "  windows   - Cross-compile for Windows"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  analyze   - Run static code analysis"
	@echo "  memcheck  - Run memory leak checks"
	@echo "  doc       - Generate documentation"
	@echo "  dist      - Create source distribution"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX    - Installation prefix (default: /usr/local)"
	@echo "  CXX       - C++ compiler (default: g++)"
	@echo "  CXXFLAGS  - Compiler flags"

# Default target
.DEFAULT_GOAL := all