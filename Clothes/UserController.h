#ifndef _UC
#define _UC
#include "GL/glut.h"
#include "GL/gl.h"
#include "KinectUser.h"
#include "skeletonjelly.hpp"
#include "Garment.h"
#include "Outfit.h"
#include <list>

class UserController
{
	Kinect* g_kinect;
	std::list<Garment> HelmetList;
	std::list<Outfit> OutfitList;
	void convertToProjCoordinates(XnSkeletonJointPosition &joint);
	void drawJoint(XnSkeletonJointPosition&);
	void drawTrackedUser(KinectUser*);
	void drawNewUser(KinectUser*);

public:
	UserController(Kinect* k) : g_kinect(k){};
	void drawUser(KinectUser*);
	void nextHelmet(KinectUser*);
	void nextOutfit(KinectUser*);
};

#endif