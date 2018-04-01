/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#ifndef _CG_MODELPART_
#define _CG_MODELPART_
#include "Globals.h"
#include "DisallowCopy.h"
//#include "glut.h"

class BoundingBox;

class ModelPart : private DisallowCopy
{
public:
	ModelPart(GLenum primitiveType, int vertexCount);
	virtual ~ModelPart(void);
	void addVertex(GLfloat x, GLfloat y, GLfloat z,
		GLfloat nx, GLfloat ny, GLfloat nz);

	const GLfloat *getVertexPointer(void);
	const GLfloat *getNormalPointer(void);
	GLenum getPrimitiveType(void);
	void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void getColor(GLfloat *pColor4);
	int getVertexCount();
	const GLfloat * getTextureCoordPointer(void);
	bool hasTexture(void) const;

	// Generate box mapping texture coordinates
	void	genTexCoords(const BoundingBox &box);
private:

	int		_vertexCount;
	int		_curPos;
	GLfloat *_pVertices;
	GLfloat *_pNormals;
	GLfloat *_pTexCoords;
	GLfloat _color[4];
	GLenum	_primitiveType;
	bool	_hasTexture;
};

#endif