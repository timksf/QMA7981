# Globally relevant
NAME = examples
OS = WINDOWS

# Arduino CLI executable
ARDUINO_CLI = arduino-cli
BOARD_TYPE ?= arduino:avr:leonardo
PORT ?= COM5

# control verbosity
V=0

# Directories
ROOT_DIRECTORY = .

# Build path -- used to store built binary and object files
BUILD_DIR = build
BUILD_PATH = ${CURDIR}/$(BUILD_DIR)

# temp directory for disassembled files
TEMP_DIR = temp
TEMP_PATH = ${CURDIR}/$(TEMP_DIR)

# Conditionally assigned variables
ifneq ($(V), 0)
	VERBOSE=-v
endif

ifeq ($(OS), WINDOWS)
    RM = cmd /c del /q /f
    RRM = cmd /c rmdir /q /s
	PWD = %cd%
	IFEXIST = if exist
	IFNEXIST = if not exist
	MKDIR = cmd /c mkdir
else
    RM = rm -f
    RRM = rm -f -r
	IFEXIST =
	IFNEXIST =
endif

# rules 
.PHONY: all test upload clean

build: clean
	$(ARDUINO_CLI) compile $(VERBOSE) \
		--build-path=$(BUILD_PATH) \
		--build-cache-path=$(BUILD_PATH) \
		--build-property "compiler.cpp.extra_flags=-std=c++14 -I${CURDIR}/.." \
		-b $(BOARD_TYPE) $(NAME).ino

compile-upload: clean build upload

upload:
	$(ARDUINO_CLI) upload $(VERBOSE) \
		-p $(PORT) \
		--fqbn $(BOARD_TYPE) \
		-i $(BUILD_PATH)/$(NAME).ino.hex

clean:
	$(IFEXIST) $(BUILD_DIR) $(RRM) $(BUILD_DIR)
	$(IFEXIST) $(RM) *.log

