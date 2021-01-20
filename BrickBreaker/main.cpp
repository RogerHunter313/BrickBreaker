#include <SDL.h>
#include <iostream>
using namespace std;

bool quit = false;
SDL_Event event;
int ballX = 10;
int ballY = 10;
int ballVelX = 1;
int ballVelY = 1;

int bgW = 800;
int bgH = 600;
int bgWMin = 0;
int bgHMin = 0;
int batX = bgW / 2;
int batY = bgH - 30;

bool aKeyDown = false;
bool dKeyDown = false;

void EventHandler() {
	
	if (aKeyDown && batX > 0)   //this removes the delay that occurs whenever SDL_PollEvent is happening
		batX = batX - 2;

	if (dKeyDown && batX < bgW - 60)
		batX = batX + 2;

	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		quit = true;
	}

	//https://stackoverflow.com/questions/21311824/sdl2-key-repeat-delay

	else if (event.type == SDL_KEYDOWN) {					//http://sdl.beuc.net/sdl.wiki/SDLKey
		if (event.key.keysym.sym == SDLK_a) {  //don't want it moving off the screen to the left
			aKeyDown = true;
		}
		if (event.key.keysym.sym == SDLK_d) {
			dKeyDown = true;
		}
	}
	else if (event.type == SDL_KEYUP) {
		aKeyDown = false;
		dKeyDown = false;
	}


}

void moveBall() {
	ballX += ballVelX;
	ballY += ballVelY;
}

void ballCollision() {
	if (ballX < bgWMin || ballX > bgW - 20) {	//20 prevents the inner side of the ball rect to hit the wall
		ballVelX = -ballVelX;
	}
	if (ballY < bgHMin || ballY > bgH - 20) {
		ballVelY = -ballVelY;
	}
}

int main(int argc, char ** argv) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("My first game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Rect ballRect = { 20, 20, 20, 20 };  //https://wiki.libsdl.org/SDL_Rect
	SDL_Rect bgRect = { 0,0, 800, 600 };
	SDL_Rect batRect = { batX, batY, 60, 30 };

	SDL_Surface *ball = SDL_LoadBMP("ball.bmp");			//https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	SDL_SetColorKey(ball, SDL_TRUE, SDL_MapRGB(ball->format, 0, 0, 0));  //https://lazyfoo.net/tutorials/SDL/10_color_keying/index.php  keyed out black
	SDL_Surface *bg = SDL_LoadBMP("bg.bmp");
	SDL_Surface *bat = SDL_LoadBMP("bat.bmp");
	SDL_SetColorKey(bat, SDL_TRUE, SDL_MapRGB(bat->format, 0, 0, 0));


	SDL_Texture *ballTexture = SDL_CreateTextureFromSurface(renderer, ball);  //ball is moving so it needs the renderer as a parameter.  it draws the surface on it
	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bg);
	SDL_Texture* batTexture = SDL_CreateTextureFromSurface(renderer, bat);

	SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
	
	while (!quit) {	//game loop
		EventHandler();
		SDL_Rect ballRect = { ballX, ballY, 20, 20 };
		SDL_Rect batRect = { batX, batY, 60, 30 };	//need rects inside your game loop
		moveBall();
		ballCollision();
		SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);  //reloading the backround image every loop
		SDL_RenderCopy(renderer, ballTexture, NULL, &ballRect);
		SDL_RenderCopy(renderer, batTexture, NULL, &batRect);
		SDL_RenderPresent(renderer);
		SDL_Delay(3);  //slowing the frame rate
		SDL_RenderClear(renderer);
	}
	
	SDL_Quit();

	return 0;
}