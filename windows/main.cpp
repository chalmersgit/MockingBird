#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <iostream>

// My definition 
#include "define.h"
#include "ObjReader.h"


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

/*
Windows specific display
Reference: 
http://msdn.microsoft.com/en-us/library/windows/desktop/ff381409(v=vs.85).aspx
http://bobobobo.wordpress.com/2008/02/11/opengl-in-a-proper-windows-app-no-glut/

Overview:
|---------|  draws to   |-------|  copied out   |---------|  shows in  |-----------|
|         | ==========> |       | ============> |         | =========> |           |
|---------|             |-------|               |---------|            |-----------|
  OPENGL                  HGLRC                     HDC                 application
 FUNCTION                                                                 window
  CALLS               
*/

//Structs
struct Globals
{
	HINSTANCE hInstance;    // window app instance
	HWND hwnd;				// handle for the window
	HDC   hdc;				// handle to device context
	HGLRC hglrc;			// handle to OpenGL rendering context
	int width, height;      // window width/height
};

//Global fields
Globals g;
ObjReader* obj = NULL;
int width = 600;
int height = 400;
int offsetx = 50;
int offsety = 50;


// Function prototypes
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow );
void draw();
 


#if defined(_WIN32)
// Entry point
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow )
{

	g.hInstance = hInstance;

	// Create Window

	// Create the WNDCLASS
	WNDCLASS wc;
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );         
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );     
	wc.hInstance = hInstance;         
	wc.lpfnWndProc = WndProc;         
	wc.lpszClassName = TEXT("Windows Display");
	wc.lpszMenuName = 0; 
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClass(&wc);
	
	//Display width and height, with offset
	RECT rect;
	SetRect( &rect, offsetx, offsety, width+offsetx, height+offsety ); 
	g.width = width;
	g.height = height;
	AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, false );
	
	// Create window
	g.hwnd = CreateWindow(TEXT("Windows Display"),
						  TEXT("Mocking Bird"),
						  WS_OVERLAPPEDWINDOW,
						  rect.left, rect.top,  // adjusted x, y positions
						  rect.right - rect.left, rect.bottom - rect.top,  // adjusted width and height
						  NULL, NULL,
						  hInstance, NULL);

	// check if create window is successful
	if( g.hwnd == NULL )
	{
		FatalAppExit( NULL, TEXT("Windows CreateWindow() failed") );
	}
	
	ShowWindow( g.hwnd, iCmdShow );

	// Get Device Context of window
	g.hdc = GetDC( g.hwnd );

	// Set pixel format of hdc
	PIXELFORMATDESCRIPTOR pfd = { 0 };  

	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );  
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL |  PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;   

	pfd.iPixelType = PFD_TYPE_RGBA;    
	pfd.cColorBits = 24;       
	pfd.cDepthBits = 32; 

	int chosenPixelFormat = ChoosePixelFormat( g.hdc, &pfd );
	if( chosenPixelFormat == 0 )
	{
		FatalAppExit( NULL, TEXT("Windows ChoosePixelFormat() failed") );
	}
	
	int result = SetPixelFormat( g.hdc, chosenPixelFormat, &pfd );
	if (result == NULL)
	{
		FatalAppExit( NULL, TEXT("Windows SetPixelFormat() failed") );
	}

	// Create rendering context
	g.hglrc = wglCreateContext( g.hdc );

	// Connect rendering context with device context
	wglMakeCurrent( g.hdc, g.hglrc );


	//Obj Reader
	OutputDebugStringW(L"Reading Obj\n");
	//obj = new ObjReader;
	//obj->ReadOBJ(argv[1]);
	//obj->CreateGLPolyGeometry();  
	//obj->CreateGLWireGeometry();
	
	// loop
	MSG msg;
	while(1){
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ){
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else{
			draw();
		}
	}

	// clean up
	if(obj != NULL) delete obj;

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( g.hglrc );
	ReleaseDC( g.hwnd, g.hdc );
	
	// fade exit
	AnimateWindow( g.hwnd, 200, AW_HIDE | AW_BLEND );

	return 0;
}
#endif

void draw() 
{
	glViewport(0, 0, g.width, g.height);

	//projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0,(float)g.width/(float)g.height, 1, 1000);

	//Draw Something

	//End of drawing, swap buffers
	SwapBuffers(g.hdc);
}

LRESULT CALLBACK WndProc(   HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ) 
{
	switch( message )
	{
	case WM_CREATE:
		Beep( 750, 300 );
		return 0;
		break;

	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint( hwnd, &ps );
			EndPaint( hwnd, &ps );
		}
		return 0;
		break;

	case WM_KEYDOWN:
		switch( wparam )
		{
		case VK_ESCAPE:
			PostQuitMessage( 0 );
			break;
		default:
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		return 0;
		break;
	}
 
	return DefWindowProc( hwnd, message, wparam, lparam );
}


