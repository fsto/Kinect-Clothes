#ifndef _kinectUser
#define _kinectUser

#include <XnCppWrapper.h>

struct KinectUser
{
	int status;
	XnSkeletonJointPosition joints[24];
	XnPoint3D centerOfMass;
};

#endif