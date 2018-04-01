#ifndef _CG_VECTOR_H_
#define _CG_VECTOR_H_

#include <cmath>
#include "DataFile.h"

/* The 3 dimensional vector class */
class Vector3
{
public:
	/* Default constructor, leaves the vector elements
	uninitialized (possibly invalid, but this is done
	for performance reasons). */
	Vector3()
	{
	}

	/* Copy constructor, builds a copy of the rhs Vector */
	Vector3(const Vector3 &rhs)
	{
		_vec[0] = rhs._vec[0];
		_vec[1] = rhs._vec[1];
		_vec[2] = rhs._vec[2];
	}

	/* Assignment operator, copies the given vector */
	Vector3 & operator=(const Vector3 &rhs)
	{
		_vec[0] = rhs._vec[0];
		_vec[1] = rhs._vec[1];
		_vec[2] = rhs._vec[2];
		return *this;
	}

	/* Constructs a vector from x, y and z values) */
	Vector3(float x, float y, float z)
	{
		_vec[0] = x;
		_vec[1] = y;
		_vec[2] = z;
	}
	/* Constructs a vector with all elements set to the value of
	the 'all' parameter */
	Vector3(float all)
	{
		_vec[0] = all;
		_vec[1] = all;
		_vec[2] = all;
	}

	/* Adds two vectors, building a new vector */
	Vector3 operator+(const Vector3 &rhs) const
	{
		Vector3 newVec;
		newVec._vec[0] = _vec[0] + rhs._vec[0];
		newVec._vec[1] = _vec[1] + rhs._vec[1];
		newVec._vec[2] = _vec[2] + rhs._vec[2];
		return newVec;
	}

	/* Adds a vector to the current vector */
	Vector3 & operator+=(const Vector3 &rhs)
	{
		_vec[0] += rhs._vec[0];
		_vec[1] += rhs._vec[1];
		_vec[2] += rhs._vec[2];
		return *this;
	}

	/* Substracts two vectors, building a new vector */
	Vector3 operator-(const Vector3 &rhs) const
	{
		Vector3 newVec;
		newVec._vec[0] = _vec[0] - rhs._vec[0];
		newVec._vec[1] = _vec[1] - rhs._vec[1];
		newVec._vec[2] = _vec[2] - rhs._vec[2];
		return newVec;
	}

	/* Substracts a vector from the current vector */
	Vector3 & operator-=(const Vector3 &rhs)
	{
		_vec[0] -= rhs._vec[0];
		_vec[1] -= rhs._vec[1];
		_vec[2] -= rhs._vec[2];
		return *this;
	}

	/* Negation operator, returns the negative vector (-x,-y,-z) */
	Vector3 operator-() const
	{
		return Vector3(-_vec[0], -_vec[1], -_vec[2]);
	}

	/* Calculates the cross product of the current and given
	vectors, returning a new vector. */
	Vector3 cross(const Vector3 & rhs) const
	{
		Vector3 crossVec;
		crossVec._vec[0] = _vec[1] * rhs._vec[2] - _vec[2] * rhs._vec[1];
		crossVec._vec[1] = _vec[2] * rhs._vec[0] - _vec[0] * rhs._vec[2];
		crossVec._vec[2] = _vec[0] * rhs._vec[1] - _vec[1] * rhs._vec[0];
		return crossVec;
	}

	/* Returns the dot product of the current vector and the given
	vector. */
	float dot(const Vector3 & rhs) const
	{
		return	  _vec[0] * rhs._vec[0] +
			_vec[1] * rhs._vec[1] +
			_vec[2] * rhs._vec[2];
	}

	/* Equality operator, returns true if the two vectors are
	equal. */
	bool operator==(const Vector3 &rhs)
	{
		return  _vec[0] == rhs._vec[0] &&
			_vec[1] == rhs._vec[1] &&
			_vec[2] == rhs._vec[2];
	}

	/* Inequality operator, returns true if the two vectors are
	different. */
	bool operator!=(const Vector3 &rhs)
	{
		return !(*this == rhs);
	}

	/* Vector-scalar division, modifies the current vector. */
	Vector3 & operator/=(float scalar)
	{
		_vec[0] /= scalar;
		_vec[1] /= scalar;
		_vec[2] /= scalar;
		return *this;
	}

	/* Vector-scalar division, returns a new vector. */
	Vector3 operator /(float scalar) const
	{
		Vector3 ret;
		ret._vec[0] = _vec[0] / scalar;
		ret._vec[1] = _vec[1] / scalar;
		ret._vec[2] = _vec[2] / scalar;
		return ret;
	}

	/* Vector-scalar multiplication, modifies the current vector. */
	Vector3 & operator*=(float scalar)
	{
		_vec[0] *= scalar;
		_vec[1] *= scalar;
		_vec[2] *= scalar;
		return *this;
	}

	/* Vector-scalar multiplication, returns a new vector. */
	Vector3 operator *(float scalar) const
	{
		Vector3 ret;
		ret._vec[0] = _vec[0] * scalar;
		ret._vec[1] = _vec[1] * scalar;
		ret._vec[2] = _vec[2] * scalar;
		return ret;
	}

	/* Returns the length of the vector. */
	float length() const
	{
		// length = sqrt(x^2 + y^2 + z^2)
		return sqrt(_vec[0] * _vec[0] +
			_vec[1] * _vec[1] +
			_vec[2] * _vec[2]);
	}

	/* Returns the squared length of the vector. */
	float squaredLength() const
	{
		// squared length = x^2 + y^2 + z^2
		return  _vec[0] * _vec[0] +
			_vec[1] * _vec[1] +
			_vec[2] * _vec[2];
	}


	/* Sets the length of the vector, leaving the direction unchanged. */
	void setLength(float newLength)
	{
		float curLength = length();
		float scaleFactor = (curLength == 0.0) ? 0.0f : (newLength / curLength);
		*this *= scaleFactor;
	}

	/* Returns the original vector resized, leaving the direction unchanged. */
	Vector3 resized(float newLength) const
	{
		float curLength = length();
		float scaleFactor = (curLength == 0.0) ? 0.0f : (newLength / curLength);
		return *this * scaleFactor;
	}

	// Flips the direction of the vector
	void flip()
	{
		_vec[0] = -_vec[0];
		_vec[1] = -_vec[1];
		_vec[2] = -_vec[2];
	}

	/* Normalizes the vector. */
	void normalize()
	{
		// Normalize by scalar-dividing the vector by its length
		float len = length();
		if (len == 0) len = 1; // prevent division by zero
		*this /= len;
	}

	/* Returns the vector normalized. */
	Vector3 normalized() const
	{
		// Normalize by scalar-dividing the vector by its length
		float len = length();
		if (len == 0) len = 1; // prevent division by zero
		return *this / len;
	}

	/* Returns a pointer to the vector data in the form of a 3
	element floating point array with the x, y and z values.
	This reference stays valid as long as the object exists. */
	const float * ref() const
	{
		return _vec;
	}

	/* Cast operator to the vector data in the form of a
	3 element floating point array with the x,y, and z values.
	The reference stays valid as long as the object exists. */
	operator const float *() const
	{
		return _vec;
	}

	/* Scales the vector's elements by the x, y and z
	factors given as parameters. */
	void scaleWorld(float x, float y, float z)
	{
		_vec[0] *= x;
		_vec[1] *= y;
		_vec[2] *= z;
	}


	// (Un)serializes the vector's data.
	void exchange(DataFile *pFile)
	{
		pFile->exchange(_vec, 3);
	}

	// Wrappers to return seperate x, y or z values of the vector
	float x() const { return _vec[0]; }
	float y() const { return _vec[1]; }
	float z() const { return _vec[2]; }

private:
	float _vec[3];	// The x, y and z values of the vector
};



#endif

