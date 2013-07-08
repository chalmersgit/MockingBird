#include "ObjReader.h"
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include <iostream> 

using namespace std;
int numVert, numNorm, numUV, numFace;
ObjReader::ObjReader(void) {
	m_pVertexArray = NULL;
	m_pNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;

	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;
}

ObjReader::~ObjReader(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
}

//-------------------------------------------------------
// Reads .obj file
// Triangle faces consist of vertex v, texture coordinate vt, and normal vn
// e.g. f v1/vt1/vn1 v2/vt1/vn2 v3/vt3/vn3 
//--------------------------------------------------------
int ObjReader::ReadOBJ(const char *filename) {
 
	//cout << "\nReading Object.\n";	

	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;

	float x, y, z;
	float u, v;

	G308_Normal* lNormalArray = NULL; // local Array
	G308_UVcoord* lUVArray = NULL; // local Array

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;

	OutputDebugStringW(L"Number of Point %d, UV %d, Normal %d, Face %d\n");
	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV, numNorm, numFace);
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumPoint];

	lNormalArray = new G308_Normal[numNorm]; 

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumPoint];

	lUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				lUVArray[numUV].u = u;
				lUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				lNormalArray[numNorm].x = x;
				lNormalArray[numNorm].y = y;
				lNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if(numUV == 0 && numNorm == 0){
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}
			else if(numUV == 0 && numNorm > 0){
				if(sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3) != 6){
					sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
				}
			}
			else{
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			}
			// Vertex index for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Sort array order in normal array
			if (numNorm != 0) {
				m_pNormalArray[v1 - 1] = lNormalArray[n1 - 1];
				m_pNormalArray[v2 - 1] = lNormalArray[n2 - 1];
				m_pNormalArray[v3 - 1] = lNormalArray[n3 - 1];
			}

			// Sort array order in texture array
			if (numUV != 0) {
				m_pUVArray[v1 - 1] = lUVArray[t1 - 1];
				m_pUVArray[v2 - 1] = lUVArray[t2 - 1];
				m_pUVArray[v3 - 1] = lUVArray[t3 - 1];
			}

			numFace++;
			break;
		default:
			break;
		}
	}
	int camVal = 0;
	if(numUV == 0 && numNorm == 0){
		for(int i = 0; i < m_nNumPolygon;){
			//Set Vector U to (Triangle.p2 minus Triangle.p1)
			float uX = m_pVertexArray[m_pTriangles[i].v2].x - m_pVertexArray[m_pTriangles[i].v1].x;
			float uY = m_pVertexArray[m_pTriangles[i].v2].y - m_pVertexArray[m_pTriangles[i].v1].y;
			float uZ = m_pVertexArray[m_pTriangles[i].v2].z - m_pVertexArray[m_pTriangles[i].v1].z;

			//Set Vector V to (Triangle.p3 minus Triangle.p1)
			float vX = m_pVertexArray[m_pTriangles[i].v3].x - m_pVertexArray[m_pTriangles[i].v1].x;
			float vY = m_pVertexArray[m_pTriangles[i].v3].y - m_pVertexArray[m_pTriangles[i].v1].y;
			float vZ = m_pVertexArray[m_pTriangles[i].v3].z - m_pVertexArray[m_pTriangles[i].v1].z;

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v1].x += (uY * vZ) - (uZ*vY);

			//Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
			m_pNormalArray[m_pTriangles[i].v1].y += (uZ*vX) - (uX*vZ);

			//Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)
			m_pNormalArray[m_pTriangles[i].v1].z += (uX*vY) - (uY*vX);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v2].x += (uY * vZ) - (uZ*vY);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v2].y += (uZ*vX) - (uX*vZ);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v2].z += (uX*vY) - (uY*vX);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v3].x += (uY * vZ) - (uZ*vY);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v3].y += (uZ*vX) - (uX*vZ);

			//Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
			m_pNormalArray[m_pTriangles[i].v3].z += (uX*vY) - (uY*vX);

			i++;
		}
		camVal = 1;
		glEnable(GL_NORMALIZE);
	}

	// Delete local array
	if (lNormalArray != NULL)
		delete[] lNormalArray;
	if (lUVArray != NULL)
		delete[] lUVArray;

	fclose(fp);
	printf("Reading OBJ file is DONE.\n");

	return camVal;

}

void ObjReader::CreateGLPolyGeometry() {
	cout << "\nCreating PolyGeometry!\n";
	if (m_glGeomListPoly != 0){
		glDeleteLists(m_glGeomListPoly, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);
	
	//glColor3f(0.66 , 0.66, 0.66);
	//glColor3f(0.54, 0.27, 0.07);
	//glColor3f(1,0,0);
	glBegin(GL_TRIANGLES);

	if(numUV == 0 && numNorm == 0){
		//Calculating dragon colour
		glColor3f(0.6, 0.2, 0.8);
	}
	else if(numUV == 0 && numNorm > 0){
		//Calculating delicious choc bunny
		glColor3f(0.54, 0.27, 0.07); 
	}
	else{
		//Calculating teapot or sphere colour
		glColor3f(1,0,0);
	}

	for(int i = 0; i < m_nNumPolygon;){
		glNormal3f(m_pNormalArray[m_pTriangles[i].v1].x, m_pNormalArray[m_pTriangles[i].v1].y, m_pNormalArray[m_pTriangles[i].v1].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v1].x, m_pVertexArray[m_pTriangles[i].v1].y, m_pVertexArray[m_pTriangles[i].v1].z);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v2].x, m_pNormalArray[m_pTriangles[i].v2].y, m_pNormalArray[m_pTriangles[i].v2].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x, m_pVertexArray[m_pTriangles[i].v2].y, m_pVertexArray[m_pTriangles[i].v2].z);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v3].x, m_pNormalArray[m_pTriangles[i].v3].y, m_pNormalArray[m_pTriangles[i].v3].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v3].x, m_pVertexArray[m_pTriangles[i].v3].y, m_pVertexArray[m_pTriangles[i].v3].z);

		i++;
	}
	glEnd();
	glColor3f(0, 0, 0);

	glEndList();
}

//--------------------------------------------------------------
// [Assignment1] 
// Fill the following function to create display list
// of the obj file to show it as wireframe 
//--------------------------------------------------------------
void ObjReader::CreateGLWireGeometry() {
	cout << "\nCreating WireGeometry!\n";

	if (m_glGeomListWire != 0){
		glDeleteLists(m_glGeomListWire, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListWire = glGenLists(1);
	glNewList(m_glGeomListWire, GL_COMPILE);

	//----------------------
	//YOUR CODE GOES HERE
	// .....

	glColor3f(0, 1, 1);

	for(int i = 0; i < m_nNumPolygon;){
		glBegin(GL_LINE_LOOP);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v1].x, m_pNormalArray[m_pTriangles[i].v1].y, m_pNormalArray[m_pTriangles[i].v1].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v1].x, m_pVertexArray[m_pTriangles[i].v1].y, m_pVertexArray[m_pTriangles[i].v1].z);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v2].x, m_pNormalArray[m_pTriangles[i].v2].y, m_pNormalArray[m_pTriangles[i].v2].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x, m_pVertexArray[m_pTriangles[i].v2].y, m_pVertexArray[m_pTriangles[i].v2].z);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v2].x, m_pNormalArray[m_pTriangles[i].v2].y, m_pNormalArray[m_pTriangles[i].v2].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x, m_pVertexArray[m_pTriangles[i].v2].y, m_pVertexArray[m_pTriangles[i].v2].z);

		glNormal3f(m_pNormalArray[m_pTriangles[i].v3].x, m_pNormalArray[m_pTriangles[i].v3].y, m_pNormalArray[m_pTriangles[i].v3].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v3].x, m_pVertexArray[m_pTriangles[i].v3].y, m_pVertexArray[m_pTriangles[i].v3].z);

		i++;
		glEnd();
	}

	glColor3f(0, 0, 0);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEndList();

}
void ObjReader::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
	} else {
		mode = G308_SHADE_POLYGON;
	}
}

void ObjReader::RenderGeometry() {

	if (mode == G308_SHADE_POLYGON) {
		//-------------------------------------------------------------
		// [Assignment1] : Uncomment the next line
		glCallList(m_glGeomListPoly);
		//glShadeModel(GL_SMOOTH);
		//glutSolidSphere(5.0, 30.0, 30.0);
		//glShadeModel(GL_SMOOTH);
		//glutWireTeapot(5.0);
		//-------------------------------------------------------------

		// [Assignment1] : Comment out the following two lines
		//makes teapot shaded polygon
		//glShadeModel(GL_SMOOTH);
		//glutSolidTeapot(5.0);
	} else if (mode == G308_SHADE_WIREFRAME) {
		//-------------------------------------------------------------
		// [Assignment1] : Uncomment the next line
		glCallList(m_glGeomListWire);
		//glShadeModel(GL_SMOOTH);
		//glutWireSphere(5.0, 30.0, 30.0);

		//-------------------------------------------------------------

		// [Assignment1] Comment out the following two lines
		//makes teapot wireframe
		//glShadeModel(GL_SMOOTH);
		//glutWireTeapot(5.0);
	} else {
		printf("Warning: Wrong Shading Mode. \n");
	}
}