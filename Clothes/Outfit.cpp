#include "Outfit.h"

Outfit::Outfit(char* id)
{
	const char *parts[NUM_GARMENTS] = {		"torso",
											"left-upper-arm",
											"left-under-arm",
											"left-upper-leg",
											"left-under-leg",
											"right-upper-arm",
											"right-under-arm",
											"right-upper-leg",
											"right-under-leg"
	};
	for(int i = 0; i < NUM_GARMENTS; i++){
		char path[64];
		strcpy(path, "outfit-");
		strcat(path, id);
		strcat(path, "/");
		strcat(path, parts[i]);
			
		garments[i] = new Garment(path);
	}
}

void Garment::bindTexture()
{
	ilutGLSetTex(id);
	ilutGLBindTexImage();
}
