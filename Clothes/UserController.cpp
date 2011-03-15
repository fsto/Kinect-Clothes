#include "UserController.h"
#include <fstream>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>

UserController::UserController(Kinect* k) 
: g_kinect(k)
{
	std::fstream f("skins/outfits.txt");
	int countOutfits, countHelmets;
	f >> countOutfits;
	f.ignore(1000, '\n');
	f >> countHelmets;
	printf("Found %d outfit(s) and %d helmet(s)\n", countOutfits, countHelmets);
	for(int i=1; i<=countOutfits; ++i)
	{
		char buffer[10];
		sprintf(buffer, "%d", i);
		OutfitList.push_back(new Outfit(buffer));
	}
	for(int i=1; i<=countHelmets; ++i)
	{
		char buffer[100];
		sprintf(buffer, "skins/helmets/%d.png", i);
		HelmetList.push_back(new Garment(buffer));
	}
	//playSound();
}



void UserController::convertToProjCoordinates(XnSkeletonJointPosition &joint)
{
	xn::DepthGenerator* depth = g_kinect->Depth();
	depth->ConvertRealWorldToProjective(1,&joint.position,&joint.position);
}

void UserController::drawJoint(XnSkeletonJointPosition& joint)
{
	convertToProjCoordinates(joint);

	glColor3f(.4f, .5f, .8f);
			
	glBegin(GL_POINTS);
	glVertex3f(joint.position.X, joint.position.Y, .1f);
	glEnd();
}


#define PI 3.1415

float UserController::getAngle(XnVector3D& a, XnVector3D& b)
{
	XnFloat dotproduct, length, result;
 
	XnVector3D yAxis;
	XnVector3D vector;

	getVectorBetween(a, b, &vector);
	
	length = sqrt(vector.X * vector.X+ vector.Y * vector.Y);
	yAxis.X=0;
	yAxis.Y = length;

	dotproduct = (vector.X * yAxis.X) + (vector.Y * yAxis.Y);

	result = acos( dotproduct / (length * length) );
 
	result += PI/2;
	if(a.X < b.X)
	{
			result = -result;
	}
	else
	{
		result += PI;
	}
	return result;
}

float UserController::getDistance(XnVector3D& a, XnVector3D& b)
{
	float dist = sqrt(pow((a.X-b.X),2) + pow((a.Y-b.Y),2));	
	return dist;
}

float UserController::getDistance3D(XnVector3D& a, XnVector3D& b)
{
	float dist = sqrt(pow((a.X-b.X),2) + pow((a.Y-b.Y),2) + pow((a.Z-b.Z),2));	
	return dist;
}

void UserController::getVectorBetween(XnVector3D& a, XnVector3D& b, XnVector3D* c)
{
	c->X = a.X-b.X;
	c->Y = a.Y-b.Y;
}

void UserController::getMidPoint(XnVector3D& a, XnVector3D& b, XnVector3D* mid) 
{
	mid->X = (a.X+b.X)/2;
	mid->Y = (a.Y+b.Y)/2;
}
void UserController::drawTexture(XnVector3D& pt1, XnVector3D& pt2, XnFloat w)
{

	XnVector3D m;
	XnFloat l = getDistance(pt1, pt2)*0.7;
	XnFloat angle = getAngle(pt1,pt2);

	//XnFloat w = ((l / s.X) * s.Y)/2;
	m.X = (pt1.X + pt2.X)/2;
	m.Y = (pt1.Y + pt2.Y)/2;

//	glColor3f(.9f, .5f, .8f);	
//	glBegin(GL_POINTS);
//		glVertex3f(m.X, m.Y, .1f);
//	glEnd();
	
	glColor3f(1,1,1);

	glPushMatrix();
	glTranslatef(m.X,m.Y,0);
	glRotatef(-angle*180/PI,0,0,1);
	glBegin(GL_QUADS);

	glTexCoord2f(1,1);
	glVertex2f(l,w);
	glTexCoord2f(0,1);
	glVertex2f(l,-w);
	glTexCoord2f(0,0);
	glVertex2f(-l,-w);
	glTexCoord2f(1,0);
	glVertex2f(-l,w);

	glEnd();
	glPopMatrix();

}
void UserController::drawHelmet(KinectUser *user)
{
	glColor3f(1,1,1);
	XnVector3D m = user->joints[XN_SKEL_HEAD-1].position;
	XnVector3D pt1 = user->joints[XN_SKEL_NECK-1].position;
	XnVector3D pt2;
	pt2.X = 2 * m.X - pt1.X;
	pt2.Y = 2 * m.Y - pt1.Y;

	HelmetList[user->helmet]->bindTexture();

	drawTexture(pt2,pt1, user->scale/3); //Ändra W

/*
	XnFloat x = pt.X;
	XnFloat y = pt.Y;
	glPushMatrix();

	glTranslatef(x,y,0);


	glBegin(GL_QUADS);
	glTexCoord2f(0,1);
	glVertex2f(-100,-60);
	glTexCoord2f(0,0);
	glVertex2f(-100,60);
	glTexCoord2f(1,0);
	glVertex2f(100,60);
	glTexCoord2f(1,1);
	glVertex2f(100,-60);
	glEnd();

	glPopMatrix();*/
}

void UserController::drawTrackedUser(KinectUser* user)
{
	glPointSize(15.0f);
	glLineWidth(8.0f);

	if (user)
	{

		if(user->joints[XN_SKEL_TORSO-1].fConfidence < .3)
			return;

 		XnSkeletonJointPosition joint;

		for(int i=0;i<Kinect::KINECT_JOINT_MAX;++i)
		{
			joint = user->joints[i];
			drawJoint(joint);

			convertToProjCoordinates(user->joints[i]);
		}

		if(!user->scale)
			calibrateUser(user);
		if(!user->scale)
			return;

		glEnable(GL_TEXTURE_2D);

		drawHelmet(user); //Rita huvud

		Outfit *outfit = OutfitList[user->outfit];
		XnVector3D pt1, pt2;
		XnFloat w = user->scale/3;//getDistance3D(user->joints[XN_SKEL_RIGHT_SHOULDER-1].position,user->joints[XN_SKEL_LEFT_SHOULDER-1].position);

		int firstUpperLeg	= user->joints[XN_SKEL_RIGHT_KNEE-1].position.Z 
							< user->joints[XN_SKEL_LEFT_KNEE-1].position.Z
							? Outfit::OUTFIT_RIGHT_UPPER_LEG
							: Outfit::OUTFIT_LEFT_UPPER_LEG;
		int secondUpperLeg	= firstUpperLeg == Outfit::OUTFIT_RIGHT_UPPER_LEG
							? Outfit::OUTFIT_LEFT_UPPER_LEG
							: Outfit::OUTFIT_RIGHT_UPPER_LEG;
		int firstUnderLeg	= firstUpperLeg == Outfit::OUTFIT_RIGHT_UPPER_LEG
							? Outfit::OUTFIT_RIGHT_UNDER_LEG
							: Outfit::OUTFIT_LEFT_UNDER_LEG;
		int secondUnderLeg	= firstUpperLeg == Outfit::OUTFIT_RIGHT_UPPER_LEG
							? Outfit::OUTFIT_LEFT_UNDER_LEG
							: Outfit::OUTFIT_RIGHT_UNDER_LEG;
		int garmentOrder[NUM_GARMENTS];

		for(int i = 0; i < NUM_GARMENTS; i++){
			if (	i == Outfit::OUTFIT_LEFT_UPPER_LEG
				||	i == Outfit::OUTFIT_RIGHT_UPPER_LEG
				||	i == Outfit::OUTFIT_LEFT_UNDER_LEG
				||	i == Outfit::OUTFIT_RIGHT_UNDER_LEG){
				garmentOrder[i] = firstUpperLeg;
				garmentOrder[++i] = firstUnderLeg;
				garmentOrder[++i] = secondUpperLeg;
				garmentOrder[++i] = secondUnderLeg;
			}
			else{
				garmentOrder[i] = i;
			}
		}

		for(int i=0; i<NUM_GARMENTS;++i)
		{
			int orderedi = garmentOrder[i];
			outfit->getOutfitGarment(orderedi)->bindTexture();
			bool draw = false;
			switch(orderedi)
			{
			case Outfit::OUTFIT_TORSO:
				{
					getMidPoint(user->joints[XN_SKEL_RIGHT_SHOULDER-1].position,user->joints[XN_SKEL_LEFT_SHOULDER-1].position, &pt1);
					getMidPoint(user->joints[XN_SKEL_RIGHT_HIP-1].position,user->joints[XN_SKEL_LEFT_HIP-1].position, &pt2);
					drawTexture(pt1,pt2, w);
					break;
				}
			case Outfit::OUTFIT_LEFT_UPPER_ARM:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_SHOULDER-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_ELBOW-1].position;
					drawTexture(pt1,pt2, w/3);
					break;
				}
			case Outfit::OUTFIT_LEFT_UNDER_ARM:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_ELBOW-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_HAND-1].position;
					drawTexture(pt1,pt2, w/3);
					break;
				}
			case Outfit::OUTFIT_LEFT_UPPER_LEG:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_HIP-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_KNEE-1].position;
					drawTexture(pt1,pt2, w/2);
					break;
				}
			case Outfit::OUTFIT_LEFT_UNDER_LEG:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_KNEE-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_FOOT-1].position;
					drawTexture(pt1,pt2, w/2);
					break;
				}
			case Outfit::OUTFIT_RIGHT_UPPER_ARM:
				{
					pt1 = user->joints[XN_SKEL_LEFT_SHOULDER-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_ELBOW-1].position;
					drawTexture(pt1,pt2, w/3);
					break;
				}
			case Outfit::OUTFIT_RIGHT_UNDER_ARM:
				{
					pt1 = user->joints[XN_SKEL_LEFT_ELBOW-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_HAND-1].position;
					drawTexture(pt1,pt2, w/3);
					break;
				}
			case Outfit::OUTFIT_RIGHT_UPPER_LEG:
				{
					pt1 = user->joints[XN_SKEL_LEFT_HIP-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_KNEE-1].position;
					drawTexture(pt1,pt2, w/2);
					break;
				}
			case Outfit::OUTFIT_RIGHT_UNDER_LEG:
				{
					pt1 = user->joints[XN_SKEL_LEFT_KNEE-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_FOOT-1].position;
					drawTexture(pt1,pt2, w/2);
					break;
				}
			}
		}
		glDisable(GL_TEXTURE_2D);
	}
}

/*
* Draw a user that is not tracked. The only available point is CoM.
*/
void UserController::drawNewUser(KinectUser* user)
{
	static Garment untracked("skins/untracked.png");

	//draw untracked image
	glColor3f(1,1,1);
	XnFloat x = user->centerOfMass.X;
	XnFloat y = user->centerOfMass.Y;
	glPushMatrix();
	glTranslatef(0,-50,0);

	untracked.bindTexture();

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);
	glVertex2f(x-80,y-100);
	glTexCoord2f(0,0);
	glVertex2f(x-80,y+100);
	glTexCoord2f(1,0);
	glVertex2f(x+80,y+100);
	glTexCoord2f(1,1);
	glVertex2f(x+80,y-100);
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}



void UserController::drawUser(KinectUser* g_userData)
{
	if (g_userData)
	{
//		drawTexture(g_userData);
		if (g_userData->status & Kinect::USER_TRACKING)
			drawTrackedUser(g_userData);
		else
			drawNewUser(g_userData);			
	}
}

void UserController::playSound()
{
	PlaySound("audio/1.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void UserController::calibrateUser(KinectUser *user)
{
	float rElbowC = user->joints[XN_SKEL_RIGHT_ELBOW - 1].fConfidence;
	float lElbowC = user->joints[XN_SKEL_LEFT_ELBOW - 1].fConfidence;
	float rShoulderC = user->joints[XN_SKEL_RIGHT_SHOULDER - 1].fConfidence;
	float lShoulderC = user->joints[XN_SKEL_LEFT_SHOULDER - 1].fConfidence;

	if(lElbowC > CONF_LIMIT && rElbowC > CONF_LIMIT && lShoulderC > CONF_LIMIT && rShoulderC > CONF_LIMIT)
	{
		XnVector3D rElbow = user->joints[XN_SKEL_RIGHT_ELBOW - 1].position;
		XnVector3D lElbow = user->joints[XN_SKEL_LEFT_ELBOW - 1].position;
		XnVector3D rShoulder = user->joints[XN_SKEL_RIGHT_SHOULDER - 1].position;
		XnVector3D lShoulder = user->joints[XN_SKEL_LEFT_SHOULDER - 1].position;

		user->scale = getDistance(rElbow, rShoulder);
		user->scale += getDistance(lShoulder, rShoulder);
		user->scale += getDistance(lElbow, lShoulder);
		printf("User scale: %f\n", user->scale);
	}
}


/*
void drawTracking()
{

	if (g_userData)
	{
		if (g_userData->userStatus(g_userData.userId()) & Kinect::USER_TRACKING)
			userController.drawTrackedUser(USER);
		else
			userController.drawNewUser(USER);			
	}
}
*/