#pragma once

#include "define.h"
#include <GL/glut.h>

class ObjReader
{
public:
	ObjReader(void);
	~ObjReader(void);

	int ReadOBJ(const char* filename);
	void CreateGLPolyGeometry();
	void CreateGLWireGeometry();
	void RenderGeometry();
	void toggleMode();

private:
	int numVert, numNorm, numUV, numFace;

	// Array for Geometry
	Point* mVertexArray;
	Normal* mNormalArray;
	Triangle* mTriangles;
	UVcoord* mUVArray;

	// Data for Geoemetry
	int mNumPoint;
	int mNumUV;
	int mNumPolygon;

	// Mode for wireframe or polygon display lists
	int mode;
	int mGeomListPoly;
	int mGeomListWire;


};


