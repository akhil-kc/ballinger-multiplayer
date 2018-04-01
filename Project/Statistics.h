#ifndef _CG_STATISTICS_H_
#define _CG_STATISTICS_H_

// This warning is disabled because of a bug in VC6.
// It supresses warnings about too long debug symbols 
#include <iostream>
#pragma warning (disable: 4786)

#include <sstream>
#include <string>
#include <iomanip>

#pragma warning (disable: 4786)
#include <set>


struct Statistics
{
	Statistics()
	{
		_frameCounter = -1;
		_frameRate = -1;
	}

	static Statistics & instance()
	{
		return _instance;
	}

	std::string getFrameString()
	{
		std::ostringstream oss;
		oss << "Frames per second: ";
		if (_frameRate < 0)
			oss << "N/A\n";
		else
			oss << std::setw(5) << std::setprecision(4) << _frameRate << "\n";
		oss << "Static triangles rendered: " << trianglesRendered << "\n";
		oss << "Model vertices: " << modelVerticesRendered << "\n";
		oss << "Dynamic object triangles: " << dynamicTrianglesRendered << "\n";
		oss << "Triangles tested for collision: " << triangleCollisions << "\n";
		oss << "Standing in rooms: " << _rooms << "\n";
		oss << "Texture switches: " << textureSwitches << "\n";
		oss << "Object switches: " << objectSwitches << "\n";


		bool first = true;
		std::set<std::string>::iterator it;
		oss << "Rooms visible from here: ";
		for (it = _visibleRooms.begin(); it != _visibleRooms.end(); it++)
		{
			if (!first) oss << ", ";
			oss << *it;
			first = false;
		}
		oss << "\n";
		return oss.str();
	}

	void onNewFrame(double timeStamp)
	{
		if (_frameCounter == -1)
		{
			_lastTimeStamp = timeStamp;
			_frameCounter = 0;
		}
		else if ((timeStamp - _lastTimeStamp) >= 1.0)
		{
			float timePassed = (timeStamp - _lastTimeStamp);
			_frameRate = static_cast<float>(_frameCounter) / timePassed;
			_lastTimeStamp = timeStamp;
			_frameCounter = 0;
		}
		_frameCounter++;
	}

	void addInRoom(std::string name)
	{
		if (_rooms.size() != 0)
			_rooms += ", ";
		_rooms += name;
	}

	void addVisibleRoom(std::string name)
	{
		_visibleRooms.insert(name);
	}

	void resetFrameStats()
	{
		trianglesRendered = 0;
		triangleCollisions = 0;
		_rooms = std::string();
		//_visibleRooms.clear();
		modelVerticesRendered = 0;
		dynamicTrianglesRendered = 0;
		textureSwitches = 0;
		objectSwitches = 0;
	}

	int		trianglesRendered;
	int		dynamicTrianglesRendered;
	int		triangleCollisions;
	int		modelVerticesRendered;
	int		textureSwitches;
	int		objectSwitches;
private:
	double					_lastTimeStamp;
	int						_frameCounter;
	double					_frameRate;
	std::string				_rooms;
	std::set<std::string>	_visibleRooms;

	static					Statistics _instance;
};

#endif
