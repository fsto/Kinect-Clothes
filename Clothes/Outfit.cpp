#include "Outfit.h"

Outfit::Outfit(char* id)
{
	const char *parts[NUM_GARMENTS] = {		"right-upper-leg",
											"left-upper-leg",
											"right-under-leg",
											"left-under-leg",
											"right-upper-arm",
											"left-upper-arm",
											"torso",
											"right-under-arm",
											"left-under-arm"
	};
	for(int i = 0; i < NUM_GARMENTS; i++){
		char path[64];
		strcpy_s(path, "skins/outfit-");
		strcat_s(path, id);
		strcat_s(path, "/");
		strcat_s(path, parts[i]);
		strcat_s(path, ".png");
			
		garments[i] = new Garment(path);
	}
}
