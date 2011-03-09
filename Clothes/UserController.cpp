#include "UserController.h"

void convertToProjCoordinates(XnSkeletonJointPosition &joint)
{
	xn::DepthGenerator* depth = g_kinect.Depth();
	depth->ConvertRealWorldToProjective(1,&joint.position,&joint.position);
}

void drawJoint(XnSkeletonJointPosition& joint)
{
	convertToProjCoordinates(joint);

	glColor3f(.4f, .5f, .8f);
			
	glBegin(GL_POINTS);
	glVertex3f(joint.position.X, joint.position.Y, .1f);
	glEnd();
}

void drawTrackedUser(KinectUser* g_userData)
{
	glPointSize(15.0f);
	glLineWidth(8.0f);

	if (g_userData)
	{

 		XnSkeletonJointPosition joint;

		for(int i=0;i<Kinect::KINECT_JOINT_MAX;++i)
		{
			joint = g_userData->joints[i];
//			printf("%d: %f,%f\n", i, joint.position.X, joint.position.Y);
			drawJoint(joint);
		}
//		printf("\n");
	}
}

/*
* Draw a user that is not tracked. The only available point is CoM.
*/
void drawNewUser(KinectUser* g_userData)
{
	glPointSize(15.0f);
	const XnPoint3D *com = &g_userData->centerOfMass;
   	glColor3f(0.66, 0.33, 0.33);
	glBegin(GL_POINTS);
		glVertex3f(com->X, com->Y, 0.1f);
	glEnd();
//	_snprintf(g_coords, 64, "CoM: (%0.4f, %0.4f, %0.4f)\n", com->X, com->Y, com->Z);

}

void drawUser(KinectUser* g_userData)
{
	if (g_userData)
	{
		if (g_userData->status & Kinect::USER_TRACKING)
			drawTrackedUser(g_userData);
		else
			drawNewUser(g_userData);			
	}
}

void nextHelmet(KinectUser* g_userData)
{

}

void nextOutfit(KinectUser* g_userData)
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