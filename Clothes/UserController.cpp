#include "UserController.h"
#include <fstream>
#include <cmath>

UserController::UserController(Kinect* k) 
: g_kinect(k)
{
	std::fstream f("skins/outfits.txt");
	int count;
	f >> count;
	printf("Found %d outfits\n", count);
	for(int i=1; i<=count; ++i)
	{
		char buffer[10];
		sprintf(buffer, "%d", i);
		OutfitList.push_back(new Outfit(buffer));
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
Garment* g;
Garment* g2;

/*void drawTexture(KinectUser* user)
{
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);
	if(!g)
	{
		g = new Garment("buzz-model/torso.png");
		g2 = new Garment("buzz-model/helmet.png");
	}
	XnFloat x = user->centerOfMass.X;
	XnFloat y = user->centerOfMass.Y;
	glPushMatrix();
	glTranslatef(0,-30,0);

	g->bindTexture();
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

	glTranslatef(0,-80,0);

	g2->bindTexture();
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
}*/

float UserController::getAngle(XnVector3D& a, XnVector3D& b)
{
 float dotproduct, lengtha, lengthb, result;
 
	dotproduct = (a.X * b.X) + (a.Y * b.Y);
	lengtha = sqrt(a.X * a.X+ a.Y * a.Y);
	lengthb = sqrt(b.X * b.X + b.Y * b.Y);

	result = acos( dotproduct / (lengtha * lengthb) );
 
	if(dotproduct < 0)
	{
		if(result > 0)
			result += 3.1415;
		else
			result -= 3.1415;
	}
	return result;
}

float UserController::getDistance(XnVector3D& a, XnVector3D& b)
{
	float dist = sqrt(pow((a.X-b.X),2) + pow((a.Y-b.Y),2));	
	return dist;
}

void getVectorBetween(XnVector3D& a, XnVector3D& b, XnVector3D* c)
{
	c->X = a.X-b.X;
	c->Y = a.Y-b.Y;
}

/*
* REMEMBER TO DELETE RETURNED VECTOR.
*/
void UserController::getMidPoint(XnVector3D& a, XnVector3D& b, XnVector3D* mid) 
{
	mid = new XnVector3D();
	mid->X = (a.X+b.X)/2;
	mid->Y = (a.Y+b.Y)/2;
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
		}
		Outfit *outfit = OutfitList[user->outfit];
		for(int i=0; i<NUM_GARMENTS;++i)
		{
			outfit->getOutfitGarment(i)->bindTexture();
			switch(i)
			{
			case Outfit::OUTFIT_RIGHT_UNDER_ARM:
				{
					XnVector3D pt1 = user->joints[XN_SKEL_LEFT_ELBOW-1].position;
					XnVector3D pt2 = user->joints[XN_SKEL_LEFT_HAND-1].position;

					glBegin(GL_QUADS);

						

					glEnd();

					printf("Coords %.2fx%.2f -> %.2fx%.2f\n", pt1.X, pt1.Y, pt2.X, pt2.Y);
					break;
				}
			}
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

void UserController::nextHelmet(KinectUser* g_userData)
{

}

void UserController::nextOutfit(KinectUser* g_userData)
{

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