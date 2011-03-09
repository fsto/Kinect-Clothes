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
	std::list<Garment> HelmetList;
	std::list<Outfit> OutfitList;
	public:
		void drawJoint(XnSkeletonJointPosition&);
		void drawTrackedUser(KinectUser*);
		void drawNewUser(KinectUser*);
		void nextHelmet(KinectUser*);
		void nextOutfit(KinectUser*);
};

#endif