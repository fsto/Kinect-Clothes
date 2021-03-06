#include "SoundPlayer.h"
#include <windows.h>
#include <mmsystem.h>
#include <fstream>

void SoundPlayer::playSound(unsigned int idx)
{
	idx--;
	if(idx < sounds.size() || idx < 0)
	{
		if(difftime(time(NULL), nextPlayback) > 0)
		{
			PlaySound(sounds[idx]->id, NULL, SND_FILENAME | SND_ASYNC);
			nextPlayback = time(NULL) + sounds[idx]->length;
			#ifdef _DEBUG
			printf("Playing \"%s\"\n", sounds[idx]->id);
			#endif
		}
		else
		{
			#ifdef _DEBUG
			printf("Cannot play sound yet, still playing the last one...\n");
			#endif
		}
	}
	else
	{
		#ifdef _DEBUG
		printf("Sound %d out of range\n", idx);
		#endif
	}
}

SoundPlayer::SoundPlayer(char *path)
	: nextPlayback(0)
{
	std::ifstream file(path);
	int count;
	file >> count;
	printf("Found %d sounds\n", count);

	for(int i=0;i<count;++i)
	{
		SoundObject *o = new SoundObject();
		unsigned int length;
		file >> length;
		o->length = length+1;
		_snprintf_s(o->id, SOUNDOBJECT_ID_SIZE, "audio/%d.wav", i+1);
		sounds.push_back(o);
	}
}
SoundPlayer::~SoundPlayer()
{
	for(unsigned int i=0; i<sounds.size(); ++i)
	{
		delete sounds[i];
	}
}