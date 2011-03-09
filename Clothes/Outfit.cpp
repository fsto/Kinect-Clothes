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
		strcpy_s(path, "skins/outfit-");
		strcat_s(path, id);
		strcat_s(path, "/");
		strcat_s(path, parts[i]);
		strcat_s(path, ".png");
			
		garments[i] = new Garment(path);
	}
}
