/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/
#ifndef _CG_CGTEXTURE_H_
#define _CG_CGTEXTURE_H_


#include "DataFile.h"
#include "3dclasses.h"
#include "DisallowCopy.h"
#include <string>
#include "Globals.h"
//#include "glut.h"

const int MAX_FILE_LEN = 260;

// Represents a texture used in cgworld
class CGTexture : private DisallowCopy
{
public:
	CGTexture();

	// Create a new texture with given filename
	CGTexture(const char *pFilename);
	virtual ~CGTexture();
	void exchange(DataFile *pFile);

	// Load the texture from file
	bool loadTextureFile();

	std::string getFilename() const;

	// Bind the texture for openGL
	void activate() const;
private:
	std::string _filename; // Filename of the texture
	GLuint		_glTexName;
};

#endif
