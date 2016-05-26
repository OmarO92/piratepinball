//Author - Hassen Seid
//May 02, 2015
#ifndef _HSEID_H_
#define _HSEID_h_
#include <GL/glx.h>
#include "ppm.h"
#include <cmath>
#include <iostream>
#include "gameObjects.h"
//#include "fonts.h"


struct score {
    int points;
    int rising_points;
    int balls_left;

};

void risingScore(score *);
//Transparent Pic
void alphaTextureInit(char *, GLuint &, Ppmimage *);
//Tex pic
void textureInit(char *, GLuint &, Ppmimage *);
//adding score
void addScore(score *, int);
//score counter
void initScore(score *);
//display score
void drawScore();
//another try
void gameOver(GameBoard &, Ball &, score &);

void showMainMenu();
#endif
