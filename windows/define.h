#ifndef __DEFINE_COMP308__
#define __DEFINE_COMP308__

// Default Window 
#define G308_WIN_WIDTH	640
#define G308_WIN_HEIGHT	480

// Projection parameters
#define G308_FOVY		20.0
#define G308_ZNEAR_3D	1
#define G308_ZFAR_3D	1000.0
#define G308_ZNEAR_2D	-50.0
#define G308_ZFAR_2D	50.0

// Shading mode : 0 Polygon, 1 Wireframe
#define G308_SHADE_POLYGON 0		
#define G308_SHADE_WIREFRAME 1

// Define number of vertex 
#define G308_NUM_VERTEX_PER_FACE 3 // Triangle = 3, Quad = 4 

// Define Basic Structures
struct G308_Point {
	float x;
	float y;
	float z;
};

struct G308_RGBA {
	float r;
	float g;
	float b;
	float a;
};

typedef G308_Point G308_Normal;

struct G308_UVcoord {

	float u;
	float v;
};

struct G308_Triangle {

	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
};

struct G308_Quad {

	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int v4;
};

#endif
