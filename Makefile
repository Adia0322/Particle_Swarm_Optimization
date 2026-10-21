################################
###      Configuration       ###
################################
CC = gcc
# define params (-Wall: show warnings, -g: add debug info, -I : Add a search directory for header)                       
# compile params
ROOT_DIR := $(CURDIR)
CFLAGS = -Wall -O2 -fPIC \
	-I$(ROOT_DIR)/include \
	-I$(ROOT_DIR)/example/dic/include \
	-I$(ROOT_DIR)/system/include

LDFLAGS 			 = 		-lm -shared -Wl,--export-all-symbols      # link params
BUILD_DIR            =      build
OUTPUT_PSO           =      PSO.dll

################################
###         Source           ###
################################
# note: SRCS_PSO & HEADERS_PSO aim to add dependency, NOT indicate header path!
SRCS_PSO             =      $(wildcard core/*.c) \
							$(wildcard example/dic/cost_function/*.c) \
                            $(wildcard system/*.c) \
							$(wildcard tool/*.c) \
							$(wildcard tool/math/*.c) \
							$(wildcard tool/interp/*.c)

HEADERS_PSO          =      $(wildcard include/*.h) \
							$(wildcard example/dic/include/*.h)

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

.PHONY: all
all: $(TARGET_PSO)

$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(TARGET_PSO): $(SRCS_PSO) $(HEADERS_PSO) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRCS_PSO) -o $(TARGET_PSO) $(LDFLAGS)

.PHONY: clean
clean:
	@if exist $(BUILD_DIR) del /f /q $(BUILD_DIR)\*.dll
