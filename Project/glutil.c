//#include "Globals.h"
#include "glutil.h"
#include "glut.h"
#include <stdio.h>
#include <string.h>

double Wallclock(void)
{
    struct timeval tv;
	
    gettimeofday(&tv, 0);
	
    return (((double) tv.tv_sec) + ((double) tv.tv_usec) / 1000000.0);
}

void Wait(double val)
{
    unsigned long usec = (unsigned long) (val * 1000000.0);
    usleep(usec);
}

#if defined(WIN32)
int gettimeofday(struct timeval *tp, void *tzp)
{
	struct _timeb t;
	
    _ftime(&t);
    tp->tv_sec = t.time;
    tp->tv_usec = t.millitm * 1000;
    return 0;
}
#endif

float frand(float min, float max)
{
	float rr;
	
#if defined(WIN32)
	rr = (float) rand() / (float) RAND_MAX;
#else
	rr = drand48();
#endif
	return min + rr * (max - min);
}


#ifndef SEEK_SET
#define SEEK_SET 0
#endif

/******************************************************************************/

typedef struct _rawImageRec 
{
	unsigned short imagic,
				   type,
				   dim,
				   sizeX, 
				   sizeY,
				   sizeZ;
	unsigned long  min, 
				   max,	
				   wasteBytes,
				   colorMap,
				   rleEnd;
	FILE		  *file;
	char		   name[80];
	unsigned char *tmp, 
				  *tmpR, 
				  *tmpG, 
				  *tmpB, 
				  *tmpA;
	GLuint		  *rowStart;
	GLint		  *rowSize;
} rawImageRec;

union 
{
	int testWord;
	char testByte[4];
} endianTest;

/******************************************************************************/

static void ConvertShort(unsigned short *array, size_t length)
{
	unsigned short b1, b2;
	unsigned char *ptr;
	
	ptr = (unsigned char *) array;
	while (length--) 
	{
		b1 = *ptr++;
		b2 = *ptr++;
		*array++ = (b1 << 8) | (b2);
	}
}

static void ConvertLong(GLuint * array, size_t length)
{
	unsigned long b1, b2, b3, b4;
	unsigned char *ptr;
	
	ptr = (unsigned char *) array;
	while (length--) 
	{
		b1 = *ptr++;
		b2 = *ptr++;
		b3 = *ptr++;
		b4 = *ptr++;
		*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
	}
}

static rawImageRec *RawImageOpen(const char *fileName)
{
	rawImageRec *raw;
	GLenum		 swapFlag;
	int			 x;
	
	endianTest.testWord = 1;
	if (endianTest.testByte[0] == 1) 
		swapFlag = GL_TRUE;
	else
		swapFlag = GL_FALSE;
	
	raw = (rawImageRec *) malloc(sizeof(rawImageRec));
	if (raw == NULL) 
	{
		fprintf(stderr, "LOAD-RGB> Out of memory!\n");
		return NULL;
	}
	if ((raw->file = fopen(fileName, "rb")) == NULL) 
	{
		perror(fileName);
		return NULL;
	}
	fread(raw, 1, 12, raw->file);
	
	if (swapFlag) 
		ConvertShort(&raw->imagic, 6);

	raw->tmp = (unsigned char *) malloc(raw->sizeX * 256);
	if (raw->tmp == NULL) 
	{
		fprintf(stderr, "LOAD-RGB> Out of memory!\n");
		return NULL;
	}
	raw->tmpR = (unsigned char *) malloc(raw->sizeX * 256);
	if (raw->tmpR == NULL) 
	{
		fprintf(stderr, "LOAD-RGB> Out of memory!\n");
		return NULL;
	}
	if (raw->sizeZ > 1) 
	{
		raw->tmpG = (unsigned char *) malloc(raw->sizeX * 256);
		raw->tmpB = (unsigned char *) malloc(raw->sizeX * 256);
		if (raw->tmpG == NULL || raw->tmpB == NULL) 
		{
			fprintf(stderr, "LOAD-RGB> Out of memory!\n");
			return NULL;
		}
		if (raw->sizeZ == 4) 
		{
			raw->tmpA = (unsigned char *) malloc(raw->sizeX * 256);
			if (raw->tmpA == NULL) 
			{
				fprintf(stderr, "LOAD-RGB> Out of memory!\n");
				return NULL;
			}
		}
	}
	if ((raw->type & 0xFF00) == 0x0100) 
	{
		x = raw->sizeY * raw->sizeZ * sizeof(GLuint);
		raw->rowStart = (GLuint *) malloc(x);
		raw->rowSize = (GLint *) malloc(x);
		if (raw->rowStart == NULL || raw->rowSize == NULL) 
		{
			fprintf(stderr, "LOAD-RGB> Out of memory!\n");
			return NULL;
		}
		raw->rleEnd = 512 + (2 * x);
		fseek(raw->file, 512, SEEK_SET);
		fread(raw->rowStart, 1, x, raw->file);
		fread(raw->rowSize, 1, x, raw->file);
		if (swapFlag) 
		{
			ConvertLong(raw->rowStart, x / sizeof(GLuint));
			ConvertLong((GLuint *) raw->rowSize, x / sizeof(GLint));
		}
	}
	return raw;
}

static void RawImageGetRow(rawImageRec * raw, unsigned char *buf, int y, int z)
{
	unsigned char *iPtr, *oPtr, pixel;
	int count;
	
	if ((raw->type & 0xFF00) == 0x0100) 
	{
		fseek(raw->file, (long) raw->rowStart[y + z * raw->sizeY], SEEK_SET);
		fread(raw->tmp, 1, (size_t) raw->rowSize[y + z * raw->sizeY],
			raw->file);
		
		iPtr = raw->tmp;
		oPtr = buf;
		for (;;) 
		{
			pixel = *iPtr++;
			count = (int) (pixel & 0x7F);
			if (!count) 
				return;

			if (pixel & 0x80) 
			{
				while (count--) 
					*oPtr++ = *iPtr++;
			} else {
				pixel = *iPtr++;
				while (count--) 
					*oPtr++ = pixel;
			}
		}
	} else {
		fseek(raw->file, 512 + (y * raw->sizeX) + (z * raw->sizeX * raw->sizeY),
			SEEK_SET);
		fread(buf, 1, raw->sizeX, raw->file);
	}
}

static void RawImageGetData(rawImageRec * raw, RGBImage * final)
{
	unsigned char *ptr;
	int i, j;
	
	final->data = (unsigned char *) malloc((raw->sizeX + 1) * (raw->sizeY + 1) * 4);
	if (final->data == NULL) 
		fprintf(stderr, "LOAD-RGB> Out of memory!\n");

	ptr = final->data;
	for (i = 0; i < (int) (raw->sizeY); i++) 
	{
		RawImageGetRow(raw, raw->tmpR, i, 0);
		if (raw->sizeZ > 1) 
		{
			RawImageGetRow(raw, raw->tmpG, i, 1);
			RawImageGetRow(raw, raw->tmpB, i, 2);
			if (raw->sizeZ > 3)
				RawImageGetRow(raw, raw->tmpA, i, 3);
		}
		for (j = 0; j < (int) (raw->sizeX); j++) 
		{
			*ptr++ = *(raw->tmpR + j);
			if (raw->sizeZ > 1) 
			{
				*ptr++ = *(raw->tmpG + j);
				*ptr++ = *(raw->tmpB + j);
				if (raw->sizeZ > 3) 
					*ptr++ = *(raw->tmpA + j);
			}
		}
	}
}

static RGBImage *tkRGBImageLoad(const char *fileName)
{
	rawImageRec *raw;
	RGBImage	*final;
	
	raw = RawImageOpen(fileName);
	final = (RGBImage *) malloc(sizeof(RGBImage));
	if (final == NULL) 
	{
		fprintf(stderr, "LOAD-RGB> Out of memory!\n");
		return NULL;
	}
	final->sizeX = raw->sizeX;
	final->sizeY = raw->sizeY;
	final->components = raw->sizeZ;
	RawImageGetData(raw, final);
	return final;
}

int ExistsFile(const char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp)
	{
		fclose(fp);
		return 1;
	}
	return 0;
}

RGBImage *LoadRGB(const char *imageFile)
{
	RGBImage *image;
	
	if (!ExistsFile(imageFile))
	{
		fprintf(stderr, "LOAD-RGB> file '%s' not found!\n", imageFile);
		return NULL;
	}

	image = tkRGBImageLoad(imageFile);
	
	if (image->components == 3)
	{
		image->format = GL_RGB;
	} else if (image->components == 4)
	{
		image->format = GL_RGBA;
	} else if (image->components == 1)
	{
		image->format = GL_LUMINANCE;
	}

	return image;
}
