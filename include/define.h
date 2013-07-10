#ifndef __DEFINE_MOCKINGBIRD__
#define __DEFINE_MOCKINGBIRD__

// Default Window
#define WIN_WIDTH	640
#define WIN_HEIGHT	480

// Projection parameters
#define FOVY		20.0
#define ZNEAR_3D	1
#define ZFAR_3D	1000.0
#define ZNEAR_2D	-50.0
#define ZFAR_2D	50.0

// Shading mode
#define SHADE_POLYGON 0
#define SHADE_WIREFRAME 1

#define NUM_VERTEX_PER_FACE 3 // Triangle = 3, Quad = 4

// Define Basic Structures
struct Point {
	float x;
	float y;
	float z;
};

struct RGBA {
	float r;
	float g;
	float b;
	float a;
};

typedef Point Normal;

struct UVcoord {
	float u;
	float v;
};

struct Triangle {
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
};

struct Quad {
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int v4;
};

#endif


