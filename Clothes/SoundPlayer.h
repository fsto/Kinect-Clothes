#ifndef _SOUNDPLAYER
#define _SOUNDPLAYER
#include <vector>
#include <ctime>

#define SOUNDOBJECT_ID_SIZE 15

struct SoundObject
{
	char id[SOUNDOBJECT_ID_SIZE];
	unsigned int length;
	SoundObject() : length(0){}
};


class SoundPlayer
{
	time_t nextPlayback;
	std::vector<SoundObject*> sounds;
public:
	SoundPlayer(char *path);
	void playSound(unsigned int);
	int count(){return sounds.size();}
};

#endif