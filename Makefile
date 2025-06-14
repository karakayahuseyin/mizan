BUILD_DIR = bin
EXECUTABLE = $(BUILD_DIR)/Simple3D

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
