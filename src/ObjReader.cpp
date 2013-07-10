#include "ObjReader.h"
#include <stdio.h>
#include <iostream>

ObjReader::ObjReader(void) {
	mVertexArray = NULL;
	mNormalArray = NULL;
	mUVArray = NULL;
	mTriangles = NULL;

	mode = SHADE_POLYGON;

	mNumPoint = mNumUV = mNumPolygon = 0;
	mGeomListPoly = mGeomListWire = 0;
}

ObjReader::~ObjReader(void) {
	if (mVertexArray != NULL)
		delete[] mVertexArray;
	if (mNormalArray != NULL)
		delete[] mNormalArray;
	if (mUVArray != NULL)
		delete[] mUVArray;
	if (mTriangles != NULL)
		delete[] mTriangles;
}

//-------------------------------------------------------
// Reads .obj file
// Triangle faces consist of vertex v, texture coordinate vt, and normal vn
// e.g. f v1/vt1/vn1 v2/vt1/vn2 v3/vt3/vn3
//--------------------------------------------------------
int ObjReader::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;

	float x, y, z;
	float u, v;

	Normal* lNormalArray = NULL; // local Array
	UVcoord* lUVArray = NULL; // local Array

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

	mNumPoint = numVert;
	mNumUV = numUV;
	mNumPolygon = numFace;

	printf("Points: %d, UVs: %d, Normals: %d, Faces: %d\n", numVert, numUV, numNorm, numFace);
	if (mVertexArray != NULL)
		delete[] mVertexArray;
	mVertexArray = new Point[mNumPoint];

	if (mNormalArray != NULL)
		delete[] mNormalArray;
	mNormalArray = new Normal[mNumPoint];

	lNormalArray = new Normal[numNorm];

	if (mUVArray != NULL)
		delete[] mUVArray;
	mUVArray = new UVcoord[mNumPoint];

	lUVArray = new UVcoord[mNumUV];

	if (mTriangles != NULL)
		delete[] mTriangles;
	mTriangles = new Triangle[mNumPolygon];

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
				mVertexArray[numVert].x = x;
				mVertexArray[numVert].y = y;
				mVertexArray[numVert].z = z;
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
				mTriangles[numFace].v1 = v1 - 1;
				mTriangles[numFace].v2 = v2 - 1;
				mTriangles[numFace].v3 = v3 - 1;
			}

			// Sort array order in normal array
			if (numNorm != 0) {
				mNormalArray[v1 - 1] = lNormalArray[n1 - 1];
				mNormalArray[v2 - 1] = lNormalArray[n2 - 1];
				mNormalArray[v3 - 1] = lNormalArray[n3 - 1];
			}

			// Sort array order in texture array
			if (numUV != 0) {
				mUVArray[v1 - 1] = lUVArray[t1 - 1];
				mUVArray[v2 - 1] = lUVArray[t2 - 1];
				mUVArray[v3 - 1] = lUVArray[t3 - 1];
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	//If no normals were provided, then calculate them.
	//Note, revise the numUV check
	if(numUV == 0 && numNorm == 0){
		for(int i = 0; i < mNumPolygon;){
			//Set Vector U to (Triangle.p2 minus Triangle.p1)
			float uX = mVertexArray[mTriangles[i].v2].x - mVertexArray[mTriangles[i].v1].x;
			float uY = mVertexArray[mTriangles[i].v2].y - mVertexArray[mTriangles[i].v1].y;
			float uZ = mVertexArray[mTriangles[i].v2].z - mVertexArray[mTriangles[i].v1].z;

			//Set Vector V to (Triangle.p3 minus Triangle.p1)
			float vX = mVertexArray[mTriangles[i].v3].x - mVertexArray[mTriangles[i].v1].x;
			float vY = mVertexArray[mTriangles[i].v3].y - mVertexArray[mTriangles[i].v1].y;
			float vZ = mVertexArray[mTriangles[i].v3].z - mVertexArray[mTriangles[i].v1].z;

			//Set normals of each face
			mNormalArray[mTriangles[i].v1].x += (uY * vZ) - (uZ*vY);
			mNormalArray[mTriangles[i].v1].y += (uZ*vX) - (uX*vZ);
			mNormalArray[mTriangles[i].v1].z += (uX*vY) - (uY*vX);

			mNormalArray[mTriangles[i].v2].x += (uY * vZ) - (uZ*vY);
			mNormalArray[mTriangles[i].v2].y += (uZ*vX) - (uX*vZ);
			mNormalArray[mTriangles[i].v2].z += (uX*vY) - (uY*vX);

			mNormalArray[mTriangles[i].v3].x += (uY * vZ) - (uZ*vY);
			mNormalArray[mTriangles[i].v3].y += (uZ*vX) - (uX*vZ);
			mNormalArray[mTriangles[i].v3].z += (uX*vY) - (uY*vX);

			i++;
		}
		glEnable(GL_NORMALIZE);
	}

	// Delete local array
	if (lNormalArray != NULL)
		delete[] lNormalArray;
	if (lUVArray != NULL)
		delete[] lUVArray;

	fclose(fp);

	printf("Reading Obj: Success\n");
	return 1;

}

void ObjReader::CreateGLPolyGeometry() {
	printf("Creating PolyGeometry\n");
	if (mGeomListPoly != 0){
		glDeleteLists(mGeomListPoly, 1);
	}
	mGeomListPoly = glGenLists(1);
	glNewList(mGeomListPoly, GL_COMPILE);

	glBegin(GL_TRIANGLES);


	glColor3f(0.5, 0.5, 0.5);

	for(int i = 0; i < mNumPolygon;){
		glNormal3f(mNormalArray[mTriangles[i].v1].x, mNormalArray[mTriangles[i].v1].y, mNormalArray[mTriangles[i].v1].z);
		glVertex3f(mVertexArray[mTriangles[i].v1].x, mVertexArray[mTriangles[i].v1].y, mVertexArray[mTriangles[i].v1].z);

		glNormal3f(mNormalArray[mTriangles[i].v2].x, mNormalArray[mTriangles[i].v2].y, mNormalArray[mTriangles[i].v2].z);
		glVertex3f(mVertexArray[mTriangles[i].v2].x, mVertexArray[mTriangles[i].v2].y, mVertexArray[mTriangles[i].v2].z);

		glNormal3f(mNormalArray[mTriangles[i].v3].x, mNormalArray[mTriangles[i].v3].y, mNormalArray[mTriangles[i].v3].z);
		glVertex3f(mVertexArray[mTriangles[i].v3].x, mVertexArray[mTriangles[i].v3].y, mVertexArray[mTriangles[i].v3].z);

		i++;
	}
	glEnd();
	glColor3f(0, 0, 0);

	glEndList();
}

void ObjReader::CreateGLWireGeometry() {
	printf("Creating WireGeometry\n\n");

	if (mGeomListWire != 0){
		glDeleteLists(mGeomListWire, 1);
	}

	mGeomListWire = glGenLists(1);
	glNewList(mGeomListWire, GL_COMPILE);

	glColor3f(0, 1, 1);

	for(int i = 0; i < mNumPolygon;){
		glBegin(GL_LINE_LOOP);

		glNormal3f(mNormalArray[mTriangles[i].v1].x, mNormalArray[mTriangles[i].v1].y, mNormalArray[mTriangles[i].v1].z);
		glVertex3f(mVertexArray[mTriangles[i].v1].x, mVertexArray[mTriangles[i].v1].y, mVertexArray[mTriangles[i].v1].z);

		glNormal3f(mNormalArray[mTriangles[i].v2].x, mNormalArray[mTriangles[i].v2].y, mNormalArray[mTriangles[i].v2].z);
		glVertex3f(mVertexArray[mTriangles[i].v2].x, mVertexArray[mTriangles[i].v2].y, mVertexArray[mTriangles[i].v2].z);

		glNormal3f(mNormalArray[mTriangles[i].v2].x, mNormalArray[mTriangles[i].v2].y, mNormalArray[mTriangles[i].v2].z);
		glVertex3f(mVertexArray[mTriangles[i].v2].x, mVertexArray[mTriangles[i].v2].y, mVertexArray[mTriangles[i].v2].z);

		glNormal3f(mNormalArray[mTriangles[i].v3].x, mNormalArray[mTriangles[i].v3].y, mNormalArray[mTriangles[i].v3].z);
		glVertex3f(mVertexArray[mTriangles[i].v3].x, mVertexArray[mTriangles[i].v3].y, mVertexArray[mTriangles[i].v3].z);

		i++;
		glEnd();
	}

	glColor3f(0, 0, 0);
	glEndList();

}
void ObjReader::toggleMode() {
	if (mode == SHADE_POLYGON) {
		mode = SHADE_WIREFRAME;
	} else {
		mode = SHADE_POLYGON;
	}
}

void ObjReader::RenderGeometry() {
	if (mode == SHADE_POLYGON) {
		glCallList(mGeomListPoly);
	} else if (mode == SHADE_WIREFRAME) {
		glCallList(mGeomListWire);
	} else {
		printf("Warning: Wrong Shading Mode. \n");
	}
}
