#ifndef _CG_VERTEX_H_
#define _CG_VERTEX_H_

// This warning is disabled because of a bug in VC6.
// It supresses warnings about too long debug symbols 
#pragma warning (disable: 4786)

#include <cmath>
#include "Vector.h"
#include "DataFile.h"

/* The 3 dimensional vertex class. This class represents a point
in the 3 dimensional space. */
class Vertex3
{
public:
	/* Default constructor, leaves the vertex elements
	uninitialized (possibly invalid, but this is done
	for performance reasons). */
	Vertex3()
	{
	}

	/* Copy constructor, builds a copy of the rhs vertex */
	Vertex3(const Vertex3 &rhs)
	{
		_ver[0] = rhs._ver[0];
		_ver[1] = rhs._ver[1];
		_ver[2] = rhs._ver[2];
	}

	/* Assignment operator, copies the rhs vertex) */
	Vertex3 & operator=(const Vertex3 &rhs)
	{
		_ver[0] = rhs._ver[0];
		_ver[1] = rhs._ver[1];
		_ver[2] = rhs._ver[2];
		return *this;
	}

	/* Builds a vertex from a 3 dimensional coordinate (x,y,z) */
	Vertex3(float x, float y, float z)
	{
		_ver[0] = x;
		_ver[1] = y;
		_ver[2] = z;
	}

	/* Builds a vertex with all elements set to the 'all' parameter,
	i.e. the vertex (all,all,all). */
	Vertex3(float all)
	{
		_ver[0] = all;
		_ver[1] = all;
		_ver[2] = all;
	}

	/* Substracts the given vertex (rhs) from the current vertex,
	resulting in a vector from 'rhs' to 'this'. */
	Vector3 operator-(const Vertex3 &rhs) const
	{
		return Vector3(_ver[0] - rhs._ver[0],
			_ver[1] - rhs._ver[1],
			_ver[2] - rhs._ver[2]);
	}

	/* Adds a vector to the current vertex, resulting in a new vertex
	which is returned. */
	Vertex3 operator+(const Vector3 & rhs) const
	{
		return Vertex3(_ver[0] + rhs.x(),
			_ver[1] + rhs.y(),
			_ver[2] + rhs.z());
	}

	/* Adds a vector to the current vertex and replaces it by the
	resulting vertex. */
	Vertex3 & operator+=(const Vector3 & rhs)
	{
		_ver[0] += rhs.x();
		_ver[1] += rhs.y();
		_ver[2] += rhs.z();
		return *this;
	}

	/* Substracts a vector to the current vertex, resulting in a new vertex
	which is returned. */
	Vertex3 operator-(const Vector3 & rhs) const
	{
		return Vertex3(_ver[0] - rhs.x(),
			_ver[1] - rhs.y(),
			_ver[2] - rhs.z());
	}

	/* Substracts a vector to the current vertex and replaces it by the
	resulting vertex. */
	Vertex3 & operator-=(const Vector3 & rhs)
	{
		_ver[0] -= rhs.x();
		_ver[1] -= rhs.y();
		_ver[2] -= rhs.z();
		return *this;
	}

	/* Equality operator, returns true if the two vertices are
	equal. */
	bool operator==(const Vertex3 &rhs)
	{
		return  _ver[0] == rhs._ver[0] &&
			_ver[1] == rhs._ver[1] &&
			_ver[2] == rhs._ver[2];
	}

	/* Inequality operator, returns true if the two vertices are
	different. */
	bool operator!=(const Vertex3 &rhs)
	{
		return !(*this == rhs);
	}

	/* Returns the distance between the vertex and the origin (0,0,0). */
	float distance()
	{
		return sqrt(_ver[0] * _ver[0] +
			_ver[1] * _ver[1] +
			_ver[2] * _ver[2]);
	}

	/* Returns the distance between two points */
	float distance(const Vertex3 &rhs)
	{
		float dx = _ver[0] - rhs._ver[0];
		float dy = _ver[1] - rhs._ver[1];
		float dz = _ver[2] - rhs._ver[2];

		return sqrt(dx*dx + dy * dy + dz * dz);
	}

	/* Scales the vertex's elements by the x, y and z
	factors given as parameters. */
	void scaleWorld(float x, float y, float z)
	{
		_ver[0] *= x;
		_ver[1] *= y;
		_ver[2] *= z;
	}

	/* Converts the vertex into a vector by substracting the origin
	point (0,0,0) from it. This results in a vector from the origin to
	the vertex. */
	Vector3 vector() const
	{
		return Vector3(_ver[0], _ver[1], _ver[2]);
	}

	/* Returns a pointer to the vertex data in the form of a 3
	element floating point array with the x, y and z values.
	This reference stays valid as long as the object exists. */
	const float * ref() const
	{
		return _ver;
	}

	/* Cast operator to the vertex data in the form of a
	3 element floating point array with the x,y, and z values.
	The reference stays valid as long as the object exists. */
	operator const float *() const
	{
		return _ver;
	}

	// (Un)serializes the vertex's data.
	void exchange(DataFile *pFile)
	{
		pFile->exchange(_ver, 3);
	}


	// Wrappers to return seperate x, y or z values of the vertex
	float x() const { return _ver[0]; }
	float y() const { return _ver[1]; }
	float z() const { return _ver[2]; }

private:

	float _ver[3];		// The x, y and z values of the vector
};

#endif

