#include "GestureTracker.h"

#define WAIT_TIME 2

void Tracker::Tick()
{
	XnVector3D head = user->joints[XN_SKEL_HEAD-1].position;
	XnVector3D left = user->joints[XN_SKEL_LEFT_HAND-1].position;
	XnVector3D right = user->joints[XN_SKEL_RIGHT_HAND-1].position;

	if(right.Y > head.Y)
	{
		//Vill byta outfit
		if(gotRight)
		{
			if(difftime(time(NULL), startedRight) > WAIT_TIME)
			{
				if(callback)
					(callback)(user, TRACKER_TYPE_OUTFIT);
				else
					printf("No tracker callback!\n");
				gotRight = false;
			}
		}
		else
		{
			gotRight = true;
			startedRight = time(NULL);
		}
	}
	else
		gotRight = false;
	if(left.Y > head.Y)
	{
		if(gotLeft)
		{
			if(difftime(time(NULL), startedLeft) > WAIT_TIME)
			{
				if(callback)
					(callback)(user, TRACKER_TYPE_HELMET);
				else
					printf("No tracker callback!\n");
				gotLeft = false;
			}
		}
		else
		{
			gotLeft = true;
			startedLeft = time(NULL);
		}
	}
	else
		gotLeft = false;

}