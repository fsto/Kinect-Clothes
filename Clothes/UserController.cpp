#include "UserController.h"

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
//			printf("%d: %f,%f\n", i, joint.position.X, joint.position.Y);
			drawJoint(joint);
		}
//		drawTexture(user);
//		printf("\n");
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