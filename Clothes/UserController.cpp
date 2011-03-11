#include "UserController.h"
#include <fstream>
#include <cmath>

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
		sprintf(buffer, "skins/outfit-%d/helmet.png", i);
		HelmetList.push_back(new Garment(buffer));
	}
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

void UserController::drawHelmet(KinectUser *user, XnVector3D& pt)
{
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);
	XnFloat x = pt.X;
	XnFloat y = pt.Y;
	glPushMatrix();

	glTranslatef(x,y,0);

	HelmetList[user->helmet]->bindTexture();
	glEnable(GL_TEXTURE_2D);
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

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
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
	//XnFloat w = ((l / s.X) * s.Y)/2;
	m.X = (pt1.X + pt2.X)/2;
	m.Y = (pt1.Y + pt2.Y)/2;

	glColor3f(.9f, .5f, .8f);	
	glBegin(GL_POINTS);
		glVertex3f(m.X, m.Y, .1f);
	glEnd();
	
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(m.X,m.Y,0);
	XnFloat angle = getAngle(pt1,pt2);
	glRotatef(-angle*180/PI,0,0,1);
	glEnable(GL_TEXTURE_2D);
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

	glDisable(GL_TEXTURE_2D);

}

void UserController::drawTrackedUser(KinectUser* user)
{
	glPointSize(15.0f);
	glLineWidth(8.0f);

	if (user)
	{

 		XnSkeletonJointPosition joint;

		for(int i=0;i<Kinect::KINECT_JOINT_MAX;++i)
		{
			joint = user->joints[i];
			drawJoint(joint);

			convertToProjCoordinates(user->joints[i]);
		}

		drawHelmet(user, user->joints[XN_SKEL_HEAD-1].position); //Rita huvud

		Outfit *outfit = OutfitList[user->outfit];
		XnVector3D pt1, pt2, s;
		XnFloat w;
		for(int i=0; i<NUM_GARMENTS;++i)
		{
			outfit->getOutfitGarment(i)->bindTexture();
			bool draw = false;
			w = 30;
			switch(i)
			{
			case Outfit::OUTFIT_TORSO:
				{
					getMidPoint(user->joints[XN_SKEL_RIGHT_SHOULDER-1].position,user->joints[XN_SKEL_LEFT_SHOULDER-1].position, &pt1);
					getMidPoint(user->joints[XN_SKEL_RIGHT_HIP-1].position,user->joints[XN_SKEL_LEFT_HIP-1].position, &pt2);
					w = 100;					
					draw = true;
					break;
				}
			case Outfit::OUTFIT_LEFT_UPPER_ARM:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_SHOULDER-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_ELBOW-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_LEFT_UNDER_ARM:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_ELBOW-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_HAND-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_LEFT_UPPER_LEG:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_HIP-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_KNEE-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_LEFT_UNDER_LEG:
				{
					pt1 = user->joints[XN_SKEL_RIGHT_KNEE-1].position;
					pt2 = user->joints[XN_SKEL_RIGHT_FOOT-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_RIGHT_UPPER_ARM:
				{
					pt1 = user->joints[XN_SKEL_LEFT_SHOULDER-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_ELBOW-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_RIGHT_UNDER_ARM:
				{
					pt1 = user->joints[XN_SKEL_LEFT_ELBOW-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_HAND-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_RIGHT_UPPER_LEG:
				{
					pt1 = user->joints[XN_SKEL_LEFT_HIP-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_KNEE-1].position;
					draw = true;
					break;
				}
			case Outfit::OUTFIT_RIGHT_UNDER_LEG:
				{
					pt1 = user->joints[XN_SKEL_LEFT_KNEE-1].position;
					pt2 = user->joints[XN_SKEL_LEFT_FOOT-1].position;
					draw = true;
					break;
				}

			}
			if(draw)
				drawTexture(pt1,pt2, w);
		}
	}
}

Garment *untracked;

/*
* Draw a user that is not tracked. The only available point is CoM.
*/
void UserController::drawNewUser(KinectUser* user)
{
	//draw CoM
	glPointSize(15.0f);
	const XnPoint3D *com = &user->centerOfMass;
   	glColor3f(0.66, 0.33, 0.33);
	glBegin(GL_POINTS);
		glVertex3f(com->X, com->Y, 0.1f);
	glEnd();

	//draw untracked image
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);
	if(!untracked)
	{
		untracked = new Garment("skins/untracked.png");
	}
	XnFloat x = user->centerOfMass.X;
	XnFloat y = user->centerOfMass.Y;
	glPushMatrix();
	//glTranslatef(0,-30,0);

	untracked->bindTexture();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);
	glVertex2f(x-80,y-80);
	glTexCoord2f(0,0);
	glVertex2f(x-80,y+80);
	glTexCoord2f(1,0);
	glVertex2f(x+80,y+80);
	glTexCoord2f(1,1);
	glVertex2f(x+80,y-80);
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