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

#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include "omarO.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <GL/glx.h>
#include "hassenS.h"
#define MAX_BUMPERS 20

extern score Scorekeeper;
extern double xres, yres;
extern struct timespec timeCurrent;
extern Flipper flipper, flipper2;
extern GameBoard board;
extern Sounds gameSounds;

/* This adds a curve to the game board */
/* It uses beizer curves to rectangles at a set number of steps */
void addCurve(Curve &c, GameBoard &g)
{
    //uses beizer curves
    double t = 0.0;
    double tStep = 1.0 / c.npoints;
    double x0, y0, x1, y1;
    Vec point1, point2, btn, horz, zero;
    MakeVector(1, 0, 0, horz);
    MakeVector(0, 0, 0, zero);

    for (int i = 0; i <= c.npoints; i++) {

        x0 = (1.0 - t) * (1.0 - t) * c.points[0][0] +
            2.0 * (1.0 - t) * t * c.points[1][0] + 
            t * t * c.points[2][0];

        y0 = (1.0 - t) * (1.0 - t) * c.points[0][1] +
            2.0 * (1.0 - t) * t * c.points[1][1] + 
            t * t * c.points[2][1];

        if (i > 0) {
            //for each segment of the curve, draw a rectangle
            Rectangle *r = &(g.rectangles[g.num_rectangles]);
            MakeVector(x0, y0, 0, point1);
            MakeVector(x1, y1, 0, point2);
            VecBtn(point1, point2, btn);
            r->pos[0] = (x0 + x1) / 2.0;
            r->pos[1] = (y0 + y1) / 2.0;

            r->collide[0] = c.collide[0];
            r->collide[1] = c.collide[1];
            r->collide[2] = false;
            r->collide[3] = false;

            //rotate the rectangle based on the angle its 
            //horizontal edge and the x-axis
            r->angle = (isLeft(zero, horz, btn) ? -1 : 1) * 
                VecAngleBtn(horz, btn);
            r->width = VecMagnitude(btn) / 2.0;
            r->width += r->width * .05;
            r->height = c.width;
            g.num_rectangles++;
        }
        x1 = x0;
        y1 = y0;

        t += tStep;

    }
}

/* Vector function for testing if a point is on either side of a line */
bool isLeft(Vec &a, Vec &b, Vec &p)
{
    Vec v1, v2;
    VecBtn(a, b, v1);
    VecBtn(p, b, v2);
    double xp = v1[0] * v2[1] - v1[1] * v2[0];
    return xp > 0;
}


/* Given a position and an angle, get coordinates of rectangle corners */
void getRectangleCorners(Rectangle &r,
        Vec &corner1, Vec &corner2, Vec &corner3, Vec &corner4)
{
    Vec vert, horz, neg_vert, neg_horz;
    MakeVector(0,1,0,vert);
    MakeVector(1,0,0, horz);

    //unit vectors rotated
    VecRotate(vert, r.angle, vert);
    VecRotate(horz, r.angle, horz);

    //scaled to rectangle width and height
    VecScale(vert, r.height, vert);
    VecScale(horz, r.width, horz);
    VecScale(vert, -1, neg_vert);
    VecScale(horz, -1, neg_horz);

    //get rectangle relative corners
    VecAdd(neg_horz, vert, corner1);
    VecAdd(horz, vert, corner2);
    VecAdd(horz, neg_vert, corner3);
    VecAdd(neg_horz, neg_vert, corner4);

    //shift by rectangle position
    VecAdd(corner1, r.pos, corner1);
    VecAdd(corner2, r.pos, corner2);
    VecAdd(corner3, r.pos, corner3);
    VecAdd(corner4, r.pos, corner4);
}

/* Handles the physics for a circle collding with a bumper */
int bumperBallCollision(Bumper &b, Ball &ba)
{
    Vec between, dV;

    VecBtn(b.c.pos, ba.pos, between);

    if (VecMagnitude(between) < b.c.radius + ba.radius) {
        VecNormalize(between, dV);
        VecScale(dV, 12, ba.vel);
        b.state = 1;
        addScore(&Scorekeeper,30); 
        return 1;
    }
    else {
        b.state = 0;
        return 0;
    }
}

//---------steering wheel functions---------------//
void initSteeringWheel(SteeringWheel &wheel)
{
    wheel.pos[0] = board.center[0];
    wheel.pos[1] = board.center[1];
    wheel.inner_radius = 7;
    wheel.outer_radius = 70;
}

void steeringWheelMovement(SteeringWheel &wheel)
{
    wheel.angle += fmod(wheel.rvel, 360.0);
    if (wheel.rvel >= 0.0) {
        wheel.rvel -= 0.01 * wheel.rvel;
    }
    if (wheel.rvel < 0) {
        wheel.rvel = 0.0;
    }
}

bool insideCircle(double radius, Vec &center, Ball &ball) 
{
    Vec between;
    VecBtn(center, ball.pos, between);

    //if inside inner radius
    return (VecMagnitude(between) < ball.radius + radius);


}

int steeringWheelBallCollision(SteeringWheel &wheel, Ball &ball)
{
    if (insideCircle(wheel.inner_radius, wheel.pos, ball)) {
        gameSounds.playSound((char *)"wheel\0");
        wheel.rvel = (VecMagnitude(ball.vel) / MAX_VELOCITY) * 10.0;
        addScore(&Scorekeeper, 50);
        return 1;
    }

    return 0;
}

void rectangleSurfaceNormals(Rectangle &r, Vec &vert, Vec &horz)
{
    MakeVector(0, 1, 0, vert);
    MakeVector(1, 0, 0, horz);
    //rotated
    VecRotate(vert, r.angle, vert);
    VecRotate(horz, r.angle, horz);
}

//collisionw with the deflectors...a simplified version of rectangle ball collision
int deflectorBallCollision(Deflector &d, Ball &b)
{
    //unit axis vectors
    Vec vert, horz, between;
    rectangleSurfaceNormals(d.r, vert, horz);
    VecBtn(d.r.pos, b.pos, between);
    double projectX = VecProject(between, horz);
    double projectY = VecProject(between, vert);

    //check collision
    if (projectX > -(d.r.width + b.radius) && projectX < d.r.width + b.radius
            && projectY > -(b.radius + d.r.height) 
            && projectY < b.radius + d.r.height) {
        gameSounds.playSound((char *)"rope\0");
        //move ball in direction of surface normal
        VecScale(vert, 15.0, vert);
        VecAdd(vert, b.vel, b.vel);

        timeCopy(&d.collision_time, &timeCurrent);
        d.state = 1;

        //set state to 1
        d.state = 1;

        return 1;
    }

    //if a small time period has passed since the collision
    if (d.state == 1 && 
            timeDiff(&d.collision_time, &timeCurrent) > 0.2) {
        d.state = 0;
    }

    return 0;
}

bool insideRectangle(Rectangle &r, Ball &b)
{
    Vec zero;
    MakeVector(0, 0, 0, zero);

    //unit axis vectors
    Vec vert, horz;
    rectangleSurfaceNormals(r, vert, horz);

    Vec between;
    VecBtn(r.pos, b.pos, between);
    double projectX = VecProject(between, horz);
    double projectY = VecProject(between, vert);

    //check collision
    return (projectX > -(r.width + b.radius) && projectX < r.width + b.radius
            && projectY > -(b.radius + r.height) 
            && projectY < b.radius + r.height); 
}

/* Handles the physics for a circle colliding with a rectangle */
int rectangleBallCollision(Rectangle &r, Ball &b)
{
    Vec zero;
    MakeVector(0, 0, 0, zero);

    Vec corner[4];

    //float angle = r.angle;
    //unit axis vectors
    Vec vert, horz, gravity;
    rectangleSurfaceNormals(r, vert, horz);
    /*MakeVector(0, 1, 0, vert);
      MakeVector(1, 0, 0, horz);
     */
    MakeVector(0, -1, 0, gravity);

    //rotated
    //VecRotate(vert, angle, vert);
    //VecRotate(horz, angle, horz);

    Vec between, dV, rNorm;
    VecBtn(r.pos, b.pos, between);
    double currentSpeed = VecMagnitude(b.vel);
    //double projectX = VecProject(between, horz);
    //double projectY = VecProject(between, vert);

    //check collision
    if (insideRectangle(r, b)) {

        getRectangleCorners(r, corner[0], corner[1], corner[2], corner[3]);

        //Figure out which edge the ball collided with
        bool lDiagonal = isLeft(corner[0], corner[2], b.pos);
        bool rDiagonal = isLeft(corner[3], corner[1], b.pos);

        if (lDiagonal && rDiagonal) {
            if (!r.collide[1])
                return 0;

            //bottom edge
            VecScale(vert, -1, rNorm);
        }
        else if (lDiagonal && !rDiagonal) {
            if (!r.collide[2])
                return 0;

            //left edge
            VecScale(horz, -1, rNorm);
        }
        else if (!lDiagonal && rDiagonal) {
            if (!r.collide[3])
                return 0;

            //right edge
            VecScale(horz, 1, rNorm);
        }
        else {
            if (!r.collide[0])
                return 0;

            //top edge
            VecScale(vert, 1, rNorm);

        }

        Vec dP;
        //project the ball's position onto the collided surface's normal
        double projectNorm = VecProject(between, rNorm);

        //scale the rNorm to the distance the ball is inside the rect.
        VecScale(rNorm, 
                std::abs(b.radius - std::abs(projectNorm - r.height)), 
                dP);

        //if the distance is small enough, shift the ball's posion
        if (VecMagnitude(dP) <= b.radius) {
            VecAdd(b.pos, dP, b.pos);
        }

        VecNormalize(b.vel, dV);
        VecScale(dV, -1, dV);
        //get incoming velocity angle and get whether is positive
        double angBtn = VecAngleBtn(rNorm, dV);
        bool posAng = isLeft(zero, rNorm, dV);

        //get the reflection vector by rotating it
        VecRotate(dV,(posAng ? 2 : -2) * angBtn, dV);
        VecScale(dV, 0.6 * currentSpeed, b.vel);

        return 1;
    }

    return 0;
}

void flipperBallCollision(Flipper &f, Ball &b)
{
    float angle = f.inverted ? -f.angle : f.angle;
    //unit axis vectors
    Vec vert, horz;
    MakeVector(0, 1, 0, vert);
    MakeVector(1, 0, 0, horz);
    if (f.inverted) {
        VecScale(horz, -1, horz);
    }

    //rotated
    VecRotate(vert, angle, vert);
    VecRotate(horz, angle, horz);

    Vec between;
    VecBtn(f.pos, b.pos, between);
    double projectX = VecProject(between, horz);
    double projectY = VecProject(between, vert);

    //check collision
    if (projectX > 0 && projectX < f.width + b.radius
            && projectY > -(b.radius + f.height) && projectY < b.radius) {

        //adjust position
        Vec dP;
        VecScale(vert, b.radius - projectY, dP);
        VecAdd(b.pos, dP,b.pos);

        Vec dV;
        double speed;

        //speed is based on the incoming velocity
        //the horizontal distance from the pivot point of the flipper (torque)
        //and the speed at which the flipper is rotating 
        speed = 0.5 * VecMagnitude(b.vel) + 
            pow((projectX / f.width) + 0.5, 2) * (f.rvel / 10);
        VecScale(vert, speed, dV);
        VecAdd(b.vel, dV, b.vel);

        //reflection vector
        VecRotate(dV, VecAngleBtn(between, vert), dV); 

        //add horizontal velocity if at the tip of flipper
        if (projectX / f.width > 0.20) {
            VecScale(horz, speed * 0.2 * projectX / f.width, dV);
            VecAdd(b.vel, dV, b.vel);
        }
    }
}

/* Scales down the ball's velocity if it is greater than maximum */
void applyMaximumVelocity(Ball &b)
{
    if (VecMagnitude(b.vel) > MAX_VELOCITY) {
        VecNormalize(b.vel, b.vel);
        VecScale(b.vel, MAX_VELOCITY, b.vel);
    }
}


