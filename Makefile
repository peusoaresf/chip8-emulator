BUILD_DIR        := build
PACKAGES_DIR     := packages
MAC_PACKAGES_DIR := $(PACKAGES_DIR)/mac

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(PACKAGES_DIR)

.PHONY: pull-packages-mac
pull-packages-mac:
	@if [ -d "$(MAC_PACKAGES_DIR)/SDL2.framework" ] ; then \
		echo "\nSkipping pulling mac packages..."; \
		exit 0; \
	fi; \
	echo "\nPulling mac packages..."; \
	mkdir -p .tmp && \
	wget -O .tmp/SDL2.dmg https://github.com/libsdl-org/SDL/releases/download/release-2.32.0/SDL2-2.32.0.dmg && \
	hdiutil attach .tmp/SDL2.dmg && \
	mkdir -p $(MAC_PACKAGES_DIR) && \
	cp -R /Volumes/SDL2/SDL2.framework $(MAC_PACKAGES_DIR) && \
	hdiutil detach /Volumes/SDL2 -force && \
	rm -rf .tmp

.PHONY: mac
mac: pull-packages-mac
	@echo "\nCompiling mac.."
	@mkdir -p $(BUILD_DIR)
	@cp -r $(PACKAGES_DIR) $(BUILD_DIR)
	@clang++ -o $(BUILD_DIR)/chip8 src/main.cpp src/chip8.cpp -F$(MAC_PACKAGES_DIR) -framework SDL2 -Wl,-rpath,@executable_path/$(MAC_PACKAGES_DIR)

.PHONY: run
run: mac
	@echo "\nRunning application..\n"
	@cd $(BUILD_DIR) && ./chip8
