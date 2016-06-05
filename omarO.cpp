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
#include <fstream>
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
extern timespec timeCurrent;
extern score Scorekeeper;
extern GameBoard board;
extern int xres, yres;
extern bool gameNotOver;
extern bool pauseGame;
extern Cannon cannon;
extern Smoke smoke;
extern Cannon boardCannon;
extern Ball ball1;
extern bool boom;
extern bool launch;
extern Sounds gameSounds;
extern int cannonFired;
extern string playerName;
extern Bird birds; 
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

//initialize openAL
int Sounds::initOpenAL()
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		cout << "ERROR: alutInit()\n";
		return 0;
	}
	else return 1;

}

//create buffers for sound Class
int Sounds::createBuffers()
{
	alGenBuffers(BUFFERS, buffers);
	if (alGetError() != AL_NO_ERROR) {
		cout << "alGenBuffers error\n";
		return 0;
	}
	else return 1;
}

//load all sounds into buffer
void Sounds::loadSounds()
{
	for (int i = 0; i < NUM_WAVS; i++) {
		buffers[i] = alutCreateBufferFromFile(wavFiles[i]);
	}
}

//play sound based on programmer input of string
void Sounds::playSound(char *name)
{
	for (int i = 0; i < NUM_WAVS; i++) {
		if (strcmp(name, soundNames[i]) == 0) {
			alSourcePlay(source[i]); //this takes ALuint
		}	
	}
}

//generate all sources of Sound class
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

//set up listeners for Sound class
void Sounds::listener()
{
	//Setup the listener.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
}

//clean up sound data 
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

//checks for collision with treasure chest
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

//animates flag sprites in game
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

//initialize cannon properties
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

//initialize chest properties
void initChest(TreasureChest &chest)
{
	Rectangle *rec = &chest.r;
	rec->pos[0] = board.center[0];
	rec->pos[1] = yres - 100;
	rec->width = 40.0;
	rec->height = 40.0;
	rec->angle = 0.0;
}
//function loads structure array properties 
//for sound from a txt file (testing)
void loadSoundProperties(soundProperties &p, char *filename)
{
	ALuint sources[100];
	//ALuint bufferArr[100];
	int i = 0;
	char *fName = NULL;
	char *uName = NULL;
	bool loop = true;
	float gain = 0.0;
	float pitch = 0.0;

	ifstream load(filename);
	if (load.is_open()) {

		while (!load.eof()) {

			load >> fName;
			load >> uName;
			load >> loop;
			load >> gain;
			load >> pitch;

			if (load.good()) {

				strcpy(p.wavName[i], fName);
				//buffArr[i] = alutCreateBufferFromFile(p.wavName[i]);
				strcpy(p.username[i], uName);
				p.looper[i] = loop;

				if (p.looper[i] == true) {
					alSourcei(sources[i], AL_LOOPING, AL_TRUE);	
				}

				else {
					alSourcei(sources[i], AL_LOOPING, AL_FALSE);
				}

				alSourcef(sources[i], AL_GAIN, p.gain[i]);
				alSourcef(sources[i], AL_PITCH, p.pitch[i]);
				i++;
			}
		}
		load.close();
	}
}

//default constructor initializes bird class properties
Bird::Bird()
{
		_frame = 0;
	strcpy(birdImages[0], "bird1.jpg\0");
	strcpy(birdImages[1], "bird2.jpg\0");
	strcpy(birdImages[2], "bird3.jpg\0");
	strcpy(birdImages[3], "bird4.jpg\0");
	strcpy(birdImages[4], "bird5.jpg\0");
	strcpy(birdImages[5], "bird6.jpg\0");
	strcpy(birdImages[6], "bird7.jpg\0");
	strcpy(birdImages[7], "bird8.jpg\0");
	strcpy(birdImages[8], "bird9.jpg\0");
	strcpy(birdImages[9], "bird10.jpg\0");
}

//Bird class desctructor
Bird::~Bird()
{
	for (int i = 0; i < 10; i++) {
		strcpy(filename, birdImages[i]);
		char *period = strchr(filename, '.');
		*period = '\0';   
		sprintf(syscall_buffer, "images/%s.ppm", filename);
		unlink(syscall_buffer);
	}
}

//converts all jpgs of birds to ppms 
void Bird::convert_to_ppm()
{
	for (int i = 0; i < 10; i++) {

		strcpy(filename, birdImages[i]);
		char *period = strchr(filename, '.');
		*period = '\0';
		sprintf(syscall_buffer, "convert ./images/%s ./images/%s.ppm",
				birdImages[i], filename);
		system(syscall_buffer);
	}
}

//generate textures and ppm images for bird animation
void Bird::generateTexturesandPpms()
{
	strcpy(_buffer, "./images/bird1.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[0], birdSprites[0]);

	strcpy(_buffer, "./images/bird2.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[1], birdSprites[1]);	

	strcpy(_buffer, "./images/bird3.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[2], birdSprites[2]);

	strcpy(_buffer, "./images/bird4.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[3], birdSprites[3]);

	strcpy(_buffer, "./images/bird5.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[4], birdSprites[4]);

	strcpy(_buffer, "./images/bird6.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[5], birdSprites[5]);

	strcpy(_buffer, "./images/bird7.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[6], birdSprites[6]);

	strcpy(_buffer, "./images/bird8.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[7], birdSprites[7]);

	strcpy(_buffer, "./images/bird9.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[8], birdSprites[8]);

	strcpy(_buffer, "./images/bird10.ppm");
	alphaTextureInit(_buffer, birdSpriteTexture[9], birdSprites[9]);
}

//this function animates bird on screen with KeyPress G
void Bird::displayBird(timespec timeCurrent)
{
		if (_frame < 10) {
			cout << _frame << " \n";//prints frame # to console for debugging
			//glPushMatrix();
			glColor3d(1.0, 1.0, 1.0);
			_border.pos[0] = 440.0;//xpos
			_border.pos[1] = 220.0;//ypos
			_border.width = 60.0;
			_border.height = 60.0;
			_border.angle = 50.0;
			drawRectangleTextureAlpha(_border, 
			birdSpriteTexture[_frame]);
			//glPopMatrix();
		if (timeDiff(&_frameTimer, &timeCurrent) > 1.0/20.0) {
				timeCopy(&_frameTimer, &timeCurrent);
				_frame++;
		}
	}
	else {
		_frame = 0;
		_border.pos[0] += 50.0;
		_border.pos[1] += 50.0;
	}
}

//at end of game, scores are written to HTML web page.
void writeScoreToFile()
{
	ofstream out("highsore.html");
	if (out.is_open()) {
		out << "<!DOCTYPE html>" << endl;
		out << "<html>" << endl;
		out << "<head>" << endl;
		out << "<title> PIRATE PINBALL </title>" << endl;
		out << "</head>" << endl;
		out << "<body>" << endl;
		out << "<h3 style=\"color:#FF0D00; position:center\">" 
			<< "HIGH SCORE: </h3>" << endl;
		out << "<ul>" << endl;
		out << "<li> " << playerName << " " 
			<< Scorekeeper.points << endl;
		out << "</li>" << endl;
		out << "</ul>" << endl; 
		out << "</body>" << endl;
		out << "</html>" << endl; 
	}	
	out.close();
}

//function meets keypress class requirement.
//launches cannon
void launchCannonOnKeyPress()
{
	//fire main launcher
	if (gameNotOver && !pauseGame && cannon.active) {
		//start frame timer
		timeCopy(&smoke.frame_timer, &timeCurrent);
		cannon.firing = 1;
		boom = true;
		launch = true;
		gameSounds.playSound((char *)"cannon\0");
		if (cannonFired < 1) {
			ball1.vel[1] = 20.0;
			cannonFired++;
		}
	}
	//fire secondary cannon
	else if (boardCannon.loaded && !boardCannon.firing) {
		gameSounds.playSound((char *)"cannon\0");
		fireCannon(boardCannon, ball1);
	}
}
