#include "GestureTracker.h"

void Tracker::Tick()
{
	XnVector3D head = user->joints[XN_SKEL_HEAD-1].position;
	XnVector3D left = user->joints[XN_SKEL_LEFT_HAND-1].position;
	XnVector3D right = user->joints[XN_SKEL_RIGHT_HAND-1].position;

	if(right.Y > head.Y)
	{
		printf("Right up\n");
		//Vill byta outfit
		if(gotRight)
		{
			if(difftime(time(NULL) > 1, startedRight))
			{
				printf("Change outfit!\n");
				gotRight = false;
			}
		}
		else
		{
			gotRight = true;
			startedRight = time(NULL);
		}
	}
	if(left.Y > head.Y)
	{
		printf("Left up\n");
		if(gotLeft)
		{
			if(difftime(time(NULL) > 1, startedLeft))
			{
				printf("Change!\n");
				gotLeft = false;
			}
		}
		else
		{
			gotLeft = true;
			startedLeft = time(NULL);
		}
	}
}