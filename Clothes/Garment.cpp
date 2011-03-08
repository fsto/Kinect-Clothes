#include "Garment.h"

Garment::Garment(char* filepath)
{
	id = ilutGLLoadImage(filepath);
	if(!id)
	{
		printf("Fatal error, could not load %s\n", filepath);
	}
}

void Garment::bindTexture()
{
	ilutGLSetTex(id);
	ilutGLBindTexImage();
}
