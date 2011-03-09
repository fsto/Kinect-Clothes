#ifndef _garment
#define _garment

#include "IL/ilut.h"

class Garment
{
	GLuint id;
	public:
	static void InitializeLibs()
	{
		ilInit();
		iluInit();
		ilutInit();
		ilutRenderer(ILUT_OPENGL);
	}
	Garment(char* filepath);
	void bindTexture();


};


#endif

