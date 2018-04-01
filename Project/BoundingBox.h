#ifndef _CG_BOUNDINGBOX_H_
#define _CG_BOUNDINGBOX_H_

// This warning is disabled because of a bug in VC6.
// It supresses warnings about too long debug symbols 
#pragma warning (disable: 4786)

#include "DataFile.h"
#include "Vertex.h"
#include "Vector.h"
#include "Plane.h"


/* Class to represent a bounding box  */

class BoundingBox
{
public:
	// Builds a new bounding box with invalid data (for performance)
	BoundingBox()
	{
	}

	// Builds a new bounding box given the minimum and maximum 
	// points the bounding box should cover.
	BoundingBox(Vertex3 min, Vertex3 max)
	{
		_minX = min.x(); _minY = min.y(); _minZ = min.z();
		_maxX = max.x(); _maxY = max.y(); _maxZ = max.z();
	}

	/* Returns true if the given bounding box intersects with
	this bounding box, false otherwise. */
	bool intersects(const BoundingBox &rhs) const
	{
		/* If in any dimension, the min-max range of both bounding
		boxes do not overlap, the bounding boxes can never intersect
		Otherwise they do intersect. */
		if (_maxX < rhs._minX || _minX > rhs._maxX)
			return false;
		if (_maxY < rhs._minY || _minY > rhs._maxY)
			return false;
		if (_maxZ < rhs._minZ || _minZ > rhs._maxZ)
			return false;
		return true;
	}

	// Expands the bounding box so that it includes the given point
	void include(const Vertex3 &point)
	{
		float	x = point.x(),
			y = point.y(),
			z = point.z();

		if (x < _minX) _minX = x;
		if (y < _minY) _minY = y;
		if (z < _minZ) _minZ = z;

		if (x > _maxX) _maxX = x;
		if (y > _maxY) _maxY = y;
		if (z > _maxZ) _maxZ = z;
	}

	// (Un)serializes the bounding box's data
	void exchange(DataFile *pFile)
	{
		pFile->exchange(&_minX);
		pFile->exchange(&_minY);
		pFile->exchange(&_minZ);
		pFile->exchange(&_maxX);
		pFile->exchange(&_maxY);
		pFile->exchange(&_maxZ);
	}

	/* Expands the bounding box so that the given bounding box
	is included as well. */
	void include(const BoundingBox &box)
	{
		include(box.minPoint());
		include(box.maxPoint());
	}

	// Returns the minimum point the bounding box includes
	Vertex3 minPoint() const
	{
		return Vertex3(_minX, _minY, _minZ);
	}

	// Returns the maximum point the bounding box includes
	Vertex3 maxPoint() const
	{
		return Vertex3(_maxX, _maxY, _maxZ);
	}

	/* Tests wether this bounding box is (partly) on the side of the plane
	where the plane's normal points into. Returns true if it does,
	and false if (probably) doesn't.

	Note that this function is conservative in that it may return true
	even though the bounding box is not on the normal's side of the plane.
	However, it will never return false if the bounding box is on the
	normal's side of the plane.

	For most uses this is sufficient and it allows for a better performance
	of the function.    */
	bool insidePlane(const Plane &plane) const
	{
		// Test all 8 points of the bounding box. If one of them is on the
		// right side of the plane, return true. Otherwise return false.
		if (plane.side(Vertex3(_minX, _minY, _minZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_minX, _minY, _maxZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_minX, _maxY, _minZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_minX, _maxY, _maxZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_maxX, _minY, _minZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_maxX, _minY, _maxZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_maxX, _maxY, _minZ)) >= 0.0)	return true;
		if (plane.side(Vertex3(_maxX, _maxY, _maxZ)) >= 0.0)	return true;
		return false;
	}

	// Returns the center point of the bounding box
	Vertex3 center() const
	{
		return Vertex3((_maxX + _minX) / 2.0f, (_maxY + _minY) / 2.0f, (_maxZ + _minZ) / 2);
	}

	// dx, dy, dz return the widths of the bounding box in each dimension.
	float dx() const { return _maxX - _minX; }
	float dy() const { return _maxY - _minY; }
	float dz() const { return _maxZ - _minZ; }


private:
	float _minX, _minY, _minZ;	// The minimum point the bounding box covers
	float _maxX, _maxY, _maxZ;	// The maximum point the bounding box covers

};

#endif

