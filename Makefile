BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/bin/MizanEditor

.PHONY: all build run clean

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && make

run: build
	./$(EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR)
