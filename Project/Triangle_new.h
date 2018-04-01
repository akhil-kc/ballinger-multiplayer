/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#ifndef _CG_TRIANGLE_H_
#define _CG_TRIANGLE_H_

#include <cstdlib>
#include <cmath>
#include "Vertex.h"
#include "DataFile.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "util.h"

/* A class to represent a triangle */
class Triangle_new
{
public:
	// Builds a new Triangle with possibly invalid values (for performance)
	Triangle_new()
	{
	}

	/* Builds a triangle from 3 vertices. */
	Triangle_new(Vertex3 v1, Vertex3 v2, Vertex3 v3)
	{
		_v1 = v1;
		_v2 = v2;
		_v3 = v3;
	}

	// Wrappers to access each of the three vertices
	Vertex3 v1() const { return _v1; };
	Vertex3 v2() const { return _v2; };
	Vertex3 v3() const { return _v3; };

	// Returns the plane the triangle lies on
	Plane plane() const
	{
		return Plane(_v1, _v2, _v3);
	}

	// Returns the bounding box of the triangle
	BoundingBox boundingBox() const
	{
		float minX, minY, minZ;
		float maxX, maxY, maxZ;

		minX = minVal(minVal(_v1.x(), _v2.x()), _v3.x());
		minY = minVal(minVal(_v1.y(), _v2.y()), _v3.y());
		minZ = minVal(minVal(_v1.z(), _v2.z()), _v3.z());

		maxX = maxVal(maxVal(_v1.x(), _v2.x()), _v3.x());
		maxY = maxVal(maxVal(_v1.y(), _v2.y()), _v3.y());
		maxZ = maxVal(maxVal(_v1.z(), _v2.z()), _v3.z());

		return BoundingBox(Vertex3(minX, minY, minZ),
			Vertex3(maxX, maxY, maxZ));
	}

	/* Returns true if the point is inside the triangle. The given point
	should be in the triangle's plane.
	*/
	bool inside(const Vertex3 &point) const
	{
		/*
		Algorithm based on:
		http://groups-beta.google.com/group/comp.graphics.algorithms/browse_frm/thread/7d8728e88c3ec830

		>   C1 = (P - A) x (B - A)
		>   C2 = (P - B) x (c - B)
		>   C3 = (P - C) x (A - C)
		*/

		Vector3 c1 = (point - _v1).cross(_v2 - _v1);
		Vector3 c2 = (point - _v2).cross(_v3 - _v2);
		Vector3 c3 = (point - _v3).cross(_v1 - _v3);


		/*
		"All you need to do now is the following dot products:

		C1.N
		C2.N
		C3.N

		where N is the triangle normal, (B-A) x (C-A).
		If all dot products are positive then the point is inside.
		Or it may be that they all need to be negative, depends which way
		the triangle normal points."
		*/
		const Vector3 n = (_v2 - _v1).cross(_v3 - _v1);
		float dot1 = c1.dot(n),
			dot2 = c2.dot(n),
			dot3 = c3.dot(n);

		if (dot1 >0 && dot2 > 0 && dot3 > 0)
			return true;
		else if (dot1 < 0 && dot2 < 0 && dot3 < 0)
			return true;
		else
			return false;
	}

	/* Scales the triangles' vertices by the x, y and z
	factor's given. */
	void scaleWorld(float x, float y, float z)
	{
		_v1.scaleWorld(x, y, z);
		_v2.scaleWorld(x, y, z);
		_v3.scaleWorld(x, y, z);
	}

private:
	Vertex3 _v1, _v2, _v3; // The three vertices of the triangle

};


#endif
