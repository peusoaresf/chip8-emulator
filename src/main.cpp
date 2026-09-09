#include "chip8.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>

#define SDL_MAIN_HANDLED
// LEARNINGS: SDL2 in macos will lookup a linked FRAMEWORK of the same name
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32

void drawGraphics(SDL_Renderer *renderer, unsigned char gfx[]);
void handleKeydown(SDL_Keycode key, Chip8 *chip8, bool *quitFlag);
void handleKeyup(SDL_Keycode key, Chip8 *chip8);
void handleMouseButtonDown(Uint8 mouseButtonIndex);

int main(void) {
	SDL_SetMainReady();

	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	Chip8 *chip8 = new Chip8();
	chip8->loadProgram("/Users/ferraped/Projects/personal/chip8-emulator/roms/brick.ch8");

	bool quit = false;
	while (!quit) {
		SDL_Delay(1);
		SDL_PollEvent(&event);

		chip8->emulateCycle();

		if (chip8->drawFlag) {
			chip8->drawFlag = false;
			drawGraphics(renderer, chip8->gfx);
		}

		switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				handleKeydown(event.key.keysym.sym, chip8, &quit);
				break;
			case SDL_KEYUP:
				handleKeyup(event.key.keysym.sym, chip8);
				break;
			case SDL_MOUSEBUTTONDOWN:
				handleMouseButtonDown(event.button.button);
				break;
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

void drawGraphics(SDL_Renderer *renderer, unsigned char gfx[]) {
	int x = 0;
	int y = 0;

	for (int i = 0; i < 64 * 32; i++) {
		if (i > 0 && (i % 64 == 0)) {
			y++;
			x = 0;
		}

		if (gfx[i] == 0x0) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		SDL_RenderDrawPoint(renderer, x, y);
		x++;
	}

	SDL_RenderPresent(renderer);
}

void handleKeydown(SDL_Keycode key, Chip8 *chip8, bool *quitFlag) {
	switch (key) {
		case SDLK_ESCAPE:
			*quitFlag = true;
			break;
		case SDLK_1:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_1);
			break;
		case SDLK_2:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_2);
			break;
		case SDLK_3:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_3);
			break;
		case SDLK_4:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_4);
			break;
		case SDLK_q:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_Q);
			break;
		case SDLK_w:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_W);
			break;
		case SDLK_e:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_E);
			break;
		case SDLK_r:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_R);
			break;
		case SDLK_a:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_A);
			break;
		case SDLK_s:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_S);
			break;
		case SDLK_d:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_D);
			break;
		case SDLK_f:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_F);
			break;
		case SDLK_z:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_Z);
			break;
		case SDLK_x:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_X);
			break;
		case SDLK_c:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_C);
			break;
		case SDLK_v:
			chip8->keyboard.setKeydown(chip8->keyboard.keyCode.Key_V);
			break;
		default:
			std::cout << "Unknown keycode " << SDL_GetKeyName(key) << std::endl;
			break;
	}
}

void handleKeyup(SDL_Keycode key, Chip8 *chip8) {
	switch (key) {
		case SDLK_1:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_1);
			break;
		case SDLK_2:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_2);
			break;
		case SDLK_3:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_3);
			break;
		case SDLK_4:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_4);
			break;
		case SDLK_q:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_Q);
			break;
		case SDLK_w:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_W);
			break;
		case SDLK_e:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_E);
			break;
		case SDLK_r:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_R);
			break;
		case SDLK_a:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_A);
			break;
		case SDLK_s:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_S);
			break;
		case SDLK_d:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_D);
			break;
		case SDLK_f:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_F);
			break;
		case SDLK_z:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_Z);
			break;
		case SDLK_x:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_X);
			break;
		case SDLK_c:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_C);
			break;
		case SDLK_v:
			chip8->keyboard.setKeyup(chip8->keyboard.keyCode.Key_V);
			break;
	}
}

void handleMouseButtonDown(Uint8 mouseButtonIndex) {
	switch (mouseButtonIndex) {
		case SDL_BUTTON_LEFT:
			std::cout << "Left mouse button down" << std::endl;
			break;
		default:
			std::cout << "Unknown mouse button down " << (int)mouseButtonIndex << std::endl;
			break;
	}
}
