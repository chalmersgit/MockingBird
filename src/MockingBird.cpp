/**
 * @file
 * @author  Andrew Chalmers and Richard Roberts
 * @version 1.0
 *
 * @section LICENSE
 *
 * TBD
 *
 * @section DESCRIPTION
 *
 * Mocking Bird is a cross-platform game engine
 */

// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //

// Display
#include "SDL.h"
#include "SDL_opengl.h"
#include "GL/glu.h"
#include "GL/glut.h"

// Standard
#include <iostream>
#include <string>
#include <math.h>

// User defined
#include "BananaClient.h"
#include "ObjReader.h"
#include "define.h"

// TMP globals for arcball
struct Keytracker {
	bool LALT = false;
	bool RALT = false;
	bool MOUSEDOWN = false;
	bool MOUSEUP = false;
	Point MOUSE_POS;
	Point MOUSE_VEL;
};
Keytracker keytracker;
Point arcballCenter{ WIN_WIDTH/2, WIN_HEIGHT/2, 0.0 };
Point eye{ 0.17, 0.8, 6.0 };
Point center{ 0.17, 0.8, 0.0 };
Point up{ 0.0, 1.0, 0.0 };
float arcballRadius = 500.0;
void calculateArcball();
void resetCamera();

void SetLight(){
	float direction[]		= {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[]	= {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]			= {0.2f, 0.2f, 0.2f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);

	glEnable(GL_LIGHT0);
}

void SetCamera(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (double) WIN_WIDTH / (double) WIN_HEIGHT, ZNEAR_3D, ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}

void resetCamera(){
	if((keytracker.LALT || keytracker.RALT) && keytracker.MOUSEDOWN){
		calculateArcball();
	}else{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}

void calculateArcball(){
	printf("Calculating arcball\n");

	Point centerToMousePos{
		(keytracker.MOUSE_POS.x - arcballCenter.x)/arcballRadius,
		(keytracker.MOUSE_POS.y - arcballCenter.y)/arcballRadius,
		0
	};
	float r1 = pow(centerToMousePos.x, 2) + pow(centerToMousePos.y, 2);
	if(r1 > 1.0){
		float s = 1.0/sqrt(r1);
		centerToMousePos.x = s*centerToMousePos.x;
		centerToMousePos.y = s*centerToMousePos.y;
		centerToMousePos.z = 0.0;
	}else{
		centerToMousePos.z = sqrt(1.0 - r1);
	}

	Point centerToMousePrevPos{
		((keytracker.MOUSE_POS.x - keytracker.MOUSE_VEL.x) - arcballCenter.x)/arcballRadius,
		((keytracker.MOUSE_POS.y - keytracker.MOUSE_VEL.y) - arcballCenter.y)/arcballRadius,
		0
	};
	float r2 = pow(centerToMousePrevPos.x, 2) + pow(centerToMousePrevPos.y, 2);
	if(r2 > 1.0){
		float s = 1.0/sqrt(r2);
		centerToMousePrevPos.x = s*centerToMousePrevPos.x;
		centerToMousePrevPos.y = s*centerToMousePrevPos.y;
		centerToMousePrevPos.z = 0.0;
	}else{
		centerToMousePrevPos.z = sqrt(1.0 - r2);
	}

	normalise(centerToMousePos);
	normalise(centerToMousePrevPos);

	//Point q = cross(centerToMousePos, centerToMousePrevPos);
	//float w = dot(centerToMousePos, centerToMousePrevPos);

	Point rotationAxis = cross(centerToMousePos, centerToMousePrevPos);

	float dotValue = dot(centerToMousePos, centerToMousePrevPos);
	float angle = acos(dotValue) * (180/M_PI);

	/*
	log(centerToMousePos);
	log(centerToMousePrevPos);
	log(rotationAxis);
	printf("%f, %f\n\n", dotValue, angle);
	 */

	//glRotatef(angle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
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
	SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, 32, SDL_OPENGL);
	glClearColor(1,1,1,1);
	glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);

	//Our instances
	//TMP ObjLoader
	ObjReader* obj = new ObjReader;
	obj->ReadOBJ(args[1]);
	obj->CreateGLPolyGeometry();
	obj->CreateGLWireGeometry();

	// TMP Network obj
	BananaClient* bc = new BananaClient();

	SetLight();
	SetCamera();

	//Event handler
	SDL_Event event;
	//Main game loop
	bool isRunning = true;
	while ( isRunning ){
		// Events
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
				case SDLK_LALT:
					keytracker.LALT = true;
					break;
				case SDLK_RALT:
					keytracker.RALT = true;
					break;
				case SDLK_w:
					glClearColor(1,1,1,1);
					break;
				case SDLK_r:
					glClearColor(1,0,0,1);
					break;
				case SDLK_g:
					glClearColor(0,1,0,1);
					break;
				case SDLK_b:
					glClearColor(0,0,1,1);
					break;
				default:
					break;
				}
				break;

			case SDL_KEYUP:
				switch( event.key.keysym.sym ){
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				case SDLK_LALT:
					keytracker.LALT = false;
					break;
				case SDLK_RALT:
					keytracker.RALT = false;
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				//printf("Mouse moved by %d,%d to (%d,%d)\n",
				//		event.motion.xrel, event.motion.yrel,
				//		event.motion.x, event.motion.y);

				keytracker.MOUSE_POS.x = event.motion.x;
				keytracker.MOUSE_POS.y = event.motion.y;
				keytracker.MOUSE_VEL.x = event.motion.xrel;
				keytracker.MOUSE_VEL.y = event.motion.yrel;

				break;
			case SDL_MOUSEBUTTONDOWN:
				keytracker.MOUSEDOWN = true;
				//printf("Mouse button down %d pressed at (%d,%d)\n",
				//		event.button.button, event.button.x, event.button.y);
	                break;
			case SDL_MOUSEBUTTONUP:
				keytracker.MOUSEDOWN = false;
				//printf("Mouse button up %d pressed at (%d,%d)\n",
				//		event.button.button, event.button.x, event.button.y);
	                break;
			case SDL_QUIT:
				isRunning = false;
				break;
			default:
				break;
		    }
		}

		//SERVER
		//bc->loop();

		//LOGIC
		resetCamera();

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
	if(bc != NULL) delete bc;

	//Quit SDL
	SDL_Quit();

	return 0;
}
