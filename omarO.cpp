//Author: Omar Oseguera
//Roles and Responsibilites: 
// - This file contains code for sound
// - coding for sprite animation/effects 
#include <iostream>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "omarO.h"
#include "gameObjects.h"
#include "alexR.h"
#include "ppm.h"
#include <sys/stat.h>
#include <GL/glx.h>
#include </usr/include/AL/alut.h>
#include "hassenS.h"
extern "C" {
#include "fonts.h"
}
using namespace std;
extern score Scorekeeper;
extern GameBoard board;
extern int xres, yres;
/*****SOUNDS CLASS IMPLEMENTATION****
	The Sound class loads all wav file data into buffers
	Plays sound according to string input from pinball.cpp
	arrays of buffers and sources allow for 
	multiple sounds playing simultaneously*/
Sounds::Sounds()
{
	//initialize wav file array w/ file names
	strcpy(wavFiles[0], "./sounds/flippers.wav\0");
	strcpy(wavFiles[1], "./sounds/Barrel2.wav\0");
	strcpy(wavFiles[2], "./sounds/soundtrack.wav\0");
	strcpy(wavFiles[3], "./sounds/Cannon.wav\0");
	strcpy(wavFiles[4], "./sounds/chest.wav\0");
	strcpy(wavFiles[5], "./sounds/monster.wav\0");
	strcpy(wavFiles[6], "./sounds/rope.wav\0");
	strcpy(wavFiles[7], "./sounds/splash.wav\0");
	strcpy(wavFiles[8], "./sounds/wheel.wav\0");
	//initialize sound names array to correspond w/ file names
	strcpy(soundNames[0], "flippers\0");
	strcpy(soundNames[1], "barrels\0");
	strcpy(soundNames[2], "soundtrack\0");
	strcpy(soundNames[3], "cannon\0");
	strcpy(soundNames[4], "chest\0");
	strcpy(soundNames[5], "monster\0");
	strcpy(soundNames[6], "rope\0");
	strcpy(soundNames[7], "splash\0");
	strcpy(soundNames[8], "wheel\0");

}
int Sounds::initOpenAL()
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		cout << "ERROR: alutInit()\n";
		return 0;
	}
	else return 1;

}
int Sounds::createBuffers()
{
	alGenBuffers(BUFFERS, buffers);
	if (alGetError() != AL_NO_ERROR) {
		cout << "alGenBuffers error\n";
		return 0;
	}
	else return 1;
}
void Sounds::loadSounds()
{
	for (int i = 0; i < NUM_WAVS; i++) {
		buffers[i] = alutCreateBufferFromFile(wavFiles[i]);
	}
}

void Sounds::playSound(char *name)
{
	for (int i = 0; i < NUM_WAVS; i++) {
		if (strcmp(name, soundNames[i]) == 0) {
			alSourcePlay(source[i]); //this takes ALuint
		}	
	}
}

int Sounds::generateSource()
{
	for (int i = 0; i < NUM_SOURCES; i++) {
		//Generate a source, and store it in a buffer.
		alGenSources(1, &source[i]);
		alSourcei(source[i], AL_BUFFER, buffers[i]);
		//Set volume and pitch to normal, no looping of sound.
		alSourcef(source[i], AL_GAIN, 1.0f);
		alSourcef(source[i], AL_PITCH, 1.0f);
		if (i == 2) {
			alSourcei(source[i], AL_LOOPING, AL_TRUE);//loop soundtrack
		}
		else {
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
		}
		if (alGetError() != AL_NO_ERROR) {
			cout << "ERROR: setting source\n";
			return 0;
		}
	}
	return 1; 
}
void Sounds::listener()
{
	//Setup the listener.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
}
void Sounds::cleanUpSound()
{
	for (int i = 0; i < NUM_SOURCES; i++) {
		alDeleteSources(1, &source[i]);
		alDeleteBuffers(1, &buffers[i]);
	}
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
}
int ballChestCollision(TreasureChest &chest, Ball &b)
{
	if (VecMagnitude(b.vel) > 1) {
		if (chest.HP > 0) {
			chest.HP--;
		}
		if (chest.HP == 0) {
			cout << "OPEN\n";
			chest.state = 1;
			addScore(&Scorekeeper, 1000);
		}
		return 1;
	}
	else
		return 0;
}
//renders a sequence of smoke sprites with loop 
void initFlag(Flag &f)
{
	Rectangle *flagSprite = &f.r;
	flagSprite->pos[0] = monsterGutter + 50.0;
	flagSprite->pos[1] = yres - 175.0;
	flagSprite->width = 50.0;
	flagSprite->height = 70.0;
	flagSprite->angle = 0.0;
}
//initSmoke sets properties for smoke sprites
//all sprites at same position to loop through each frame
void initSmoke(Smoke &s)
{
	Rectangle *smoke_sprite = &s.r;
	smoke_sprite->pos[0] = 460.0;
	smoke_sprite->pos[1] = 150.0;
	smoke_sprite->width = 30.0;
	smoke_sprite->height = 50.0;
	smoke_sprite->angle = 90.0;

}
void flagAnimation(Flag &f, timespec timeCurrent)
{ 
	if (f.flagFrame < FLAG_SPRITES * 2) {
		glPushMatrix();
		glColor3d(1.0, 1.0, 1.0);
		f.r.angle = 90;
		drawRectangleTextureAlpha(f.r, 
				flagSpriteTexture[f.flagFrame % FLAG_SPRITES]);

		glPopMatrix();

		if (f.state == 1) {
			//if a 20th of a second has passed
			if (timeDiff(&f.flagFrameTimer, &timeCurrent) > 1.0/20.0) {
				//reset the timer
				timeCopy(&f.flagFrameTimer, &timeCurrent);
				//advance to the next frame
				f.flagFrame++;
			}   
		}
	} else {
		f.state = 0;
		f.flagFrame = 0;
	}

}
//renders a sequence of smoke sprites with loop 
void smokeAnimation(Smoke &s, timespec timeCurrent)
{   
	cout << s.frame << " \n";//prints frame # to console for debugging
	if (s.frame < SMOKE_SPRITES) {
		drawRectangleTextureAlpha(s.r, smokeSpriteTexture[s.frame]);
		//if a 20th of a second has passed
		if (timeDiff(&s.frame_timer, &timeCurrent) > 1.0/20.0) {
			//reset the timer
			timeCopy(&s.frame_timer, &timeCurrent);
			//advance to the next frame
			s.frame++;
		} 
	} else {
		s.state = 0;
	}
}

void initLauncher(Cannon &c)
{
	c.active = 1;
	MakeVector(0, 1, 0, c.direction);

	Rectangle *rec = &c.r;
	c.resting_pos[0] = xres - 20.0;
	c.resting_pos[1] = 100.0;
	VecCopy(rec->pos, c.resting_pos);
	rec->width = 40.0;
	rec->height = 40.0;
	rec->angle = 0.0;

	Rectangle *smoke_sprite = &c.smoke.r;
	smoke_sprite->pos[0] = c.resting_pos[0];
	smoke_sprite->pos[1] = 150.0;
	smoke_sprite->width = 30.0;
	smoke_sprite->height = 50.0;
	smoke_sprite->angle = 90.0;
}

void initChest(TreasureChest &chest)
{
	Rectangle *rec = &chest.r;
	rec->pos[0] = board.center[0];
	rec->pos[1] = yres - 100;
	rec->width = 40.0;
	rec->height = 40.0;
	rec->angle = 0.0;
}
