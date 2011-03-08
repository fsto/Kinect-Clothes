#ifndef _UC
#define _UC
#include "KinectUser.h"
#include "Garment.h"
#include "Outfit.h"
#include <list>

class UserController
{
	std::list<Garment> HelmetList;
	std::list<Outfit> OutfitList;
	public:
		void drawUser(KinectUser*);
		void nextHelmet(KinectUser*);
		void nextOutfit(KinectUser*);
};

#endif