#include "SoundPlayer.h"
#include <windows.h>
#include <mmsystem.h>
#include <fstream>

void SoundPlayer::playSound(unsigned int idx)
{
	if(idx < sounds.size())
	{
		if(difftime(time(NULL), nextPlayback) > 0)
		{
			PlaySound(sounds[idx]->id, NULL, SND_FILENAME | SND_ASYNC);
			nextPlayback = time(NULL) + sounds[idx]->length;
			printf("Playing \"%s\"\n", sounds[idx]->id);
			printf("Next playback at %d\n", nextPlayback);
		}
		else
		{
			printf("Cannot play sound yet, still playing the last one...\n");
		}
	}
	else
		printf("Sound %d out of range\n", idx);
}

SoundPlayer::SoundPlayer(char *path)
	: nextPlayback(0)
{
	std::fstream file(path);
	int count;
	file >> count;
	printf("Found %d sounds\n", count);

	for(int i=0;i<count;++i)
	{
		SoundObject *o = new SoundObject();
		unsigned int length;
		file >> length;
		o->length = length;
		_snprintf_s(o->id, SOUNDOBJECT_ID_SIZE, "audio/%d.wav", i+1);
		sounds.push_back(o);
		printf("Length %d - %d\n", o->length, length);
	}
}