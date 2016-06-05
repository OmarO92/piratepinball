//Author: Omar Oseguera
//see CPP file for roles description
#ifndef _OMAR_O_
#define _OMAR_O_

#include <iostream>
#include <string.h>
#include <cstring>
#include <math.h>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include "gameObjects.h"
#include <GL/glx.h>
#include <sys/stat.h>
#include "ppm.h"
#include </usr/include/AL/alut.h>
using namespace std;

const int SIZE = 100;
const int SMOKE_SPRITES = 12;
const int FLAG_SPRITES = 6;
const int BUFFERS = 9;
const int NUM_WAVS = 9;
const int NUM_SOURCES = 9;
extern GLuint smokeSpriteTexture[];
extern GLuint flagSpriteTexture[];

struct soundProperties {
	char wavName[NUM_WAVS][SIZE];
	char username [NUM_WAVS][SIZE];
	bool looper[SIZE];
	float gain[SIZE];
	float pitch[SIZE];
};
class Bird {
	private:
	Ppmimage *birdSprites[10];
	GLuint birdSpriteTexture[10];
	char syscall_buffer[256];
	char filename[256];
	char birdImages[10][100];
	int _frame;
	public:
	Bird();
	void displayBird();
	void convert_to_ppm();
};

class Sounds {
	private:
		ALuint buffers[BUFFERS];
		ALuint source[NUM_SOURCES];
		char wavFiles[NUM_WAVS][100];
		char soundNames[NUM_WAVS][100];
		int sound_number;
	public:
		Sounds();
		int initOpenAL();
		int createBuffers();
		void loadSounds();
		void cleanUpSound();
		void playSound(char *);
		int generateSource();
		void listener();
};
void initFlag(Flag &);
//initSmoke sets properties for smoke sprites
void initSmoke(Smoke &);
void smokeAnimation(Smoke &, timespec );
void flagAnimation(Flag &, timespec);
//initChest initializes treasure chest object properties
void initChest(TreasureChest &);
//ballChestCollision plays sound when ball collides with chest
int ballChestCollision(TreasureChest &, Ball &);
//initCanon initializes canon object properties
void initCannon(Cannon & );
void initLauncher(Cannon &);
void loadSoundProperties(soundProperties &, char *);
void writeScoreToFile();
void launchCannonOnKeyPress();
#endif
