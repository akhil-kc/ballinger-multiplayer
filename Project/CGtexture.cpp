#include "cgtexture.h"
#include "glutil.c"
//#include <GL/glut.h>
#include "Globals.h"
using namespace std;

CGTexture::CGTexture()
{
	_glTexName = -1;
}

CGTexture::CGTexture(const char *pFilename)
{
	_glTexName = -1;
	_filename = pFilename;
}

CGTexture::~CGTexture()
{
}

void CGTexture::exchange(DataFile *pFile)
{
	pFile->exchange(&_filename, MAX_FILE_LEN);
}

std::string CGTexture::getFilename() const
{
	return _filename;
}

void CGTexture::activate() const
{
	if (_glTexName == -1) return;
	glBindTexture(GL_TEXTURE_2D, _glTexName);
}

bool CGTexture::loadTextureFile()
{
	// Already loaded?
	if (_glTexName != -1) return true;

	// Load RGB file
	RGBImage *pImage = LoadRGB(_filename.c_str());
	if (!pImage) return false;

	// Create a new texture name
	glGenTextures(1, &_glTexName);

	// Bind the texture and set some parameters
	glBindTexture(GL_TEXTURE_2D, _glTexName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	printf("image size: %d, %d\n", pImage->sizeX, pImage->sizeY);
	// Build mipmaps automatically
	int ret = gluBuild2DMipmaps(GL_TEXTURE_2D, pImage->components,
		pImage->sizeX, pImage->sizeY,
		pImage->format,
		GL_UNSIGNED_BYTE, pImage->data);

	free(pImage);

	// Building failed
	if (ret != 0) return false;

	// Success!
	return true;
}
