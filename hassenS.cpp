//Author: Hassen Seid
//2016,
//Roles and Resp
// - Texture Maps 
// - Tutorial is going to be one of the options that appear in the main menu which teaches the player how to play the game
// - Main menu is the starting point of the game that appears with a row of options
// - ScoreBoard is to keep track of how many points the player has
// - Ball life is how many trys a player get before losing the game completely

#include "hassenS.h"
#include "omarO.h"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "ppm.h"
#include <iostream>
#include <math.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
extern "C" {
#include "fonts.h"
}

using namespace std;

extern Ppmimage *canonImage;
extern GLuint canonTexture;
extern score Scorekeeper;
extern Ppmimage *OceanImage;
extern Ppmimage *pinballImage;
extern Ppmimage *flippers;
extern Ppmimage *flippers2;
extern Ppmimage *MainMenu;
extern GLuint OceanTexture;
extern GLuint MainMenuTexture;
extern GLuint flippersTexture;
extern GLuint flippersTexture2;
extern GLuint pinballTexture;
extern Ball ball1;
extern bool cannonFired;
extern bool launch;
extern bool gameNotOver;
extern bool boom;
extern bool MainMenuOn;
extern int xres;
extern int yres;
extern Cannon cannon;
extern Sounds gameSounds;
#define FLIPPER_LENGTH 70.0
#define FLIPPER_HEIGHT 15.0

void showMainMenu() 
{
    
    Rectangle screen;
    screen.width = (double)yres / 2.0;
    screen.height = (double)xres / 2.0;
    screen.pos[0] = (double)xres / 2.0;
    screen.pos[1] = (double)yres / 2.0;
    screen.angle = 90;
    glColor4d(1.0,1.0,1.0,1.0);
    drawRectangleTextureAlpha(screen, MainMenuTexture);
   
}
void gameOver(GameBoard &gb, Ball &ball1, score &s)
{
    if(ball1.pos[1] < 0.0 + ball1.radius) {
								gameSounds.playSound((char *)"splash\0");
        ball1.pos[0] = gb.starting_point[0];
        ball1.pos[1] = gb.starting_point[1];
        s.balls_left--;
        ball1.inPlay = 0;
        cannonFired = 0;
        launch = false;
        boom = false;
        ball1.vel[0] = 0;
        ball1.vel[1] = 0;
        cannon.active = 1;
    }
    if(s.balls_left == 0) {
        gameNotOver = false;
    }
}

void addScore(score *s, int add)
{
    //Adding score points after collision
    s->points = s->points + add;
}

void risingScore(score *s)
{
    //rising points increments until it reaches the same amount as score
    if (s->points > s->rising_points) {
        s->rising_points += 30;
    }
    else 
        //if rising matches score don't increment anymore
        s->rising_points = s->points;
}

void initScore(score *s)
{
    //starting point of player score and ball life
    s->points = 0;
    s->balls_left = 4;
}

void drawScore()
{
    //position of the display 
    Rect re;
    re.bot = yres - 20;
    re.left = 10;
    re.center = 0;
    //Display Scoreboard in red
    risingScore(&Scorekeeper);
    ggprint16(&re, 22, 0x00ff0000, "Balls Left: %i", Scorekeeper.balls_left); 
    //displaying the points in increments of 10
    ggprint16(&re, 22, 0x00ff0000, "Score: %i", Scorekeeper.rising_points);
}

//Transparent pictures
void alphaTextureInit(char *fileTexture, GLuint &textureID, Ppmimage *fileT)
{
    //load the images 
    fileT = ppm6GetImage(fileTexture);

    glGenTextures(1, &textureID);

    int w = fileT->width;
    int h = fileT->height;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //makes image transparent 
    unsigned char * alphaData = buildAlphaData(fileT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, alphaData);
    free(alphaData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//from texture framework
void textureInit(char *fileTexture, GLuint &textureID, Ppmimage *fileT)
{

    fileT = ppm6GetImage(fileTexture);

    glGenTextures(1, &textureID);
    int w = fileT->width;
    int h = fileT->height;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //non transparent image 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, fileT->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


