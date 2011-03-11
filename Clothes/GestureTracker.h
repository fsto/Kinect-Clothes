#ifndef _tracker
#define _tracker

#include "skeletonjelly.hpp"

struct Coord
{
	XnFloat X;
	XnFloat Y;
};

class Tracker
{
	Coord oldCoords[30];
	KinectUser *user;
	void (*callback)(KinectUser*, int);
public:
	Tracker(KinectUser *_user, void (*_callback)(KinectUser*, int)) : user(_user), callback(_callback){};
	void Tick();
};

#endif