/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#pragma warning (disable: 4786)
#include "ModelPart.h"
#include "BoundingBox.h"
#include <cmath>
#include "util.h"

/* Constructor. primitiveType is a type you can give to glBegin,
vertexCount is the number of vertices in this part */
ModelPart::ModelPart(GLenum primitiveType, int vertexCount)
{
	_curPos = 0;
	_vertexCount = vertexCount;
	_pVertices = new GLfloat[_vertexCount * 3];
	_pNormals = new GLfloat[_vertexCount * 3];
	_primitiveType = primitiveType;

	for (int i = 0; i<4; i++)
		_color[i] = 1.0;

	_hasTexture = false;
	_pTexCoords = NULL;
}

// Destructor, free everything
ModelPart::~ModelPart(void)
{
	delete[] _pVertices;
	delete[] _pNormals;
	delete[] _pTexCoords;
}

/* Adds a new vertex including normal to the ModelPart object. The object
internally maintains a 'current vertex index' so this function can be
called sequentially. */

void ModelPart::addVertex(GLfloat x, GLfloat y, GLfloat z,
	GLfloat nx, GLfloat ny, GLfloat nz)
{
	if (_curPos < _vertexCount)
	{
		GLfloat *vertexBasePtr = _pVertices + _curPos * 3;
		GLfloat *normalBasePtr = _pNormals + _curPos * 3;
		vertexBasePtr[0] = x;
		vertexBasePtr[1] = y;
		vertexBasePtr[2] = z;

		normalBasePtr[0] = nx;
		normalBasePtr[1] = ny;
		normalBasePtr[2] = nz;

		_curPos++;
	}
}

// Returns a pointer to the vertex array (3 * number of vertices)
const GLfloat * ModelPart::getVertexPointer(void)
{
	return _pVertices;
}

// Returns a pointer to the normal array (3 * number of vertices)
const GLfloat * ModelPart::getNormalPointer(void)
{
	return _pNormals;
}

// Returns the primitive type (for glBegin)
GLenum ModelPart::getPrimitiveType(void)
{
	return _primitiveType;
}

// Sets the color of this part
void ModelPart::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
	_color[3] = a;
}


// Returns the color of this part
void ModelPart::getColor(GLfloat *pColor4)
{
	for (int i = 0; i<4; i++)
		pColor4[i] = _color[i];
}

// Returns the number of vertices in the model
int ModelPart::getVertexCount()
{
	return _vertexCount;
}
const GLfloat * ModelPart::getTextureCoordPointer(void)
{
	return _pTexCoords;
}


bool ModelPart::hasTexture(void) const
{
	return _hasTexture;
}

void ModelPart::genTexCoords(const BoundingBox &box)
{
	//const float PI = 3.141592654;
	if (_vertexCount == 0)
		return;

	// Determine center
	Vertex3 center = box.center();
	float cx = center.x(), cy = center.y(), cz = center.z();

	_pTexCoords = new GLfloat[_vertexCount * 2];
	for (int i = 0; i<_vertexCount; i++)
	{
		GLfloat *pPoint = _pVertices + 3 * i;
		GLfloat *pNormal = _pNormals + 3 * i;
		GLfloat *pTex = _pTexCoords + 2 * i;
		float x = pPoint[0], y = pPoint[1], z = pPoint[2];

		// Scale x,y,z to box
		x -= cx; y -= cy; z -= cz;
		x /= (box.dx() / 2.0f);
		y /= (box.dy() / 2.0f);
		z /= (box.dz() / 2.0f);

		// Get absolute normal value
		float nx = fabs(pNormal[0]), ny = fabs(pNormal[1]), nz = fabs(pNormal[2]);

		// Find the largest dimension
		float maxDim = maxVal(nx, maxVal(ny, nz));


		// Do a box mapping
		float s, t;
		if (maxDim == nx)
		{
			s = z;
			t = y;
		}
		else if (maxDim == ny)
		{
			s = z;
			t = x;
		}
		else
		{
			s = x;
			t = y;
		}

		pTex[0] = s;
		pTex[1] = t;
	}

	_hasTexture = true;

}
