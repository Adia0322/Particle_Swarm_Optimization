################################
###      Configuration       ###
################################
CC = gcc
# define params (-Wall: show warnings, -g: add debug info, -I : srearch from root)                       
# compile params
ROOT_DIR := $(CURDIR)
CFLAGS = -Wall -O2 -fPIC \
	-I$(ROOT_DIR)/include

LDFLAGS 			 = 		-lm -shared -Wl,--export-all-symbols      # link params
BUILD_DIR            =      build
OUTPUT_PSO           =      PSO.dll

################################
###         Source           ###
################################
SRCS_PSO             =      $(wildcard core/*.c) \
							$(wildcard core/cost_function/*.c) \
                            $(wildcard factory/*.c) \
                            $(wildcard interp/*.c) \
                            $(wildcard system/*.c)

HEADERS_PSO          =      $(wildcard include/*.h) \
							$(wildcard include/cost_function/*.h)

################################
###         Target           ###
################################
TARGET_PSO           =      $(BUILD_DIR)/$(OUTPUT_PSO)

################################
###          Rules           ###
################################
.PHONY: check
check:
	@echo $(SRCS_PSO) $(HEADERS_PSO)

.PHONY: prepare
prepare:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

.PHONY: all
all: prepare $(TARGET_PSO) $(HEADERS_PSO)

$(TARGET_PSO): $(SRCS_PSO) $(HEADERS_PSO)
	$(CC) $(CFLAGS) $(SRCS_PSO) -o $(TARGET_PSO) $(LDFLAGS)

.PHONY: clean
clean:
	@if exist $(BUILD_DIR) del /f /q $(BUILD_DIR)\*.dll
