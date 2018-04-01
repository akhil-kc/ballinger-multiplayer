/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#ifndef _CG_UTIL_H_
#define _CG_UTIL_H_

// This warning is disabled because of a bug in VC6.
// It supresses warnings about too long debug symbols 
#pragma warning (disable: 4786)

template<typename T>
inline T minVal(T a, T b)
{
	return (a < b) ? a : b;
}

template<typename T>
inline T maxVal(T a, T b)
{
	return (a > b) ? a : b;
}

// Convertes degrees to radians
inline float deg2rad(float degrees)
{
	const float PI1 = 3.14159265358979324f;
	const float MAX_DEGREES = 360.0f;
	return (degrees * 2.0f * PI1) / MAX_DEGREES;
}

// Converts radians to degrees
inline float rad2deg(float radians)
{
	const float PI1 = 3.14159265358979324f;
	const float MAX_DEGREES = 360.0f;
	return radians * (MAX_DEGREES / (2.0f * PI1));
}


// This function multiplies an openGL 4x4 model-view matrix with
// a point (4-dimensional) so that the point is transformed into
// a new point (result).
inline void mulMatrixPoint(float *result, float *matrix, float *point)
{
	// Matrix multiplication: 4x4 x 4x1 -> 4x1 
	for (int y = 0; y<4; y++)
	{
		float s = 0.0;
		for (int i = 0; i<4; i++)
		{
			float v = matrix[i * 4 + y]; // row
			float w = point[i];
			s += v * w;

		}
		result[y] = s;
	}
}
#endif
