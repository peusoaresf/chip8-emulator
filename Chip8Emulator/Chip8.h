class Chip8 {
	private: 
		unsigned char delayTimer;
		unsigned char soundTimer;

		struct Stack {
			private:
				unsigned int sp;
				unsigned int stk[16];
			public:
				void clear();
				void push(unsigned int data);
				unsigned int pop();
		} stack;

		struct RAM {
			private:
				unsigned char mem[4096];
			public:
				void clear();
				unsigned char readFrom(unsigned int address);
				void writeTo(unsigned int adress, unsigned char data);
		} ram;

		struct Registers {
			public:
				unsigned int I;				
				unsigned int pc;
				unsigned char V[16];
		} registers;

		struct Opcode {
			public:
				unsigned int NNN, NN, N, X, Y, opId;
		} opcode;

		unsigned int fetch();
		void decode(unsigned int);
		void execute();

	public:
		struct Keyboard {
			private:
				unsigned char keys[16];
			public: 				
				struct KeyCode {
					const unsigned char Key_1 = 0x0;
					const unsigned char Key_2 = 0x1;
					const unsigned char Key_3 = 0x2;
					const unsigned char Key_4 = 0x3;
					const unsigned char Key_Q = 0x4;
					const unsigned char Key_W = 0x5;
					const unsigned char Key_E = 0x6;
					const unsigned char Key_R = 0x7;
					const unsigned char Key_A = 0x8;
					const unsigned char Key_S = 0x9;
					const unsigned char Key_D = 0xA;
					const unsigned char Key_F = 0xB;
					const unsigned char Key_Z = 0xC;
					const unsigned char Key_X = 0xD;
					const unsigned char Key_C = 0xE;
					const unsigned char Key_V = 0xF;
				} keyCode;
				void init(); 				
				void setKeydown(unsigned char key);
				void setKeyup(unsigned char key);
				unsigned char getKeyState(unsigned char keyIndex);
		} keyboard;

		bool drawFlag;		
		unsigned char gfx[64 * 32];

		Chip8();

		void emulateCycle();
		void loadProgram(const char *path);
};