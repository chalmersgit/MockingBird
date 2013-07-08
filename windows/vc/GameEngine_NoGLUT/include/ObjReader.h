#pragma once

#include "define.h"
#include <GL/glut.h>

class ObjReader
{
public:
	ObjReader(void);
	~ObjReader(void);

	// Array for Geometry
	Point* m_pVertexArray;		// Vertex Array
	Normal* m_pNormalArray;	// Normal Array
	Triangle* m_pTriangles;	// Triangle Array
	UVcoord* m_pUVArray;	    // Texture Coordinate Array

	// Data for Geoemetry
	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumPolygon;

	int mode; // Which mode to display

	// Data for Rendering
	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe

public:
	int ReadOBJ(const char* filename);

	void CreateGLPolyGeometry();
	void CreateGLWireGeometry();

	void RenderGeometry();     // mode : SHADE_POLYGON, SHADE_WIREFRAME
	void toggleMode(); //Switch between showing filled polygons and wireframes
};