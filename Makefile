mac:
	mkdir -p build
	cp -r deps build
	clang++ -o build/chip8 src/main.cpp src/chip8.cpp -Fdeps/mac -framework SDL2 -Wl,-rpath,@executable_path/deps/mac

run:
	cd build && ./chip8
