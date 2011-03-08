#include <list>
#include "GL/glut.h"
#include "GL/gl.h"
#include "skeletonjelly.hpp"
#include "KinectUser.h"
#include "Garment.h"

#define WINDOW_X 800
#define WINDOW_Y 600

#define ROOM_X 4.0f // m
#define ROOM_Y 3.0f // m

#define GRID_SIZE 0.25f // 25 cm

#define SCALE(x) ((x) / 1000.0f)

bool g_running = false;
Kinect g_kinect;

char g_message[64] = {0};
char g_coords[64] = {0};
char g_leftHand[64] = {0};
char g_rightHand[64] = {0};
XnUInt32XYPair res;

bool drawImage = true;

float imageOffset[2];

float scale = 0;

unsigned char* imageBuffer;

const KinectUser *g_userData = NULL;

Garment* g = 0;

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
	_snprintf(g_message, 64, "User [%d]: %s", id, MESSAGES[cb_type]);

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
		glVertex2f(-scale,res.Y -scale);

		glTexCoord2f(1,1);
		glVertex2f(res.X + scale, res.Y -scale);

		glTexCoord2f(1,0);
		glVertex2f(res.X + scale, scale);

		glTexCoord2f(0,0);
		glVertex2f(-scale, scale);
	glEnd();

	_snprintf(g_rightHand, 64, "Scale: %.2f\n", scale);
	_snprintf(g_leftHand, 64, "Offset: %.2fx%.2f\n", imageOffset[0], imageOffset[1]);
}

void drawJoint(XnSkeletonJointPosition& joint)
{
	convertToProjCoordinates(joint);

	glColor3f(.4f, .5f, .8f);
			
	glBegin(GL_POINTS);
	glVertex3f(joint.position.X, joint.position.Y, .1f);
	glEnd();
}


//////////////////////TMP////////////////////////

void drawTexture()
{
	if(!g)
		g = new Garment("buzz-model/torso.png");

	if(g_userData)
	{
		XnFloat x = g_userData->centerOfMass.X;
		XnFloat y = g_userData->centerOfMass.Y;
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
		glPopMatrix();
	}
}

/////////////////////////////////////////////////

void drawTracking()
{
	drawTexture();
	glPointSize(15.0f);
	glLineWidth(8.0f);

	if (g_userData)
	{
		glPushMatrix();
		glTranslatef(imageOffset[0], imageOffset[1], 0);

		const XnPoint3D *com = &g_userData->centerOfMass;

    	glColor3f(0.66, 0.33, 0.33);
		if (g_userData->status & Kinect::USER_TRACKING)
        	glColor3f(0.33, 0.66, 0.33);

		glBegin(GL_POINTS);
			glVertex3f(com->X, com->Y, 0.1f);
		glEnd();

		_snprintf(g_coords, 64, "CoM: (%0.4f, %0.4f, %0.4f)\n", com->X, com->Y, com->Z);

        if (g_kinect.userStatus() & Kinect::USER_TRACKING)
		{
			XnSkeletonJointPosition joint;

			for(int i=0;i<g_kinect.KINECT_JOINT_MAX;++i)
			{
				joint = g_userData->joints[i];
				printf("%d: %f,%f\n", i, joint.position.X, joint.position.Y);
				drawJoint(joint);
			}
			printf("\n");
		}
		glPopMatrix();
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

	drawTracking();
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

void glutSpecialKeyboard(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_DOWN:
		scale -= 10;
		break;
	case GLUT_KEY_UP:
		scale += 10;
		break;
	}
}

//void newUserTrackin

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 's':
		imageOffset[1] += 1;
		break;
	case 'w':
		imageOffset[1] -= 1;
		break;
	case 'a':
		imageOffset[0] -= 1;
		break;
	case 'd':
		imageOffset[0] += 1;
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
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutSpecialFunc(glutSpecialKeyboard);
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

int main(int argc, char **argv)
{
	imageOffset[0] = 0;
	imageOffset[1] = 0;

	g_kinect.setEventCallback(kinect_status, NULL);
	g_kinect.setRenderFormat(Kinect::RENDER_RGBA);
	g_kinect.setTicksPerSecond(30);
	g_kinect.init(Kinect::SENSOR_VGA_30FPS, Kinect::SENSOR_VGA_30FPS);

	res = g_kinect.getDepthResolution();

	glInit(&argc, argv);

	int imageSize = g_kinect.getDepthTexSize();
	imageBuffer = new unsigned char[imageSize];

	Garment::InitializeLibs();

	glutMainLoop();
}
