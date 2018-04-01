/**
 * VU Computer Graphics Practicum
 * Timing, random number and texture utilities
 **/

#ifndef VUCG_UTILS_H
#define VUCG_UTILS_H

#include <stdlib.h>
#if defined(WIN32)
#include <winsock.h>
#include <sys\timeb.h>
int gettimeofday(struct timeval *tp, void *tzp);
#define usleep(mseconds) Sleep(mseconds/1000)
#define M_PI 3.14159
#else
#include <unistd.h>
#include <sys/time.h>
#endif


/** 
 * Returns the time in seconds as a double value.
 **/
double Wallclock(void);

/**
 * Waits for 'val' number of seconds (a double value)
 **/
void Wait(double val);

/**
 * Returns a random float value within the interval [min, max]
 **/
float frand(float min, float max);


/**
 * The RGB-image struct returned by LoadRGB(char *imageFile):
 *
 * sizeX and sizeY: the resolution of the image
 * components:		the number of bytes used per pixel
 *					(normally: 4 for RGBA, 3 for RGB or 1 for 
 *					 grayscale images)
 * format:			either GL_RGB, GL_RGBA or GL_LUMINANCE (= grayscale)
 * data:			pointer to a char array of size (sizeX * sizeY * components) 
 *					it contains the actual image data
 **/
typedef struct _TK_RGBImageRec 
{
  int				sizeX, 
					sizeY,
					components;
  unsigned int		format;
  unsigned char	   *data;
} RGBImage;


/** 
 * Load an .rgb file named 'imageFile'. If no path precedes the filename
 * it will be taken from the directory where the executable is running. 
 * Do not use full paths (i.e. C:/graphics/textures/brick.rgb). That way, 
 * your application will most likely only work on your own PC. Use relative 
 * paths (e.g. ../textures/brick.rgb) instead.
 *
 * This function returns a pointer to a RGBImage struct if the file was loaded.
 * Otherwise, if something went wrong, it returns NULL.
 * 
 * (Note: this is SGI-RGB not windows style RGB, check the website 
 * www.cs.vu.nl/~graphics/ to find out how to convert more common formats 
 * like.jpg to .rgb)
 **/
extern RGBImage* LoadRGB(const char *imageFile);

#endif


