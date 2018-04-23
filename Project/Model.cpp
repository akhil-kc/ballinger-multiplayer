/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#pragma warning (disable: 4786)
//#include <GL/glut.h>
#include "Model.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "Statistics.h"

using namespace std;

// This associative array maps the GL_* glBegin type identifiers
// to the strings used in the SGF files
static const VertexTypeNameMapping TYPE_NAME_MAP[] =
{
	{ GL_POINTS,			"POINTS" },
{ GL_LINES,			"LINES" },
{ GL_LINE_STRIP,		"LINE_STRIP" },
{ GL_LINE_LOOP,		"LINE_LOOP" },
{ GL_TRIANGLES,		"TRIANGLES" },
{ GL_QUADS,			"QUADS" },
{ GL_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
{ GL_TRIANGLE_FAN,	"TRIANGLE_FAN" },
{ GL_QUAD_STRIP,		"QUAD_STRIP" },
{ GL_POLYGON,		"POLYGON" },
{ NULL,				NULL }
};

// Construct an empty model
Model::Model(void)
{
	_pTexture = NULL;
}

Model::~Model(void)
{
	clear();
}

void Model::clear(void)
{
	// Free all model parts
	for (size_t i = 0; i<_modelPartPtrs.size(); i++)
	{
		delete(_modelPartPtrs[i]);
	}
	_modelPartPtrs.clear();
	delete _pTexture;
	_pTexture = NULL;
}

void Model::draw(bool isPlayer2)
{
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	//glScalef(0.5f, 0.5f, 0.5f);
	// Setup shininess
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Draw all model parts
	for (size_t i = 0; i<_modelPartPtrs.size(); i++)
	{
		// p is the current model part
		ModelPart *p = _modelPartPtrs[i];

		// Get color of model part and tell OpenGL
		GLfloat color[4];
		p->getColor(color);
		color[0] /= 1.5; color[1] /= 1.5; color[2] /= 1.5;
		if (isPlayer2) {
			color[0] = 1.0f;
			color[1] = 0.0f;
			color[2] = 0.0f;
		}
		glColor4fv(color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		p->getColor(color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

		glPushMatrix();
		// Give OpenGL the data arrays and draw it
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		if (_pTexture && p->hasTexture())
		{
			// Eanble textures
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, p->getTextureCoordPointer());
			glEnable(GL_TEXTURE_2D);
			_pTexture->activate();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
		glVertexPointer(3, GL_FLOAT, 0, p->getVertexPointer());
		glNormalPointer(GL_FLOAT, 0, p->getNormalPointer());
		glDrawArrays(p->getPrimitiveType(), 0, p->getVertexCount());
		glPopMatrix();
		// Update statistics
		Statistics::instance().modelVerticesRendered += p->getVertexCount();
	}
	glPopMatrix();
	glPopAttrib();
}

void Model::loadSGF(const char *pFilename)
{
	// Load the file
	ifstream file;
	file.open(pFilename);
	if (!file)
		throw Error(string("Failed to load SGF file: ") + pFilename);

	// Read the whole file
	while (!file.eof())
	{
		int		vertexCount;
		string	typeString;

		// Clear error bits
		file.clear();

		// Read the type string (POINTS, TRIANGLES, etc.) and uppercase it
		file >> typeString;
		transform(typeString.begin(), typeString.end(), typeString.begin(), toupper);

		// Lookup the type in the map to get a GL_ identifier for it
		bool found = false;
		GLenum type = 0;
		const VertexTypeNameMapping *pMap = TYPE_NAME_MAP;
		while (!found)
		{
			if (pMap->pName == NULL)
				break;

			if (typeString == pMap->pName)
			{
				found = true;
				type = pMap->glType;
			}
			pMap++;
		}
		// Not found? throw error
		if (!found)
			throw Error(string("Unknown type identifier: '") + typeString + string("' in file: ") + pFilename);

		// Get the vertex count
		file >> vertexCount;
		if (file.eof())
			throw Error(string("Unexpected end of file in: ") + pFilename);

		// Read the color
		float a, r, g, b;
		file >> a >> r >> g >> b;
		if (file.eof())
			throw Error(string("Unexpected end of file in: ") + pFilename);

		// Create a new model part
		ModelPart *pPart = new ModelPart(type, vertexCount);
		_modelPartPtrs.push_back(pPart);
		pPart->setColor(r, g, b, a);

		// Read all the vertices
		int count = 0;
		while (file.good())
		{
			float x, y, z, nx, ny, nz;
			// Get coordinates and normal
			file >> x >> y >> z >> nx >> ny >> nz;
			if (!file.good()) break;

			// Add the coordinates and normal to the part
			pPart->addVertex(x, y, z, nx, ny, nz);
			count++;
		}

		// Did it read the correct number of vertices?
		if (count != vertexCount)
			throw Error(string("Vertex count incorrect in file: ") + pFilename);
	}
}


void Model::setTexture(CGTexture *pTexture, const BoundingBox &box)
{
	_pTexture = pTexture;
	for (unsigned int i = 0; i<_modelPartPtrs.size(); i++)
	{
		_modelPartPtrs[i]->genTexCoords(box);
	}
}