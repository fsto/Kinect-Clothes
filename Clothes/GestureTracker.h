#ifndef _tracker
#define _tracker

#include "skeletonjelly.hpp"
#include <ctime>

struct Coord
{
	XnFloat X;
	XnFloat Y;
};

enum TRACKER_TYPE
{
	TRACKER_TYPE_OUTFIT,
	TRACKER_TYPE_HELMET
};

class Tracker
{
	time_t startedRight, startedLeft;
	bool gotRight, gotLeft;
	KinectUser *user;
	void (*callback)(KinectUser*, int);
public:
	Tracker(KinectUser *_user, void (*_callback)(KinectUser*, int)) : user(_user), callback(_callback), gotRight(0), gotLeft(0){};
	void Tick();
	void reset();
};

#endif