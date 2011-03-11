#ifndef _kinectUser
#define _kinectUser

#include <XnCppWrapper.h>

struct KinectUser
{
	int outfit;
	int helmet;
	int status;
	XnSkeletonJointPosition joints[24];
	XnPoint3D centerOfMass;
	KinectUser() : outfit(0), helmet(0) {}
};

#endif