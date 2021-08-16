#include "MyGL.h"
#include <glm/gtc/matrix_access.hpp>
//------------------------------------------------------------------------------
MyGL::MyGL()
    :
    _doTriangulate( false ),
    _doRasterize( false ) {
}

//------------------------------------------------------------------------------
MyGL::~MyGL()
{}

//------------------------------------------------------------------------------
bool MyGL::TriangulatePolygon( const vector<GLVertex> &polygonVerts,
                               vector<GLVertex> &triangleVerts ) {
    if( !_doTriangulate )
        return false;

    //
    // YOUR CODE HERE
    //

    // Implement triangulation here.
    // Keep in mind that the color of the first scene (F1) will depend on your implementation.
    // You must set it right so that it should not feel so "glitchy".
	int i,n = polygonVerts.size();
	if (n > 2) {
		for (i = 2;i < n;++i) {
			triangleVerts.push_back(polygonVerts[0]);
			triangleVerts.push_back(polygonVerts[i - 1]);
			triangleVerts.push_back(polygonVerts[i]);
		}
	}

	return true;
}

//------------------------------------------------------------------------------
bool MyGL::RasterizeTriangle(GLVertex verts[3]) {
	if (!_doRasterize)
		return false;

	//
	// YOUR CODE HERE
	//

	// Implement rasterization here. You are NOT required to implement wireframe mode (i.e. 'w' key mode).
	int i, j, k, w, h;
	frameBuffer.GetSize(w, h);
	glm::vec3 v[3], e[3], z;
	for (i = 0;i < 3;++i)v[i] = glm::vec3(glm::vec2(verts[i].position), 1),z[i]=verts[i].position[2];
	for (i = 0;i < 3;++i)e[i] = glm::cross(v[i], v[(i + 1) % 3]);
	if (triangleCount < 2)z = glm::vec3(1, 1, 1);
	float s = e[0][2] + e[1][2] + e[2][2];
	if (s == 0 || (cullFaceEnabled && s < 0))return true;
	++triangleCount;
	glm::mat3 E(e[1], e[2], e[0]);
	glm::mat4x3 C;
	for (i = 0;i < 4;++i)C[i]=glm::vec3(verts[0].color[i], verts[1].color[i], verts[2].color[i]);
	z = E * z;
	C = E * C;
	for (i = 1;i < h-1;++i) {
		for (j = 1;j < w-1;++j) {
			glm::vec3 p(i + 0.5, j + 0.5, 1);
			for (k = 0;k < 3;++k) {
				float d = glm::dot(e[k], p);
				bool t = e[k][0] == 0 ? e[k][1] > 0:e[k][0] > 0;
				if ((d < 0 && s>0) || (d > 0 && s < 0) || (d == 0 && !t))break;
			}
			if (k < 3)continue;
			float z_tri = glm::dot(z, p) / s;
			if (triangleCount > 2 && (z_tri < 0 || frameBuffer.GetDepth(i, j) <= z_tri))continue;
			glm::vec4 color=p*C/s;
			frameBuffer.SetPixel(i, j, color);
			frameBuffer.SetDepth(i,j,z_tri);
		}
	}
	return true;
}

