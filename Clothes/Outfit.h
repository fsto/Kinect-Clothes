#ifndef _outfit
#define _outfit
#define NUM_GARMENTS 9
#include "Garment.h"

class Outfit
{
	Garment* garments[NUM_GARMENTS];
	
	public:
		Outfit(char*);
		~Outfit();
		Garment* getOutfitGarment(int id){return garments[id]; }

		enum OutfitGarments
		{
			OUTFIT_RIGHT_UPPER_LEG,
			OUTFIT_LEFT_UPPER_LEG,
			OUTFIT_RIGHT_UNDER_LEG,
			OUTFIT_LEFT_UNDER_LEG,
			OUTFIT_RIGHT_UPPER_ARM,
			OUTFIT_LEFT_UPPER_ARM,
			OUTFIT_TORSO,
			OUTFIT_RIGHT_UNDER_ARM,
			OUTFIT_LEFT_UNDER_ARM
		};
};


#endif

