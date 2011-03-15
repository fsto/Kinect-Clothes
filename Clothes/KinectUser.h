#ifndef _kinectUser
#define _kinectUser

#include <XnCppWrapper.h>

class Tracker;

struct KinectUser
{
	unsigned int outfit;
	unsigned int helmet;
	float scale;
	Tracker *tracker;
	int status;
	XnSkeletonJointPosition joints[24];
	XnPoint3D centerOfMass;
	KinectUser() : outfit(0), helmet(0), scale(1) {}
};

#endif