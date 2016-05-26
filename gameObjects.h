//Author: Hassen Seid, Omar Oseguera, Alex Rinaldi
//Purpose - Contains resources that can be used by everyone (and don't really belong to anyone)

//Description
//Includes struct definitions for all the different objects in the game (flipper, bumper)
//Also includes useful functions like drawing debug rectangles and time/counter management


#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"
#include "time.h"
#include "ppm.h"
//#include "alexR.h"
#include <GL/glx.h>

#define MAX_RECTANGLES 900
#define MAX_PARTICLES 800
#define MAX_CANNONS 10

//Collision objects
struct Rectangle {
    Vec pos;
    bool collide[4]; //up, down, left, right
    double width, height, angle;

    Rectangle() { for (int i = 0; i < 4; i++) collide[i] = true; }
};
struct Shape {
    float width, height;
    float radius;
    Vec center;
};
struct Particle {
    Shape s;
    Vec velocity;
};
struct Water {
    bool waterflow;
    Vec pos;
    int n;
    Particle particle[MAX_PARTICLES];
    Water() {
        waterflow = true;//may not need this
    }
};
struct Smoke {
    Rectangle r;
    int frame;
    timespec frame_timer;
    int state; //1 = active, 0 = not
};
struct Flag {
	Rectangle r;
	int flagFrame;
    int state; //1 = waving, 0 = still
	timespec flagFrameTimer;

    Flag() 
    {
        flagFrame = 0;
        state = 0;
    }
};
struct Circle {
    Vec pos;
    double radius;

};

//Smacks the ball when you press a button
struct Flipper {
    Vec pos;
    double width, height, angle;
    double rvel;
    //direction inverted
    bool inverted;
    //rotational velocity; gets added to angle every frame
    int flipstate;
    //0 - idle, 1 - going up, 2 - going down
};


// A curve that is built from small rectangles
struct Curve {
    double width;
    bool collide[2]; //above, below
    int npoints;
    Vec points[3];

    Curve() { for (int i = 0; i < 4; i++) collide[i] = true; }
};

//The main character of our story
struct Ball {
    Vec pos;
    Vec vel;
    double radius;
    double mass;
    int inPlay; //1 if ball has left launch chute
    int isVisible; // 0 is hidden, 1 is showing
    int hasGravity;
    
    Ball() {
        inPlay = 0;
        isVisible = 1;
        hasGravity = 1;
    }
};

//A circular object that bounces the ball away
struct Bumper {
    Circle c;
    int state; // 0 up, 1 down
};

//Opens and awards points after it is hit multiple times
struct TreasureChest {
    int state; //0 is closed, 1 is open
    int active; // 0 if recently collided, 1 if not
    Rectangle r;
    int HP; // 3, 2, 1
    timespec collision_time;
    TreasureChest() {
        active = 1;
        state = 0;
        HP = 3;
    }
};

//Shoots the ball
struct Cannon {
    timespec timer;
    Vec direction;
    Vec resting_pos;
    Rectangle r;
    Circle collision_circle;
    Smoke smoke;
    int active;//use to launch canon
    int firing; // if firing
    int loaded;
};

//A rope that flicks out the ball 
struct Deflector {
    Rectangle r;
    int state; // active = 1
    int direction; // left = 0, right = 1
    timespec collision_time;
    Deflector() {
        state = 0;
        direction = 0;
    }
};

//Does a little spin when the ball moves over it
struct SteeringWheel {
    Vec pos;
    double inner_radius, outer_radius;
    double angle, rvel;
    int state; // 0 is not spinning, 1 is spinning
    timespec collision_time; //keeps track of how long the ball spins
    SteeringWheel() {
        state = 0;
        angle = 0;
        rvel = 0;
    }
};

struct SeaMonster {
    Rectangle rectangle;
    int state; //0 - hiding, 1 - active, 2 - damaged
    int HP;

    timespec active_time;
    
    Vec hiding_pos, active_pos, dead_pos;
    
    Circle collision_circle;

    Smoke bloodspurts[50]; // such violence!
    int num_bloodspurts;

    SeaMonster() 
    {
        HP = 3;
        num_bloodspurts = 0;
    }
};


struct Whirlpool {
    int state;
    Vec pos;
    double inner_radius, outer_radius;
};

//Holds all of the repeating objects in the game:
//collision rectangles, bumpers, etc.
struct GameBoard {
    Rectangle rectangles[MAX_RECTANGLES];
    Bumper bumpers[MAX_RECTANGLES];
    Deflector deflectors[MAX_RECTANGLES];
    Cannon cannons[MAX_CANNONS];
    int num_rectangles;
    int num_bumpers;
    int num_deflectors;
    int num_cannons;
    Vec center;

    Vec starting_point;

    GameBoard() {
        num_rectangles = 0;
        num_bumpers = 0;
        num_deflectors = 0;
        num_cannons = 0;
    }
};

//functions
unsigned char *buildAlphaData(Ppmimage *);
//used for timing
double timeDiff(struct timespec *, struct timespec *);
void timeCopy(struct timespec *, struct timespec *);

//used to set default values 
void initBumpers(GameBoard &);
void initSteeringWheel(SteeringWheel &);

//simple filled shapes, good for debugging
void drawCircle(Circle &);
void drawRectangle(Rectangle &);
void drawRectangleTextureAlpha(Rectangle &, GLuint &);

//adds a bumper to the board
void addBumperToBoard(Bumper &, GameBoard &);
void addRectangleToBoard(Rectangle &, GameBoard &);
#endif
