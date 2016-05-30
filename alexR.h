
// Author: Alex Rinaldi 
// CS335 Project 

// Purpose 
// Provide functions to detect and handle collision between the ball and various objects, 
// along with functions to add more complex objects to the game board.


/* Roles for this project:
 * Collision detection
 * Physics and movement 
 * Special effects 
 */


#ifndef _ALEX_R_H_
#define _ALEX_R_H_

#include "vector.h"
#include "gameObjects.h"
#include <iostream>
#include <cmath>

const double monsterGutter = 90.0;
const double  MAX_VELOCITY = 25.0;
const double FLIPPER_SPEED = 14.8;
const double FLIPPER_REST_ANGLE = -50;

bool isLeft(Vec &, Vec &, Vec &);
void addCurve(Curve &, GameBoard &);
void rectangleSurfaceNormals(Rectangle &, Vec &, Vec &);

void steeringWheelMovement(SteeringWheel &);

bool insideCircle(double, Vec &, Ball &);
bool insideRectangle(Rectangle&, Ball &);
int rectangleBallCollision(Rectangle &, Ball &);
int deflectorBallCollision(Deflector &, Ball &);
int bumperBallCollision(Bumper &, Ball &);
int steeringWheelBallCollision(SteeringWheel &, Ball &);
void flipperBallCollision(Flipper &, Ball &);
void flipperMovement(Flipper &e);
void applyMaximumVelocity(Ball &);

void fireCannon(Cannon &, Ball &);
void cannonPhysics(Cannon &cannon, Ball &ball);

void seaMonsterPhysics(SeaMonster &, Ball &);
void killSeaMonster(SeaMonster &);
#endif

