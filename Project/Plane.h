#ifndef _CG_PLANE_H_
#define _CG_PLANE_H_

/*	Some of the code in this class is based on the pseudo code in the article
'Generic Collision Detection for Games Using Ellipsoids' by Paul Nettle
(June 13, 2000, revision October 5, 2000). Code that is based on or uses
parts of this article is marked with '[Nettle,2000]'. See the following
link to obtain this article:
http://www.fluidstudios.com/?publications
*/

#include "Vector.h"
#include "Vertex.h"
#include "Ray.h"
#include "DataFile.h"

// The Plane class represents a plane in 3d space 
class Plane
{
public:

	/* Default constructor, builds a plane through the origin, with the normal
	pointing into the positive Y direction */
	Plane()
	{
		_origin = Vertex3(0, 0, 0);
		_normal = Vector3(0, 1, 0);
	}

	/* Creates a new plane from an origin point and a
	normal vector. The normal should be normalized. */
	Plane(Vertex3 origin, Vector3 normal)
	{
		_origin = origin;
		_normal = normal;
	}

	/* Copy constructor */
	Plane(const Plane &rhs)
	{
		_origin = rhs._origin;
		_normal = rhs._normal;
	}

	/* Assingment operator */
	Plane & operator=(const Plane &rhs)
	{
		_origin = rhs._origin;
		_normal = rhs._normal;
		return *this;
	}

	/* The following constructor builds a plane from 3 vertices by
	defining the plane that contains v1, v2 and v3. The following
	calculation is done:

	plane origin = v1
	plane normal = (v2-v1) x (v3-v1)  (normalized)		*/
	Plane(Vertex3 v1, Vertex3 v2, Vertex3 v3)
	{
		_origin = v1;
		_normal = (v2 - v1).cross(v3 - v1);
		_normal.normalize();
	}


	/* Determines on which side of the plane a given point
	in space is. The return value is a floating point number
	with the following possible meanings:

	> 0		The point lies on the side of the
	plane the normal points into.
	= 0		The point lies on the plane itself.
	< 0		The point lies on the side of the plane
	opposite to where the normal points into.	*/
	float side(const Vertex3 &point) const
	{
		/* The dot product of the plane's normal and origin (dPlane) and the dot
		product of the plane's normal and the point itself are both calculated.
		Their difference tells the point's position relative to the plane. */
		float dPoint = _normal.dot(point.vector());
		float dPlane = _normal.dot(_origin.vector());
		return dPoint - dPlane;
	}

	/* Intersects a ray with the plane.
	Note: The ray's direction vector should be normalized!
	Returns true if the ray intersects the plane, or false if
	the ray does not. If the ray intersects the plane, the float
	variable pDistance points to is set to the distance between
	the ray's origin and the plane. This distance may be negative if
	the ray's origin was on the side opposite to where the normal
	points to.
	*/
	bool intersect(const Ray &ray, float *pDistance) const
	{
		/* Based on the pseudo code in [Nettle,2000] (see top of file) */
		double d = -(_normal.dot(_origin.vector()));
		double numer = _normal.dot(ray.origin().vector()) + d;
		double denom = _normal.dot(ray.vector());
		if (denom == 0)
		{
			// Ray is parallel to the plane
			return false;
		}
		*pDistance = static_cast<float>(-(numer / denom));
		return true;
	}

	// Flips the plane by inverting its normal
	void flip()
	{
		_normal.flip();
	}

	// Returns the smallest distance between the given point and plane
	double distance(const Vertex3 &point) const
	{
		float val = 0.0;
		intersect(Ray(point, -_normal), &val);
		return val;
	}


	/* Returns the normal of the plane */
	Vector3 normal() const
	{
		return _normal;
	}

	/* Returns the origin point of the plane */
	Vertex3 origin() const
	{
		return _origin;
	}

	// (Un)serializes the Plane's data
	void exchange(DataFile *pFile)
	{
		_normal.exchange(pFile);
		_origin.exchange(pFile);
	}

private:
	Vector3 _normal;	// The plane's normal
	Vertex3 _origin;	// The plane's origin
};

#endif

