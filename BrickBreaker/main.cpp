#include <SDL.h>
#include "LTimer.h"
#include <iostream>
using namespace std;

const int SCREEN_FPS = 240;  //this type of small game is just going to have a high fps.  i could adjust velocities, but this causes jitter and the game just looks better this way
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//http://lazyfoo.net/tutorials/SDL/25_capping_frame_rate/index.php
LTimer fpsTimer;
LTimer capTimer;
int countedFrames = 0;

bool quit = false;
bool a = false;
SDL_Event event;
int ballX = 210;
int ballY = 200;
int ballVelX = 1;
int ballVelY = 2;

int bgW = 800;
int bgH = 600;

int brickW = 80;
int brickH = 35;
SDL_Surface* brick;  //might be some advantage to making this global...will follow up  
SDL_Texture* brickTexture;
SDL_Rect brickRect[3][7]; //pointer not needed if this is an array?
SDL_Rect ballRect;

int bgWMin = 0;
int bgHMin = 0;
int batX = bgW / 2;
int batY = bgH - 20;

int delete_brick_count = 0;
int no_of_bricks = 21;

bool aKeyDown = false;
bool dKeyDown = false;

void initializeBrick() {
	brickRect[0][0] = { 50, 50, brickW, brickH };
	brickRect[0][1] = { 150, 50, brickW, brickH };
	brickRect[0][2] = { 250, 50, brickW, brickH };
	brickRect[0][3] = { 350, 50, brickW, brickH };
	brickRect[0][4] = { 450, 50, brickW, brickH };
	brickRect[0][5] = { 550, 50, brickW, brickH };
	brickRect[0][6] = { 650, 50, brickW, brickH };
	brickRect[1][0] = { 50, 100, brickW, brickH };
	brickRect[1][1] = { 150, 100, brickW, brickH };
	brickRect[1][2] = { 250, 100, brickW, brickH };
	brickRect[1][3] = { 350, 100, brickW, brickH };
	brickRect[1][4] = { 450, 100, brickW, brickH };
	brickRect[1][5] = { 550, 100, brickW, brickH };
	brickRect[1][6] = { 650, 100, brickW, brickH };
	brickRect[2][0] = { 50, 150, brickW, brickH };
	brickRect[2][1] = { 150, 150, brickW, brickH };
	brickRect[2][2] = { 250, 150, brickW, brickH };
	brickRect[2][3] = { 350, 150, brickW, brickH };
	brickRect[2][4] = { 450, 150, brickW, brickH };
	brickRect[2][5] = { 550, 150, brickW, brickH };
	brickRect[2][6] = { 650, 150, brickW, brickH };
}

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
	if (ballY < bgHMin) {
		ballVelY = -ballVelY;
	}
	int ballScaling = 20;  //accounts for the outer age of the rectangle for ball
	if (ballY + ballScaling >= batY && ballY + ballScaling <= batY + 15 && ballX + ballScaling >= batX && ballX + ballScaling <= batX + 60) {
		ballVelY = -ballVelY;
	}
	
}

bool ball_brick_collision_detect(SDL_Rect brick , SDL_Rect ball) {  //brick , ball
	if (ballY < brick.y + brickH && ballX > brick.x && ballX < brick.x + brickW && ballVelY < 0) {
		return true;
	}
	if (ballY < brick.y + brickH && ballY > brick.y + brickH - 5 && ballX > brick.x && ballX < brick.x + brickW && ballVelY > 0) {
		return true;
	}

}

void ball_brick_collision() {
	
	//if (ballY < brickRect[2][3].y + brickH && ballX > brickRect[2][3].x && ballX < brickRect[2][3].x + brickW) {  //test for one brick
	//	ballVelY = -ballVelY;
	//	brickRect[2][3].x = 1000;
	//}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			a = ball_brick_collision_detect(brickRect[i][j], ballRect);
			if (a == true) {
				brickRect[i][j].x = 1000;
				delete_brick_count++;
				ballVelY = -ballVelY;
				a = false;  //not necessary i don't thinkg   TODO: ball sometimes breaks through more than one brick
			}
		}
	}
	
}


int main(int argc, char ** argv) {  //SDL requires command line arguments

	fpsTimer.start();

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("My first game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Rect ballRect = { 20, 20, 20, 20 };  //https://wiki.libsdl.org/SDL_Rect
	SDL_Rect bgRect = { 0,0, 800, 600 };
	SDL_Rect batRect = { batX, batY, 60, 20 };

	SDL_Surface* ball = SDL_LoadBMP("ball.bmp");			//https://stackoverflow.com/questions/21392755/difference-between-surface-and-texture-sdl-general
	SDL_SetColorKey(ball, SDL_TRUE, SDL_MapRGB(ball->format, 0, 0, 0));  //https://lazyfoo.net/tutorials/SDL/10_color_keying/index.php  keyed out black
	SDL_Surface* bg = SDL_LoadBMP("bg.bmp");
	SDL_Surface* bat = SDL_LoadBMP("bat.bmp");
	SDL_SetColorKey(bat, SDL_TRUE, SDL_MapRGB(bat->format, 0, 0, 0));
	brick = SDL_LoadBMP("brick.bmp");


	SDL_Texture* ballTexture = SDL_CreateTextureFromSurface(renderer, ball);  //ball is moving so it needs the renderer as a parameter.  it draws the surface on it
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bg);
	SDL_Texture* batTexture = SDL_CreateTextureFromSurface(renderer, bat);
	brickTexture = SDL_CreateTextureFromSurface(renderer, brick);

	initializeBrick();

	SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][0]);  
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][1]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][2]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][3]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][4]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][5]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][6]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][0]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][1]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][2]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][3]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][4]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][5]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][6]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][0]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][1]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][2]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][3]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][4]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][5]);
	SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][6]);
	
	while (!quit) {	//game loop
		//Start cap timer
		capTimer.start();

		EventHandler();

		//Calculate and correct fps
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}

		ballRect = { ballX, ballY, 20, 20 };
		SDL_Rect batRect = { batX, batY, 60, 20 };	//need rects inside your game loop
		
		ballCollision();
		ball_brick_collision();
		moveBall();

		if (ballY > bgH + 20) {
			quit = true;
		}


		if (delete_brick_count >= no_of_bricks) {
			cout << "You won!" << endl;
			cout << "destroying all textures and and freeing surfaces...." << endl;

			quit = true;
		}

		SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);  //reloading the backround image every loop
		SDL_RenderCopy(renderer, ballTexture, NULL, &ballRect);
		SDL_RenderCopy(renderer, batTexture, NULL, &batRect);

		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][0]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][1]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][2]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][3]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][4]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][5]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[0][6]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][0]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][1]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][2]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][3]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][4]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][5]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[1][6]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][0]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][1]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][2]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][3]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][4]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][5]);
		SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[2][6]);

		SDL_RenderPresent(renderer);
		//SDL_Delay(3);  //slowing the frame rate

		++countedFrames;

		  //If frame finished early
		int frameTicks = capTimer.getTicks();
		if (frameTicks < SCREEN_TICKS_PER_FRAME){
			//Wait remaining time
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	
		SDL_RenderClear(renderer);
	}

	SDL_DestroyTexture(batTexture);
	SDL_DestroyTexture(brickTexture);
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(ballTexture);
	SDL_FreeSurface(bat);
	SDL_FreeSurface(brick);
	SDL_FreeSurface(bg);
	SDL_FreeSurface(ball);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();

	return 0;
}