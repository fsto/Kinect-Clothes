#ifndef _UC
#define _UC
#include "GL/glut.h"
#include "GL/gl.h"
#include "KinectUser.h"
#include "skeletonjelly.hpp"
#include "Garment.h"
#include "Outfit.h"
#include <vector>

class UserController
{
	Kinect* g_kinect;
	std::vector<Garment*> HelmetList;
	std::vector<Outfit*> OutfitList;
	void convertToProjCoordinates(XnSkeletonJointPosition &joint);
	void drawJoint(XnSkeletonJointPosition&);
	void drawTrackedUser(KinectUser*);
	void drawNewUser(KinectUser*);
	float getAngle(XnVector3D&, XnVector3D&);
	void getVectorBetween(XnVector3D&, XnVector3D&, XnVector3D*);
	float getDistance(XnVector3D&, XnVector3D&);
	void getMidPoint(XnVector3D&, XnVector3D&, XnVector3D*);
	void drawTexture(XnVector3D&, XnVector3D&, XnFloat);
	void drawHelmet(KinectUser*, XnVector3D&);

public:
	UserController(Kinect* k);
	void drawUser(KinectUser*);
	void nextHelmet(KinectUser* user)
	{
		if(++(user->helmet) >= HelmetList.size())
			user->helmet = 0;
	};
	void nextOutfit(KinectUser* user)
	{
		if(++(user->outfit) >= OutfitList.size())
			user->outfit = 0;
	};

};

#endif