#ifndef _tracker
#define _tracker

#include "skeletonjelly.hpp"
#include <ctime>

struct Coord
{
	XnFloat X;
	XnFloat Y;
};

class Tracker
{
	time_t startedRight, startedLeft;
	bool gotRight, gotLeft;
	KinectUser *user;
	void (*callback)(KinectUser*, int);
public:
	Tracker(KinectUser *_user, void (*_callback)(KinectUser*, int)) : user(_user), callback(_callback){};
	void Tick();
};

#endif