BUILD_DIR    := build
PACKAGES_DIR := packages

clean:
	rm -rf $(BUILD_DIR)

mac:
	mkdir -p $(BUILD_DIR)
	cp -r $(PACKAGES_DIR) $(BUILD_DIR)
	clang++ -o $(BUILD_DIR)/chip8 src/main.cpp src/chip8.cpp -F$(PACKAGES_DIR)/mac -framework SDL2 -Wl,-rpath,@executable_path/$(PACKAGES_DIR)/mac

run:
	cd $(BUILD_DIR) && ./chip8
