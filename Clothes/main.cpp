#include <vld.h>

#include <list>
#include "GL/glut.h"
#include "GL/gl.h"
#include "skeletonjelly.hpp"
#include "KinectUser.h"
#include "UserController.h"

#define WINDOW_X 800
#define WINDOW_Y 600

bool g_running = false;
Kinect g_kinect;

char g_message[64] = {0};
char g_coords[64] = {0};
char g_leftHand[64] = {0};
char g_rightHand[64] = {0};

XnUInt32XYPair res;

bool drawImage = true;

UserController *uc;

unsigned char* imageBuffer;

const KinectUser *g_userData = NULL;

static const char *MESSAGES[] =
{
	"Found user",
	"Lost user",
	"Pose detected",
	"Calibration started...",
	"TRACKING",
	"Calibration failed"
};

void convertToProjCoordinates(XnSkeletonJointPosition &joint)
{
	xn::DepthGenerator* depth = g_kinect.Depth();
	depth->ConvertRealWorldToProjective(1,&joint.position,&joint.position);
}


void kinect_status(Kinect *k, Kinect::CallbackType cb_type, XnUserID id, void *data)
{
	_snprintf_s(g_message, 64, "User [%d]: %s", id, MESSAGES[cb_type]);

	if (cb_type == Kinect::CB_NEW_USER && id == 1)
	{
		g_userData = k->getUserData(id);
	}
}

void glPrintString(void *font, char *str)
{
	int i,l = strlen(str);

	for(i=0; i<l; i++)
	{
		glutBitmapCharacter(font,*str++);
	}
}

void drawBackground()
{
	if(drawImage)
		g_kinect.renderImage(imageBuffer, 0);
	else
		g_kinect.renderDepth(imageBuffer, true, 0);

	glColor3f(1,1,1);

	glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.X, res.Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer);

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2f(0, res.Y);

		glTexCoord2f(1,1);
		glVertex2f(res.X, res.Y);

		glTexCoord2f(1,0);
		glVertex2f(res.X, 0);

		glTexCoord2f(0,0);
		glVertex2f(0, 0);
	glEnd();

//	_snprintf(g_rightHand, 64, "Scale: %.2f\n", scale);
//	_snprintf(g_leftHand, 64, "Offset: %.2fx%.2f\n", imageOffset[0], imageOffset[1]);
}

void drawUsers()
{
	for(int i=1; i<=g_kinect.getMaxUsers(); ++i)
	{
		KinectUser *user = g_kinect.getUserData(i);
		if(user)
		{
			uc->drawUser(user);
		}
	}
}

void drawHUD()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, WINDOW_X, 0, WINDOW_Y, -1.0, 1.0);
	glDisable(GL_DEPTH_TEST); 

	glColor3f(1, 1, 1);

	glRasterPos2i(10, 10);
	glPrintString(GLUT_BITMAP_HELVETICA_18, g_message);

	glRasterPos2i(10, 30);
	glPrintString(GLUT_BITMAP_HELVETICA_18, g_coords);

	glRasterPos2i(10, 50);
	glPrintString(GLUT_BITMAP_HELVETICA_18, g_leftHand);

	glRasterPos2i(10, 70);
	glPrintString(GLUT_BITMAP_HELVETICA_18, g_rightHand);

	glEnable(GL_DEPTH_TEST); 
	glPopMatrix();
}


// this function is called each frame
void glutDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glOrtho(0, res.X, res.Y, 0, -1.0, 1.0);
	drawBackground();

	glDisable(GL_TEXTURE_2D);

	drawUsers();
	drawHUD();

	glutSwapBuffers();
}

void glutIdle()
{
	static int time = 0;

	int now = glutGet(GLUT_ELAPSED_TIME);

	g_kinect.tick(now - time);
	glutPostRedisplay();

	time = now;

}

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		drawImage = !drawImage;
		break;
	}
}
void glInit (int *pargc, char **argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(res.X, res.Y);
	glutCreateWindow ("Clothes");
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glClearColor(1.0f, 153.0f / 255.0f, .2f, 1.0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void updateOutfitForUser(KinectUser *user, int type)
{
	uc->nextOutfit(user, type);
}

int main(int argc, char **argv)
{
	g_kinect.setEventCallback(kinect_status, NULL);
	g_kinect.setRenderFormat(Kinect::RENDER_RGBA);
	g_kinect.setTicksPerSecond(30);
	XnStatus status = g_kinect.init(Kinect::SENSOR_VGA_30FPS, Kinect::SENSOR_VGA_30FPS);
	
	if(status != XN_STATUS_OK)
	{
		printf("Got an error while initiating, is the kinect connected?\nThe error message was \"%s\"\nContinue? (y/N)\n", xnGetStatusString(status));
		char input = ' ';
		scanf_s("%c", &input);
		if(input != 'y')
			return(1);
	}

	res = g_kinect.getDepthResolution();

	glInit(&argc, argv);

	int imageSize = g_kinect.getImageTexSize();//getDepthTexSize();

	imageBuffer = new unsigned char[imageSize];

	Garment::InitializeLibs();

	uc = new UserController(&g_kinect);
	g_kinect.trackerCallback = &updateOutfitForUser;

	glutMainLoop();
}
