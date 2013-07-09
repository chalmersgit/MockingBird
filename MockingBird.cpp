#include "SDL.h"
#include "SDL_opengl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#include <iostream>
#include <string>

//Our Definitions
#include "ObjReader.h"
#include "define.h"

//Temp: Our globals (shouldn't have this)
int width = 600;
int height = 400;
ObjReader* obj = NULL;

void SetCamera(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (double) width / (double) height, ZNEAR_3D, ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.17, 0.8, 6.0, 0.17, 0.8, 0.0, 0.0, 1.0, 0.0);
}

void SetLight(){
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]       = {0.2f, 0.2f, 0.2f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);


	glEnable(GL_LIGHT0);
}

int main(int agrc, char* args[] ){

	SDL_Init(SDL_INIT_EVERYTHING);


	//Set OpenGL memory usage
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	//Window
	SDL_WM_SetCaption( "Mocking Bird", NULL);
	SDL_SetVideoMode(600, 400, 32, SDL_OPENGL);
	glClearColor(1,1,1,1);
	glViewport(0,0,600,400);

	////2D rendering
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glDisable(GL_DEPTH_TEST);

	//Temp: ObjLoader
	obj = new ObjReader;
	obj->ReadOBJ(args[1]);
	obj->CreateGLPolyGeometry();
	obj->CreateGLWireGeometry();

	SetLight();
	SetCamera();

	//Event handler
	SDL_Event event;
	//Main game loop
	bool isRunning = true;
	while ( isRunning ){
		// Events
		while( SDL_PollEvent(&event) ){
			if( event.type == SDL_QUIT ){
        	  isRunning = false;
			}
			if( event.type = SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE ){
				isRunning = false;
			}

			if( event.type = SDL_KEYUP & event.key.keysym.sym == SDLK_w ){
				glClearColor(1,1,1,1);
			}
			if( event.type = SDL_KEYUP & event.key.keysym.sym == SDLK_r ){
				glClearColor(1,0,0,1);
			}
			if( event.type = SDL_KEYUP & event.key.keysym.sym == SDLK_g ){
				glClearColor(0,1,0,1);
			}
			if( event.type = SDL_KEYUP & event.key.keysym.sym == SDLK_b ){
				glClearColor(0,0,1,1);
			}

			if( event.type = SDL_KEYUP & event.key.keysym.sym == SDLK_t ){
				obj->toggleMode();
			}

		}
		//LOGIC

		//RENDERING
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		//Object to display
		obj->RenderGeometry();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);

		SDL_GL_SwapBuffers();
	}

	//Clean up
	if(obj != NULL) delete obj;

	//Quit SDL
	SDL_Quit();

	return 0;
}
