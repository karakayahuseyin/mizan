BUILD_DIR = build
BIN_DIR = bin
EXECUTABLE = MizanEditor

.PHONY: all build run clean

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && make

run: build
	cd $(BUILD_DIR)/$(BIN_DIR) && ./$(EXECUTABLE)
clean:
	rm -rf $(BUILD_DIR)
