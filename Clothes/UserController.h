#ifndef _UC
#define _UC
#include "GL/glut.h"
#include "GL/gl.h"
#include "KinectUser.h"
#include "skeletonjelly.hpp"
#include "Garment.h"
#include "Outfit.h"
#include "GestureTracker.h"
#include <vector>
#include "SoundPlayer.h"

#define CONF_LIMIT .95
#define DRAW_USER_CONFIDENCE .3

class UserController
{
	Kinect* g_kinect;
	std::vector<Garment*> HelmetList;
	std::vector<Outfit*> OutfitList;
	SoundPlayer* player;
	void convertToProjCoordinates(XnSkeletonJointPosition &joint);
	void drawJoint(XnSkeletonJointPosition&);
	void drawTrackedUser(KinectUser*);
	void drawNewUser(KinectUser*);
	float getAngle(XnVector3D&, XnVector3D&);
	void getVectorBetween(XnVector3D&, XnVector3D&, XnVector3D*);
	float getDistance(XnVector3D&, XnVector3D&);
	float getDistance3D(XnVector3D&, XnVector3D&);
	void getMidPoint(XnVector3D&, XnVector3D&, XnVector3D*);
	void drawTexture(XnVector3D&, XnVector3D&, XnFloat);
	void drawHelmet(KinectUser*);
	void calibrateUser(KinectUser *);

public:
	UserController(Kinect* k);
	void drawUser(KinectUser*);
	void nextOutfit(KinectUser* user, int type);
	void attractUsers();
	void greet();
};

#endif