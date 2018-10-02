#include "stdafx.h"
#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

#define RANDOM_MAX 255
#define RANDOM_MIN 0

unsigned char fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

Chip8::Chip8() {
	std::cout << "Chip8 initialization" << std::endl;

	this->drawFlag = false;
	this->delayTimer = 0x0;
	this->soundTimer = 0x0;

	this->registers.pc = 0x200;
	this->registers.I = 0x0;
	
	for (int i = 0; i < 16; i++) {
		this->registers.V[i] = 0x0;
	}

	for (int i = 0; i < 64 * 32; i++) {
		this->gfx[i] = 0x0;
	}

	this->keyboard.init();
	this->stack.clear();
	this->ram.clear();	

	srand(time(NULL));
}

void Chip8::emulateCycle() {
	//std::cout << std::endl << "Emulating cycle" << std::endl << std::endl;
	std::cout << std::endl << std::endl;

	unsigned int operation = this->fetch();

	std::cout << "0x" << std::hex << operation << std::endl;

	this->decode(operation);

	execute();

	if (delayTimer > 0)
	{
		--delayTimer;
	}

	if (soundTimer > 0)
	{
		if (soundTimer == 1)
		{
			std::cout << " BEEP!" << std::endl;
			--soundTimer;
		}
	}
}

unsigned int Chip8::fetch() {
	return (this->ram.readFrom(this->registers.pc) << 8) | (this->ram.readFrom(this->registers.pc + 1));
}

void Chip8::decode(unsigned int operation) {
	this->opcode.N    = operation & 0x000F;
	this->opcode.NN   = operation & 0x00FF;
	this->opcode.NNN  = operation & 0x0FFF;
	this->opcode.X    = (operation & 0x0F00) >> 8;
	this->opcode.Y    = (operation & 0x00F0) >> 4;
	this->opcode.opId = operation & 0xF000;
}

void Chip8::execute() {
	switch (opcode.opId) {
		case 0x0000:
			switch (opcode.N) {
				case 0x0000:
					std::cout << "CLEAR SCREEN" << std::endl;
					for (int i = 0; i < 64 * 32; i++) {
						this->gfx[i] = 0x00;
					}
					this->drawFlag = true;
					this->registers.pc += 2;
					break;
				case 0x000E:
					std::cout << "RETURN FROM SUBROUTINE" << std::endl;
					this->registers.pc = this->stack.pop();
					this->registers.pc += 2;
					break;
				default:
					std::cout << "Unknown opcode for OP0 " << opcode.N << std::endl;
					break;
			}
			break;
		case 0x1000:			
			std::cout << "JUMP TO ADDRESS 0x" << std::hex << opcode.NNN << std::endl;
			this->registers.pc = opcode.NNN;
			break;
		case 0x2000:
			std::cout << "CALL SUBROUTINE 0x" << std::hex << opcode.NNN << std::endl;
			this->stack.push(this->registers.pc);
			this->registers.pc = opcode.NNN;
			break;
		case 0x3000:
			std::cout << "SKIP NEXT EQ, V" << opcode.X << ", 0x" << std::hex << opcode.NN << std::endl;
			if (this->registers.V[opcode.X] == opcode.NN) {
				this->registers.pc += 2;
			}
			this->registers.pc += 2;
			break; 
		case 0x4000:
			std::cout << "SKIP NEXT !EQ, V" << opcode.X << ", 0x" << std::hex << opcode.NN << std::endl;
			if (this->registers.V[opcode.X] != opcode.NN) {
				this->registers.pc += 2;
			}
			this->registers.pc += 2;
			break;
		case 0x5000:
			std::cout << "SKIP NEXT EQ, V" << opcode.X << ", V" << opcode.Y << std::endl;
			if (this->registers.V[opcode.X] == this->registers.V[opcode.Y]) {
				this->registers.pc += 2;
			}
			this->registers.pc += 2;
			break; 
		case 0x6000:
			std::cout << "STORE V" << opcode.X << ", 0x" << std::hex << opcode.NN << std::endl;
			this->registers.V[opcode.X] = opcode.NN;
			this->registers.pc += 2;
			break;
		case 0x7000:
			std::cout << "STORE V" << opcode.X << ", ADD(V" << opcode.X << ", 0x" << std::hex << opcode.NN << ")" << std::endl;
			this->registers.V[opcode.X] += opcode.NN;
			this->registers.pc += 2;
			break; 
		case 0x8000:
			switch (opcode.N) {
				case 0x0000:
					std::cout << "STORE V" << opcode.X << ", V" << opcode.Y << std::endl;
					this->registers.V[opcode.X] = this->registers.V[opcode.Y];
					this->registers.pc += 2;
					break;
				case 0x0001:
					std::cout << "STORE V" << opcode.X << ", OR(V" << opcode.X << ", V" << opcode.Y << ")" << std::endl;
					this->registers.V[opcode.X] |= this->registers.V[opcode.Y];
					this->registers.pc += 2;
					break;
				case 0x0002:
					std::cout << "STORE V" << opcode.X << ", AND(V" << opcode.X << ", V" << opcode.Y << ")" << std::endl;
					this->registers.V[opcode.X] &= this->registers.V[opcode.Y];
					this->registers.pc += 2;
					break;
				case 0x0003:
					std::cout << "STORE V" << opcode.X << ", XOR(V" << opcode.X << ", V" << opcode.Y << ")" << std::endl;
					this->registers.V[opcode.X] ^= this->registers.V[opcode.Y];
					this->registers.pc += 2;
					break;
				case 0x0004:
					std::cout << "STORE V" << opcode.X << ", ADD(V" << opcode.X << ", V" << opcode.Y << ")" << std::endl;
					this->registers.V[opcode.X] += this->registers.V[opcode.Y];
					this->registers.V[0xF] = this->registers.V[opcode.Y] > (0xFF - this->registers.V[opcode.X]) ? 0x1 : 0x0;
					this->registers.pc += 2;
					break;
				case 0x0005:
					std::cout << "STORE V" << opcode.X << ", SUB(V" << opcode.X << ", V" << opcode.Y << ")" << std::endl;
					this->registers.V[0xF] = this->registers.V[opcode.Y] > this->registers.V[opcode.X] ? 0x0 : 0x1;
					this->registers.V[opcode.X] -= this->registers.V[opcode.Y];					
					this->registers.pc += 2;
					break;
				case 0x0006:
					std::cout << "STORE Vf, LEAST_BIT(V" << opcode.X << ")" << std::endl;
					this->registers.V[0xF] = this->registers.V[opcode.X] & 0x1;
					this->registers.V[opcode.X] >>= 1;
					this->registers.pc += 2;
					break;
				case 0x0007:
					// Implementar
					this->registers.pc += 2;
					break;
				case 0x000E:
					// Implementar
					this->registers.pc += 2;
					break;
				default:
					std::cout << "Unknown opcode for OP8 " << opcode.N << std::endl;
					break;
			}
			break;
		case 0x9000:
			std::cout << "SKIP NEXT !EQ, V" << opcode.X << ", V" << opcode.Y << std::endl;
			if (this->registers.V[opcode.X] != this->registers.V[opcode.Y]) {
				this->registers.pc += 2;
			}
			this->registers.pc += 2;
			break;
		case 0xA000:
			std::cout << "STORE I, 0x" << std::hex << opcode.NNN << std::endl;
			this->registers.I = opcode.NNN;
			this->registers.pc += 2;
			break;
		case 0xB000:
			std::cout << "JUMP TO ADDRESS ADD(0x" << std::hex << opcode.NNN << ", V0)" << std::endl;
			this->registers.pc = opcode.NNN + this->registers.V[0x0];
			break;
		case 0xC000:
			std::cout << "STORE V" << opcode.X << ", RANDOM()" << std::endl;
			this->registers.V[opcode.X] = opcode.NN && (rand() % (RANDOM_MAX - RANDOM_MIN + 1) + RANDOM_MAX);
			this->registers.pc += 2;
			break;
		case 0xD000: 
		{
			std::cout << "DRAW V" << opcode.X << ", V" << opcode.Y << ", 0x" << std::hex << opcode.N << std::endl;

			this->registers.V[0xF] = 0;

			unsigned char x = this->registers.V[opcode.X];
			unsigned char y = this->registers.V[opcode.Y];
			unsigned char height = opcode.N;

			unsigned char pixel;

			for (int yline = 0; yline < height; yline++) {
				pixel = this->ram.readFrom(this->registers.I + yline);

				for (int xline = 0; xline < 8; xline++) {
					if ((pixel & (0x80 >> xline)) != 0) {
						if (this->gfx[x + xline + ((y + yline) * 64)] == 1) {
							this->registers.V[0xF] = 1;
						}
						this->gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			this->drawFlag = true;
			this->registers.pc += 2;

			break;
		}
		case 0xE000:
			switch (opcode.NN) {
				case 0x009E:
					std::cout << "SKIP NEXT !EQ, Key(0x" << std::hex << (int)this->registers.V[opcode.X] << "), 0x0" << std::endl;
					if (this->keyboard.getKeyState(this->registers.V[opcode.N]) != 0x0) {
						this->registers.pc += 2;
					}
					this->registers.pc += 2;
					break;
				case 0x00A1:
					std::cout << "SKIP NEXT EQ, Key(0x" << std::hex << (int)this->registers.V[opcode.X] << "), 0x0" << std::endl;
					if (this->keyboard.getKeyState(this->registers.V[opcode.X]) == 0x0) {
						this->registers.pc += 2;
					}
					this->registers.pc += 2;
					break;
				default:
					std::cout << "Unknown opcode for OPE " << opcode.N << std::endl;
					break;
			}
			break;
		case 0xF000:
			switch (opcode.NN) {
				case 0x0007:
					std::cout << "STORE V" << opcode.X << ", delay_timer" << std::endl;
					this->registers.V[opcode.X] = this->delayTimer;
					this->registers.pc += 2;
					break;
				case 0x000A:
					// Implementar
					this->registers.pc += 2;
					break;
				case 0x0015:
					std::cout << "SET delay_timer, V" << opcode.X << std::endl;
					this->delayTimer = this->registers.V[opcode.X];
					this->registers.pc += 2;
					break;
				case 0x0018:
					// Implementar
					this->registers.pc += 2;
					break;
				case 0x001E:
					std::cout << "ADD I, I, V" << opcode.X << std::endl;
					if (this->registers.I + this->registers.V[opcode.X] > 0xFFF) {
						this->registers.V[0xF] = 0x1;
					}
					else {
						this->registers.V[0xF] = 0x0;
					}
					this->registers.I += this->registers.V[opcode.X];
					this->registers.pc += 2;
					break;
				case 0x0029:
					std::cout << "STORE I, 0x" << std::hex << opcode.X * 0x5 << std::endl;
					this->registers.I = opcode.X * 0x5;
					this->registers.pc += 2;
					break;
				case 0x0033:
				{
					std::cout << "LOAD I, BINARY(V" << opcode.X << ")" << std::endl;
					unsigned char vxValue = this->registers.V[opcode.X];
					this->ram.writeTo(this->registers.I, vxValue / 100);
					this->ram.writeTo(this->registers.I + 1, (vxValue / 10) % 10);
					this->ram.writeTo(this->registers.I + 2, (vxValue % 100) % 10);
					this->registers.pc += 2;
					break;
				}
				case 0x0055:
					// Implementar
					this->registers.pc += 2;
					break;
				case 0x0065:
					std::cout << "FILL V0 TO V" << opcode.X << std::endl;
					for (int i = 0; i < opcode.X; i++) {
						this->registers.V[i] = this->ram.readFrom(this->registers.I + i);
					}

					this->registers.I += opcode.X + 1;
					this->registers.pc += 2;
					break;
				default:
					std::cout << "Unknown opcode for OPF " << opcode.NN << std::endl;
					break;
			}
			break;
		default:
			std::cout << "Unknown opcode " << opcode.opId << std::endl;
			break;
	}
}

void Chip8::loadProgram(const char *path) {
	std::ifstream is(path, std::ifstream::binary);

	if (is) {
		// get length of file:
		is.seekg(0, is.end);
		int length = is.tellg();
		is.seekg(0, is.beg);

		char * buffer = new char[length];

		std::cout << "Reading " << length << " characters... ";

		// read data as a block:
		is.read(buffer, length);

		if (is) {
			std::cout << "all characters read successfully." << std::endl;

			for (int i = 0; i < length; i++) {
				this->ram.writeTo(0x200 + i, (unsigned char)buffer[i]);
			}			
		}
		else {
			std::cout << "error: only " << is.gcount() << " could be read" << std::endl;
		}

		is.close();
	}
}

void Chip8::Stack::clear() {
	this->sp = 0x0;

	for (int i = 0; i < 16; i++) {
		this->stk[i] = 0x0;
	}
}

void Chip8::Stack::push(unsigned int data) {
	this->stk[this->sp] = data;
	++this->sp;
}

unsigned int Chip8::Stack::pop() {
	--this->sp;
	return this->stk[this->sp];
}

void Chip8::RAM::clear() {
	for (int i = 0; i < 4096; i++) {
		this->mem[i] = 0x0;
	}
}

unsigned char Chip8::RAM::readFrom(unsigned int address) {	
	return this->mem[address];
}

void Chip8::RAM::writeTo(unsigned int address, unsigned char data) {
	this->mem[address] = data;
}

void Chip8::Keyboard::init() {
	for (int i = 0; i < 16; i++) {
		this->keys[i] = 0x0;
	}
}

unsigned char Chip8::Keyboard::getKeyState(unsigned char keyIndex) {
	return this->keys[keyIndex];
}

void Chip8::Keyboard::setKeydown(unsigned char key) {
	this->keys[key] = 0x1;
}

void Chip8::Keyboard::setKeyup(unsigned char key) {
	this->keys[key] = 0x0;
}