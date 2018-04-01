#ifndef _CG_RAY_H_
#define _CG_RAY_H_

#include "Vertex.h"
#include "Vector.h"

/* The Ray class represents a ray in 3D space, that is an origin with a direction
vector. The direction vector can have any length. */
class Ray
{
public:

	// Builds a ray from an origin point and direction vector
	Ray(const Vertex3 origin, const Vector3 vector)
	{
		_origin = origin;
		_vector = vector;
	}

	// Returns the origin of the ray
	Vertex3 origin() const
	{
		return _origin;
	}

	// Returns the direction vector of the ray
	Vector3 vector() const
	{
		return _vector;
	}

	// Normalizes the direction vector of the ray
	void normalize()
	{
		_vector.normalize();
	}

	// Returns the length of the ray
	float length() const
	{
		return _vector.length();
	}

	// Returns the end point of the ray (origin + vector)
	Vertex3 end() const
	{
		return _origin + _vector;
	}

private:
	Vertex3 _origin;		// The ray's origin
	Vector3 _vector;		// The ray's direction vector
};

#endif _CG_RAY_H_
