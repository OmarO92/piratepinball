//cs335 Sample
//NOTE: This program contains to flippers for pinball game

//This program demonstrates billiard ball collisions
//
//program: bump.cpp
//author:  Gordon Griesel
//date:    2014
//
//-------------------------------------------
//Press arrow keys to move a ball
//Press S to slow the ball movement
//Grab and move a ball with mouse left button
//-------------------------------------------
//
//
//Depending on your Linux distribution,
//may have to install these packages:
// libx11-dev
// libglew1.6
// libglew1.6-dev
//
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

extern "C" {
#include "fonts.h"
}

#include "ppm.h"
#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include "omarO.h"
#include "hassenS.h"

const int NUM_IMAGES = 48;
//const int SMOKE_SPRITES = 12;
const double CHUTE_THICKNESS = 6.0;
const double CHUTE_WIDTH = 40.0;
const double FLIPPER_LENGTH = 70.0;
const double FLIPPER_HEIGHT = 15.0;
const double FLIPPER_SPEED = 14.8;
const double FLIPPER_REST_ANGLE = -50;

const double CANNON_POS_Y = 100.0;


typedef double Flt;
//typedef Flt Vec[3];
#define MakeVector(x,y,z,v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecNegate(a) (a)[0]=(-(a)[0]); (a)[1]=(-(a)[1]); (a)[2]=(-(a)[2]);
//#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

//X Windows variables
Display *dpy;
Window win;

void initXWindows(void);
void initOpengl(void);
void initBalls(void);
void initCannons(void);
void initGameBoard(GameBoard &);
void initFlipper(Flipper &, float, float, bool);
void initSeaMonster(SeaMonster &);


void initTextures(void);

void initDeflectors(GameBoard &board);
void cleanupXWindows(void);
void checkResize(XEvent *e);
void checkMouse(XEvent *e);
void checkKeys(XEvent *e);
void render(void);

void resetGame(void);

void drawBumper(Bumper &);
void drawCannon(Cannon &);
void drawCannonMovement(Cannon &, Smoke &);
void rotateCannon(Cannon &c, double angle);
void drawChest(TreasureChest &);
void drawFlipper(const Flipper &);
void flagAnimation(Flag &);
void flagPhysics(Flag &, Ball &);
void drawSteeringWheel(SteeringWheel &);
void drawDeflector(Deflector &);
void OceanBackground();
void drawFlipper(Flipper &f);
void drawBall();


void fireCannon(Cannon &, Ball &);
void cannonPhysics(Cannon &cannon, Ball &ball);
void physics(void);
void flipperMovement(Flipper &e);

bool hide = false;

int done=0;
int xres=780, yres=900;
int leftButtonDown=0;
bool boom = false;
bool launch = false;
//
bool pauseGame = false;
bool MainMenuOn;
int cannonFired = 0;

char buffer[256];

Vec tmp;
Vec leftButtonPos;

score Scorekeeper;
bool gameNotOver = true;
GameBoard board;
Curve curve, curve2;

//gameObjects
Ball ball1;
Ball ball2;
Flipper flipper, flipper2;
Rectangle r;
SteeringWheel steeringWheel;
TreasureChest chest;
Cannon cannon;
Cannon boardCannon;
Smoke smoke;
Flag flag;
SeaMonster seaMonster;
Sounds gameSounds;

Ppmimage *OceanImage;

Ppmimage *MainMenuImage;
Ppmimage *gameOverImage;
Ppmimage *controlsImage;
Ppmimage *pinballImage;
Ppmimage *flippers;
Ppmimage *flippers2;
Ppmimage *closeChestImage;
Ppmimage *closeChestImage2;
Ppmimage *closeChestImage3;
Ppmimage *openChestImage;
Ppmimage *bumperUpImage;
Ppmimage *bumperDownImage;
Ppmimage *CannonImage;
Ppmimage *steeringWheelImage;
Ppmimage *ropeDeflectorImage[2];
Ppmimage *smokeSprites[SMOKE_SPRITES];
Ppmimage *flagSprites[FLAG_SPRITES];
Ppmimage *monsterImages[6];
Ppmimage *scoreBoardImage;

char ImageFile[NUM_IMAGES][250] = {
    "flippers.png\0",
    "flippers2.jpg\0",
    "pinball.png\0",
    "open-chest2.png\0",
    "close-chest2.png\0",
    "board.jpg\0",
    "bumper_up.jpg\0",
    "bumper_down.jpg\0",
    "cannon.jpg\0",
    "steering_wheel.png\0",
    "flippers.png\0",
    "flippers2.jpg\0",
    "pinball.png\0",
    "open-chest2.png\0",
    "close-chest2.png\0",
    "board.jpg\0",
    "bumper_up.jpg\0",
    "bumper_down.jpg\0",
    "cannon.jpg\0",
    "steering_wheel.png\0",
    "rope.jpg\0",
    "rope_bent.png\0",
    "smoke0.png\0","smoke1.png\0",
    "smoke2.png\0","smoke3.png\0",
    "smoke4.png\0","smoke5.png\0",
    "smoke6.png\0","smoke7.png\0",
    "smoke8.png\0","smoke9.png\0",
    "smoke10.png\0","smoke11.png\0",
    "flag1.jpg\0", "flag2.jpg\0",
    "flag3.jpg\0", "flag4.jpg\0",
    "flag5.jpg\0", "flag6.jpg\0",
    "monster0.png\0", "monster3.png\0",
    "GameControls.jpg\0",
    "GameOver.jpg\0",
    "MainMenu.jpg\0",
    "close-chest21.jpg\0", "close-chest212.jpg\0",
    "scoreboard.png\0"

};
GLuint OceanTexture;

GLuint MainMenuTexture;
GLuint gameOverTexture;
GLuint controlsTexture;
GLuint flippersTexture;
GLuint flippersTexture2;
GLuint pinballTexture;
GLuint closeChestTexture;
GLuint openChestTexture;
GLuint openChestTexture_alpha;
GLuint closeChestTexture_alpha;
GLuint closeChestTexture_alpha2;
GLuint closeChestTexture_alpha3;
GLuint bumperUpTexture;
GLuint bumperDownTexture;
GLuint CannonTexture;
GLuint smokeSpriteTexture[SMOKE_SPRITES];
GLuint flagSpriteTexture[FLAG_SPRITES];
GLuint steeringWheelTexture;
GLuint ropeDeflectorTexture[2];
GLuint monsterTextures[6];
GLuint scoreBoardTexture;

//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;

int main(void)
{

    srand(time(NULL));
    Scorekeeper.points = 0;
    Scorekeeper.balls_left = 3;

    MainMenuOn = 1;
    pauseGame = 1;

    char syscall_buffer[256];
    char filename[256];

    for (int i = 0; i < NUM_IMAGES; i++) {
        strcpy(filename, ImageFile[i]);
        char *period = strchr(filename, '.');
        *period = '\0';   
        sprintf(syscall_buffer, "convert ./images/%s ./images/%s.ppm", 
                ImageFile[i], filename);

        system(syscall_buffer);
    }
    initXWindows();
    initOpengl();

    initGameBoard(board);
    initDeflectors(board);
    initCannons();
    initBumpers(board);
    initBalls();
    initChest(chest);//initialize chest properties
    initLauncher(cannon);
    initSmoke(smoke);
    initFlag(flag);
    initSteeringWheel(steeringWheel);
    initSeaMonster(seaMonster);

    gameSounds.initOpenAL();
    gameSounds.listener();
    gameSounds.createBuffers();
    gameSounds.loadSounds();
    gameSounds.generateSource();


    r.pos[0] = 200.0;
    r.pos[1] = 220.0;
    r.width = 100.0;
    r.height = 5.0;
    r.angle = 0.0;

    curve.points[0][0] = xres; curve.points[0][1] = yres - 200;
    curve.points[1][0] = xres; curve.points[1][1] = yres - 100;
    curve.points[2][0] = xres - 100; curve.points[2][1] = yres - 100;
    curve.width = 8.0;
    curve.npoints = 10;


    // Add chute to board
    addCurve(curve, board);

    Rectangle *rec = &board.rectangles[board.num_rectangles];
    rec->pos[0] = xres;
    rec->pos[1] = 300;
    rec->height = 200;
    rec->width = CHUTE_THICKNESS;
    board.num_rectangles++;

    rec = &board.rectangles[board.num_rectangles];
    rec->pos[0] = xres - CHUTE_WIDTH;
    rec->pos[1] = 300;
    rec->height = 200;
    rec->width = 6.0;
    board.num_rectangles++;

    std::cout << "num rectangles " << board.num_rectangles << endl;

    initFlipper(flipper, board.center[0] - 75, 100, false);
    initFlipper(flipper2, board.center[0] + 75, 100, true);

    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    gameSounds.playSound((char *)"soundtrack\0");
    while (!done) {
        while (XPending(dpy)) {
            XEvent e;
            XNextEvent(dpy, &e);
            checkResize(&e);
            checkMouse(&e);
            checkKeys(&e);
        }


        //if (!pauseGame) {
        clock_gettime(CLOCK_REALTIME, &timeCurrent);
        timeSpan = timeDiff(&timeStart, &timeCurrent);
        timeCopy(&timeStart, &timeCurrent);
        physicsCountdown += timeSpan;
        while (physicsCountdown >= physicsRate) {
            physics();
            physicsCountdown -= physicsRate;
        }

        //}
        render();
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    cleanup_fonts();
    gameSounds.cleanUpSound();
    for (int i = 0; i < NUM_IMAGES; i++) {
        strcpy(filename, ImageFile[i]);
        char *period = strchr(filename, '.');
        *period = '\0';   
        sprintf(syscall_buffer, "images/%s.ppm", filename);
        unlink(syscall_buffer);
    }
    return 0;
}

void cleanupXWindows(void)
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void setTitle(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "CS335 Demo Program");
}

void setupScreenRes(const int w, const int h)
{
    xres = w;
    yres = h;
}

void initXWindows(void)
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
    Colormap cmap;
    XSetWindowAttributes swa;

    setupScreenRes(xres, yres);
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(EXIT_FAILURE);
    } 
    //else {
    //	// %p creates hexadecimal output like in glxinfo
    //	printf("\n\tvisual %p selected\n", (void *)vi->visualid);
    //}
    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask |
        KeyReleaseMask | PointerMotionMask |
        ButtonPressMask | ButtonReleaseMask |
        StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
            vi->depth, InputOutput, vi->visual,
            CWColormap | CWEventMask, &swa);
    GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    setTitle();
    XAutoRepeatOn(dpy);
}

void reshapeWindow(int width, int height)
{
    //window has been resized.
    setupScreenRes(width, height);
    //
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, xres, 0, yres, -1, 1);
    setTitle();
}

void initOpengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, xres, yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, xres, 0, yres, -1, 1);
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    initTextures();
}


void resetGame() 
{
    gameNotOver = true;
    Scorekeeper.balls_left = 3;
    Scorekeeper.points = 0;
    Scorekeeper.rising_points = 0;

    initBalls();
    boom = false;
    launch = false;
    ball1.inPlay = false;
    cannonFired = false;
    cannon.active = 1;

}

void initCannons() 
{

    boardCannon.resting_pos[0] = xres - 100.0;
    boardCannon.resting_pos[1] = 550.0;
    Rectangle *rec = &boardCannon.r;
    VecCopy(rec->pos, boardCannon.resting_pos);
    rec->width = 40.0;
    rec->height = 40.0;
    rec->angle = 90.0;

    boardCannon.active = 1;
    boardCannon.loaded = 0;
    Vec vert;
    MakeVector(0, 1, 0, vert);
    VecRotate(vert, boardCannon.r.angle, vert);

    VecCopy(boardCannon.direction, vert);

    VecCopy(boardCannon.collision_circle.pos, boardCannon.r.pos);


    boardCannon.collision_circle.radius = 20.0;

    Rectangle *smoke_sprite = &boardCannon.smoke.r;
    smoke_sprite->pos[0] = boardCannon.resting_pos[0];
    smoke_sprite->pos[1] = boardCannon.resting_pos[1];
    smoke_sprite->width = 30.0;
    smoke_sprite->height = 50.0;
    smoke_sprite->angle = 90.0 + boardCannon.r.angle;

}

void initGameBoard(GameBoard &gb) 
{
    gb.center[0] = ((double)xres - CHUTE_THICKNESS - CHUTE_WIDTH) / 2.0 + monsterGutter;
    gb.center[1] = (double)yres / 2.0;

    gb.starting_point[0] = xres - CHUTE_THICKNESS - ball1.radius - 15;
    gb.starting_point[1] = 200;

    gb.num_rectangles = 0;

    Rectangle rec;

    //1st rec
    rec.angle = -50.0;
    rec.width = 60.0;
    rec.height = 10.0;
    MakeVector(gb.center[0] - 125, 135, 0, rec.pos);
    addRectangleToBoard(rec, gb);
    //2nd rec
    rec.angle = 50.0;
    rec.width = 165.0;
    MakeVector(gb.center[0] + 180, 210, 0, rec.pos);
    addRectangleToBoard(rec, gb);

    /****SIDE BOX******/
    rec.angle = -30;
    rec.width = 30;
    rec.height = 7;
    MakeVector(gb.center[0] - 350, 400, 0, rec.pos);
    addRectangleToBoard(rec,gb);

    rec.angle = 30;
    rec.width = 30;
    rec.height = 7;
    MakeVector(gb.center[0] - 230, 400, 0, rec.pos);
    addRectangleToBoard(rec,gb);

    rec.angle = -20;
    rec.width = 80;
    rec.height = 7;
    MakeVector(gb.center[0] - 300, 600, 0, rec.pos);
    addRectangleToBoard(rec,gb);


    curve.points[0][0] = gb.center[0] - 360; curve.points[0][1] = 350;
    curve.points[1][0] = gb.center[0] - 360; curve.points[1][1] = 180;
    curve.points[2][0] = gb.center[0] - 150; curve.points[2][1] = 180;
    curve.width = 8.0;
    curve.npoints = 10;

    // Add chute to board
    addCurve(curve, board);
}

void initBalls(void)
{
    ball1.radius = 11.0;
    ball1.pos[0] = xres - CHUTE_THICKNESS - ball1.radius;
    ball1.pos[1] = 200;
    //ball1.vel[0] = 1.6;
    //ball1.vel[1] = 0.0;
    ball1.mass = 1.0;

}

void initSeaMonster(SeaMonster &monster)
{   
    double h = (double)yres / 3.0;
    monster.rectangle.width  = h;
    monster.rectangle.height = h;
    monster.rectangle.angle = 90; 
    monster.state = -1;

    //set up left tentacle
    MakeVector(-h, h, 0, monster.rectangle.pos);
    MakeVector(-monster.rectangle.width, h, 0, monster.hiding_pos);
    MakeVector(40, h, 0, monster.active_pos);
    MakeVector(40, -monster.rectangle.height, 0, monster.dead_pos);

    monster.num_bloodspurts = 1;
    Smoke *blood = &monster.bloodspurts[0];
    blood->state = 0;
    MakeVector(100, 100, 0, blood->r.pos);
    blood->r.angle = 0;
    blood->frame = 0;

    MakeVector(-h + 90, 550, 0,
            monster.collision_circle.pos);
    monster.collision_circle.radius = 60.0;
}

void killSeaMonster(SeaMonster &monster)
{
    if (monster.state == 1) {
        monster.state = 2;
        timeCopy(&monster.active_time, &timeCurrent);
    }
}

void seaMonsterPhysics(SeaMonster &monster, Ball &ball)
{

    if (monster.state == 1 &&
            insideCircle(monster.collision_circle.radius,
                monster.collision_circle.pos, ball)) {
        monster.state = 2;
        addScore(&Scorekeeper, 50);
        timeCopy(&monster.active_time, &timeCurrent);

    }
    if (monster.HP == 0) {
        monster.HP = 3;
        monster.state = 3;
        addScore(&Scorekeeper, 1000);
        timeCopy(&monster.active_time, &timeCurrent);
    }
}

void seaMonsterState(SeaMonster &monster)
{

    /*std::cout << "time difference: " << timeDiff(&monster.active_time, &timeCurrent) << std::endl;
      std::cout << "monster state: " << monster.state << std::endl;
    //just started
     */
    if (monster.state == -1) {
        timeCopy(&monster.active_time, &timeCurrent);
        monster.state = 0;
    }

    switch (monster.state) {
        //if hidden for 5 seconds
        case 0: 
            if (monster.rectangle.pos[0] > -(monster.rectangle.width)) {
                monster.rectangle.pos[0] -= 2;
                monster.collision_circle.pos[0] -= 2.0;
            }
            if (timeDiff(&monster.active_time, &timeCurrent) > 10) {
                //make active
                monster.state = 1;
                //reset timer
                timeCopy(&monster.active_time, &timeCurrent);
            }
            break;

            //move right until on screen
        case 1:
            if (monster.rectangle.pos[0] < 40) {
                monster.rectangle.pos[0] += 2.0;
                monster.collision_circle.pos[0] += 2.0;
            } else {
                monster.rectangle.pos[0] = 40;
            }

            if (timeDiff(&monster.active_time, &timeCurrent) > 15)
            {
                monster.state = 0;
                timeCopy(&monster.active_time, &timeCurrent);
            }
            break;
        case 2:
            //just got hit
            if (timeDiff(&monster.active_time, &timeCurrent) < 0.5) {
                gameSounds.playSound((char *)"monster\0");
                monster.rectangle.pos[0] += (rand() % 10) - 5;
                monster.rectangle.pos[1] += (rand() % 10) - 5;

            } else {
                //after 0.5 seconds, monster is dead
                monster.HP--;
                monster.state = 0;
                timeCopy(&monster.active_time, &timeCurrent);
            }
            break;

        case 3:
            //dead
            if (monster.rectangle.pos[1] > -monster.rectangle.height) {
                //monster sinks
                monster.rectangle.pos[1] -= 1;
            } else {
                //reset state back to 0
                monster.state = 0;
                initSeaMonster(monster);
                timeCopy(&monster.active_time, &timeCurrent);
            }
            break;
    }

}

void drawSeaMonster(SeaMonster &monster)
{
    glPushMatrix();

    //drawCircle(monster.collision_circle);

    //left tentacle
    if (monster.state < 3) {
        //draw normal image if not damaged
        if (monster.state == 2)
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        else 
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        drawRectangleTextureAlpha(monster.rectangle, monsterTextures[0]);
    } else {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        //draw damaged image
        drawRectangleTextureAlpha(monster.rectangle, monsterTextures[3]);
    }
    glPopMatrix();
}

void initDeflectors(GameBoard &board)
{
    Deflector *deflector = &board.deflectors[0];
    MakeVector(board.center[0] - 120, 350, 0, deflector->r.pos);
    deflector->r.angle = -70.0;
    deflector->r.width = 40;
    deflector->r.height = 5;

    deflector++;
    MakeVector(board.center[0] + 120, 350, 0, deflector->r.pos);
    deflector->r.angle = 70.0;
    deflector->r.width = 40;
    deflector->r.height = 5;

    deflector++;
    MakeVector(
            xres - 100, 
            yres - 50, 0, 
            deflector->r.pos);
    deflector->r.angle = 90.0;
    deflector->r.width = 50.0;
    deflector->r.height = 5.0;

    deflector++;
    MakeVector(board.center[0] + 160, 200, 0, deflector->r.pos);
    deflector->r.angle = 50;
    deflector->r.width = 30.0;
    deflector->r.height = 5.0;

    deflector++;
    MakeVector(monsterGutter + 60, yres - 290, 0, deflector->r.pos);
    deflector->r.angle = -20;
    deflector->r.width = 30;
    deflector->r.height = 5.0;

    board.num_deflectors = 5;
}

void initFlipper(Flipper &f, float xpos, float ypos, bool inverted=false)
{
    f.pos[0] = xpos;
    f.pos[1] = ypos;
    f.width = FLIPPER_LENGTH;
    f.height = FLIPPER_HEIGHT;

    f.angle = FLIPPER_REST_ANGLE;
    f.flipstate = 0;
    f.rvel = 0;
    f.inverted = inverted;
}

void checkResize(XEvent *e)
{
    //The ConfigureNotify is sent by the
    //server if the window is resized.
    if (e->type != ConfigureNotify)
        return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != xres || xce.height != yres) {
        //Window size did change.
        reshapeWindow(xce.width, xce.height);
    }
}

void checkMouse(XEvent *e)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    if (e->type == ButtonRelease) {
        leftButtonDown=0;
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button is down
            leftButtonDown = 1;
            leftButtonPos[0] = (Flt)e->xbutton.x;
            leftButtonPos[1] = (Flt)(yres - e->xbutton.y);
        }
        if (e->xbutton.button==3) {
            //Right button is down
        }
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        //Mouse moved
        savex = e->xbutton.x;
        savey = e->xbutton.y;
        if (leftButtonDown) {
            leftButtonPos[0] = (Flt)e->xbutton.x;
            leftButtonPos[1] = (Flt)(yres - e->xbutton.y);
        }
    }
}

void checkKeys(XEvent *e)
{
    int key = XLookupKeysym(&e->xkey, 0);

    if (e->type == KeyPress) {
        switch (key) {
            case XK_1:
                if (MainMenuOn == 1) {
                    MainMenuOn = 0;
                    pauseGame = 0;
                    resetGame();
                }
                if (!gameNotOver) {
                    resetGame();
                }
                break;

            case XK_2:
                if (MainMenuOn == 1) {
                    done = 1;
                }
                if (!gameNotOver || pauseGame) {
                    pauseGame = 1;
                    MainMenuOn = 1;
                    resetGame();
                }
                break;

            case XK_Left:
                ball1.vel[0] -= 1.0;
                break;
            case XK_Right:
                ball1.vel[0] += 1.0;
                break;
            case XK_Up:
                ball1.vel[1] = MAX_VELOCITY;
                break;
            case XK_Down:
                ball1.vel[1] -= 1.0;
                break;
            case XK_s:
                //press s to slow the balls
                ball1.vel[0] *= 0.5;
                ball1.vel[1] *= 0.5;
                ball2.vel[0] *= 0.5;
                ball2.vel[1] *= 0.5;
                break;
            case XK_f:
                //press s to slow the balls
                flipper.flipstate = 1;
                gameSounds.playSound((char *)"flippers\0");
                break;
            case XK_k:
                //flipper 2
                flipper2.flipstate = 1;
                gameSounds.playSound((char *)"flippers\0");
                break;
            case XK_b:
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
                break;
            case XK_h:
                hide = true;
                pauseGame ^= 1;
                break;
            case XK_v:
                killSeaMonster(seaMonster);
                break;
            case XK_Escape:
                done=1;
                break;
        }

    }
    else if (e->type == KeyRelease)
    {
        char keys[32];
        XQueryKeymap(dpy, keys);

        if (!(keys[e->xkey.keycode>>3] & (0x1 << (e->xkey.keycode % 8))))
        {
            // Stuff to do on KeyRelease
            switch (key) {
                case XK_f:
                    //press s to slow the balls
                    flipper.flipstate = 2;
                    break;
                case XK_k:
                    //flipper 2
                    flipper2.flipstate = 2;
                    break;
            }
        }
    }
}



//moves the flipper based on its flipstate
void flipperMovement(Flipper &f)
{
    //add rotational velocity to angle
    f.angle += f.rvel;

    switch (f.flipstate)
    {
        //idle; rotational velocity is 0 and the angle is the rest angle
        case 0:
            f.rvel = 0;
            f.angle = FLIPPER_REST_ANGLE;
            break;
        case 1:
            //going up; set rotational velocity to 
            //flipper speed until the flipper passes the max angle
            if (f.angle < 40) {
                f.rvel = FLIPPER_SPEED;
            }
            else {
                // if the flipper is passed the max angle, set flipstate to 
                //'going down'
                f.rvel = 0;
                //f.flipstate = 2;
            }
            break;
        case 2: 
            //going down; make rotational velocity negative 
            //until flipper is at rest angle
            if (f.angle > FLIPPER_REST_ANGLE) {
                f.rvel = -10;
            }
            else {
                f.rvel = 0;
                f.flipstate = 0;
            }
            break;
    }
}

void cannonPhysics(Cannon &can, Ball &ball)
{
    if (insideCircle(can.collision_circle.radius, can.collision_circle.pos, ball)) { 
        if (!can.loaded) {
            can.loaded = 1;
            ball.vel[0] = 0;
            ball.vel[1] = 0;
            ball.isVisible = 0;
            ball.hasGravity = 0;
            timeCopy(&can.timer, &timeCurrent);
            VecScale(can.direction, can.r.height + ball.radius, ball.pos);
            VecAdd(ball.pos, can.r.pos, ball.pos);
        }


    }

    if (can.loaded && !can.firing
            && timeDiff(&can.timer, &timeCurrent) > 10.0) {

        can.loaded = 0;
        can.firing = 1;

        ball1.hasGravity = 1;
        ball.isVisible = 1;
        VecScale(can.direction, 15.0, ball.vel);

    }
}

void fireCannon(Cannon &can, Ball &ball) {
    can.loaded = 0;
    can.firing = 1;

    ball1.hasGravity = 1;
    ball.isVisible = 1;
    VecScale(can.direction, 15.0, ball.vel);
}

void physics(void)
{
    //gravity
    if (ball1.inPlay && !pauseGame && ball1.hasGravity) {
        ball1.vel[1] += -0.2;
    }
    //flipper physics
    flipperMovement(flipper);
    flipperBallCollision(flipper, ball1);
    //flipper2 physics
    flipperMovement(flipper2);
    flipperBallCollision(flipper2, ball1);


    //rectangle collisions
    bool collided = false;
    for (int i = 0; i < board.num_rectangles; i++) {
        if (rectangleBallCollision(board.rectangles[i], ball1)) {
            collided = true;

        }
    }

    if (chest.active == 0) {
        if (timeDiff(&chest.collision_time, &timeCurrent) > 0.5) {
            chest.active = 1;

        }

    }
    //treasure chest collision
    if (rectangleBallCollision(chest.r, ball1)) {

        if (chest.active == 1) {
            timeCopy(&chest.collision_time, &timeCurrent);
            chest.active = 0;

            if (ballChestCollision(chest, ball1)) {
                collided = true;
            }
        }


    }   

    if (chest.state == 1 && timeDiff(&chest.collision_time, &timeCurrent) > 5) {
        gameSounds.playSound((char *)"chest\0");
        chest.state = 0;
        chest.HP = 3;
    } 

    //bumper collision
    for (int i = 0; i < board.num_bumpers; i++) {
        if (bumperBallCollision(board.bumpers[i], ball1)) {
            gameSounds.playSound((char *)"barrels\0");
        }
    }

    //deflector collisions
    for (int i = 0; i < board.num_rectangles; i++) {
        if (deflectorBallCollision(board.deflectors[i], ball1)) {
        }
    }

    //steering wheel collision
    steeringWheelBallCollision(steeringWheel, ball1);
    steeringWheelMovement(steeringWheel);

    flagPhysics(flag, ball1);

    //cannons
    cannonPhysics(boardCannon, ball1);


    static bool cannonGoesUp = true;
    //make cannon move up and down
    if (boardCannon.r.angle > 45.0 + 90.0) {
        cannonGoesUp = false;
    }
    if (boardCannon.r.angle < -65.0 + 90.0) {
        cannonGoesUp = true;
    }

    if (!boardCannon.firing) {
        rotateCannon(boardCannon, cannonGoesUp ? 1.0 : -1.0);
    }



    if (collided) {
        //apply roll
        double momentum = ball1.vel[0] * 0.6;
        Vec mv;
        MakeVector(1, 0, 0, mv);
        VecScale(mv, momentum, mv);
        VecAdd(ball1.vel, mv, ball1.vel);

    }

    for (int i = 0; i < board.num_bumpers; i++) {
        if (bumperBallCollision(board.bumpers[i], ball1)) {
        }
    }
    //KaBoom(Cannon, ball1, alSource);//when ball is on cannon

    applyMaximumVelocity(ball1);

    //set ball inPlay flag if ball is left of launch chute
    if (ball1.pos[0] < xres - CHUTE_WIDTH - CHUTE_THICKNESS) {
        ball1.inPlay = 1;
        cannon.active = 0;
    }

    seaMonsterPhysics(seaMonster, ball1);

    if (!pauseGame || !gameNotOver) {
        //Update position
        ball1.pos[0] += ball1.vel[0];
        ball1.pos[1] += ball1.vel[1];
        seaMonsterState(seaMonster);
    }

    if (leftButtonDown) {
        //make ball go toward mouse pointer position
        ball1.vel[0] = (leftButtonPos[0] - ball1.pos[0]) * 0.5;
        ball1.vel[1] = (leftButtonPos[1] - ball1.pos[1]) * 0.5;
    }

    gameOver(board, ball1, Scorekeeper);

    //Check for collision with window edges

    //right board edge shifts left if the ball is in play 
    //and has left the chute
    double boardEdge = ball1.inPlay 
        ? xres - (CHUTE_WIDTH + CHUTE_THICKNESS + ball1.radius + 0.5) 
        : xres - ball1.radius;

    //left window edge
    if (ball1.pos[0] < ball1.radius + monsterGutter && ball1.vel[0] < 0.0) {
        ball1.pos[0] = ball1.radius + monsterGutter;
        ball1.vel[0] *= -0.2;
    }

    //right window edge
    else if (ball1.pos[0] >= boardEdge && ball1.vel[0] > 0.0) {
        ball1.pos[0] = boardEdge;
        ball1.vel[0] *= - 0.2;
    }

    //bottom window edge

    if (ball1.pos[1] < ball1.radius && ball1.vel[1] < 0.0) {
        ball1.pos[1] = ball1.radius;
        ball1.vel[1] *= - 0.2;
    }

    //top window edge
    else if	(ball1.pos[1] >= (Flt)yres-ball1.radius && ball1.vel[1] > 0.0) {
        ball1.pos[1] = yres - ball1.radius;
        ball1.vel[1] *= -0.2;
    }
}

void initTextures(void)
{

    strcpy(buffer, "./images/rope.ppm");
    alphaTextureInit(buffer, ropeDeflectorTexture[0], ropeDeflectorImage[0]);

    strcpy(buffer, "./images/rope_bent.ppm");
    alphaTextureInit(buffer, ropeDeflectorTexture[1], ropeDeflectorImage[1]);

    strcpy(buffer, "./images/pinball.ppm");
    textureInit(buffer, pinballTexture, pinballImage);

    strcpy(buffer, "./images/flippers.ppm");
    textureInit(buffer, flippersTexture, flippers);

    strcpy(buffer, "./images/flippers2.ppm");
    textureInit(buffer, flippersTexture2, flippers2);

    strcpy(buffer, "./images/board.ppm");
    textureInit(buffer, OceanTexture, OceanImage);

    strcpy(buffer , "./images/cannon.ppm");
    alphaTextureInit(buffer, CannonTexture, CannonImage);

    strcpy(buffer, "./images/open-chest2.ppm");
    alphaTextureInit(buffer, openChestTexture_alpha, openChestImage);

    strcpy(buffer, "./images/close-chest2.ppm");
    alphaTextureInit(buffer, closeChestTexture_alpha, closeChestImage);

    strcpy(buffer, "./images/close-chest21.ppm");
    alphaTextureInit(buffer, closeChestTexture_alpha2, closeChestImage2);

    strcpy(buffer, "./images/close-chest212.ppm");
    alphaTextureInit(buffer, closeChestTexture_alpha3, closeChestImage3); 

    strcpy(buffer, "./images/bumper_up.ppm");
    alphaTextureInit(buffer, bumperUpTexture, bumperUpImage);

    strcpy(buffer, "./images/bumper_down.ppm");
    alphaTextureInit(buffer, bumperDownTexture, bumperDownImage);

    strcpy(buffer, "./images/steering_wheel.ppm");
    alphaTextureInit(buffer, steeringWheelTexture, steeringWheelImage);

    strcpy(buffer, "./images/smoke0.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[0], smokeSprites[0]);

    strcpy(buffer, "./images/smoke1.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[1], smokeSprites[1]);

    strcpy(buffer, "./images/smoke2.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[2], smokeSprites[2]);

    strcpy(buffer, "./images/smoke3.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[3], smokeSprites[3]);

    strcpy(buffer, "./images/smoke4.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[4], smokeSprites[4]);

    strcpy(buffer, "./images/smoke5.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[5], smokeSprites[5]);

    strcpy(buffer, "./images/smoke6.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[6], smokeSprites[6]);

    strcpy(buffer, "./images/smoke7.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[7], smokeSprites[7]);

    strcpy(buffer, "./images/smoke8.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[8], smokeSprites[8]);

    strcpy(buffer, "./images/smoke9.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[9], smokeSprites[9]);

    strcpy(buffer, "./images/smoke10.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[10], smokeSprites[10]);

    strcpy(buffer, "./images/smoke11.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[11], smokeSprites[11]);

    sprintf(buffer, "./images/monster%d.ppm", 0);
    alphaTextureInit(buffer, monsterTextures[0], monsterImages[0]);

    sprintf(buffer, "./images/monster%d.ppm", 3);
    alphaTextureInit(buffer, monsterTextures[3], monsterImages[3]);

    sprintf(buffer, "./images/flag1.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[0], flagSprites[0]);

    sprintf(buffer, "./images/flag2.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[1], flagSprites[1]);

    sprintf(buffer, "./images/flag3.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[2], flagSprites[2]);

    sprintf(buffer, "./images/flag4.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[3], flagSprites[3]);

    sprintf(buffer, "./images/flag5.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[4], flagSprites[4]);

    sprintf(buffer, "./images/flag6.ppm");
    alphaTextureInit(buffer, flagSpriteTexture[5], flagSprites[5]);

    sprintf(buffer, "./images/GameControls.ppm");
    alphaTextureInit(buffer, controlsTexture, controlsImage);

    sprintf(buffer, "./images/GameOver.ppm");
    alphaTextureInit(buffer, gameOverTexture, gameOverImage);

    sprintf(buffer, "./images/MainMenu.ppm");
    alphaTextureInit(buffer, MainMenuTexture, MainMenuImage);

    sprintf(buffer, "./images/scoreboard.ppm");
    alphaTextureInit(buffer, scoreBoardTexture, scoreBoardImage);
}

void drawSteeringWheel(SteeringWheel &wheel)
{
    glPushMatrix();
    glColor3d(1.0, 1.0, 1.0);
    glTranslated(wheel.pos[0], wheel.pos[1], wheel.pos[2]);
    glRotatef(wheel.angle, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, steeringWheelTexture);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);

    glBegin(GL_QUADS);
    glVertex2d(-wheel.outer_radius, -wheel.outer_radius); 
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(-wheel.outer_radius,  wheel.outer_radius); 
    glTexCoord2f(0.0f, 0.0f); 
    glVertex2d( wheel.outer_radius,  wheel.outer_radius); 
    glTexCoord2f(1.0f, 0.0f); 
    glVertex2d( wheel.outer_radius, -wheel.outer_radius); 
    glTexCoord2f(1.0f, 1.0f); 
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void drawDeflector(Deflector &d)
{
    glPushMatrix();
    glColor3d(1.0, 1.0, 1.0);
    glTranslated(d.r.pos[0], d.r.pos[1], d.r.pos[2]);
    glRotatef(d.r.angle, 0, 0, 1);

    //use d.state to determine which image to bind
    glBindTexture(GL_TEXTURE_2D, ropeDeflectorTexture[d.state]);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);

    //enlarge the image beyond the width of the rectangle
    //get ratio between image height and rectangle height
    //w = ropeDeflectorImage[0]->width * ratio;
    double w = d.r.width;
    double h = d.r.height * 2;

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(-w, -h);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-w,  h);
    glTexCoord2f(0.0f, 1.0f); glVertex2i( w,  h);
    glTexCoord2f(1.0f, 1.0f); glVertex2i( w, -h);
    glEnd();


    glPopMatrix();
}


void OceanBackground()
{
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, OceanTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
    glEnd();
}

void flagPhysics(Flag &f, Ball &b)
{
    if (insideRectangle(f.r, b)) {
        f.state = 1;
        addScore(&Scorekeeper, 10);
    }
}

void flagAnimation(Flag &f)
{		
    //Flag properties
    //int flagFrame
    //timespec flagFrameTimer
    //Rectangle r
    if (f.flagFrame < FLAG_SPRITES * 2) {
        //cout << "flag: " << f.flagFrame << endl;
        glPushMatrix();
        glColor3d(1.0, 1.0, 1.0);

        f.r.angle = 90;
        drawRectangleTextureAlpha(f.r, flagSpriteTexture[f.flagFrame % FLAG_SPRITES]);
        /*
           glTranslated(f.r.pos[0], f.r.pos[1], f.r.pos[2]);
           glRotatef(f.r.angle + 90.0, 0, 0, 1);
           glBindTexture(GL_TEXTURE_2D, flagSpriteTexture[f.flagFrame]);
           glEnable(GL_ALPHA_TEST);
           glAlphaFunc(GL_GREATER, 0.0f);
           glColor4ub(255,255,255,255);

           glBegin(GL_QUADS);
           glVertex2d(-f.r.width, -f.r.height); glTexCoord2f(0.0f, 1.0f);
           glVertex2d(-f.r.width, f.r.height); glTexCoord2f(0.0f, 0.0f); 
           glVertex2d(f.r.width, f.r.height); glTexCoord2f(1.0f, 0.0f); 
           glVertex2d(f.r.width, -f.r.height); glTexCoord2f(1.0f, 1.0f); 
           glEnd();

           glBindTexture(GL_TEXTURE_2D,0);*/
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


//Display Image 
void drawFlipper(Flipper &f)
{
    float length = f.inverted ? -FLIPPER_LENGTH : FLIPPER_LENGTH;
    float angle = f.inverted ? -f.angle : f.angle;
    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslated(f.pos[0], f.pos[1], f.pos[2]);
    glRotatef(angle, 0, 0, 1);
    glBindTexture(GL_TEXTURE_2D, flippersTexture);
    glBegin(GL_QUADS);
    glVertex2f(0, -FLIPPER_HEIGHT); glTexCoord2f(1.0f, 0.0f); 
    glVertex2f(length, -FLIPPER_HEIGHT); glTexCoord2f(0.0f, 0.0f);
    glVertex2f(length, 0); glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(0, 0); glTexCoord2f(1.0f, 1.0f); 
    glEnd();
    glPopMatrix();
}
//display pinball
void drawBall()
{
    float angle, radian, x, y, tx, ty, xcos, ysin;

    glBindTexture(GL_TEXTURE_2D, pinballTexture);
    glBegin(GL_POLYGON);

    for (angle = 0.0; angle < 360.0; angle+= 2.0)
    {
        radian = angle * (M_PI/100.0f);
        xcos = (float)cos(radian);
        ysin = (float)sin(radian);
        x = xcos * ball1.radius;
        y = ysin * ball1.radius;
        tx = xcos * 0.5 + 0.5;
        ty = ysin * 0.5 + 0.5;

        glTexCoord2f(tx, ty);
        glVertex2f(x, y);
    }
    glEnd();
}
void drawBumper(Bumper &b)
{
    double angle, radian, x, y, tx, ty, xcos, ysin;
    double radius = b.state == 0 ? b.c.radius : b.c.radius - 2.0;

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslated(b.c.pos[0], b.c.pos[1], b.c.pos[2]);
    glBindTexture(GL_TEXTURE_2D, b.state == 0 
            ? bumperUpTexture 
            : bumperDownTexture);
    glBegin(GL_POLYGON);

    for (angle = 0.0; angle < 360.0; angle+= 2.0) {
        radian = angle * (M_PI/100.0f);

        xcos = (float)cos(radian);
        ysin = (float)sin(radian);
        x = xcos * radius;
        y = ysin * radius;
        tx = xcos * 0.5 + 0.5;
        ty = ysin * 0.5 + 0.5;

        glTexCoord2f(tx, ty);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}

void rotateCannon(Cannon &c, double angle) {
    c.r.angle += angle;
    VecRotate(c.direction, angle, c.direction);
    c.smoke.r.angle += angle;

    VecCopy(c.smoke.r.pos, c.r.pos);
    VecScale(c.direction, c.r.height + ball1.radius, tmp);
    VecAdd(tmp, c.r.pos, c.smoke.r.pos);

}

void drawCannonMovement(Cannon &c)
{
    if (c.firing == 1)
    {
        //move cannon down for first eighth of animation
        if (c.smoke.frame < SMOKE_SPRITES / 8.0) {
            VecScale(c.direction, -10.0, tmp);
            VecAdd(c.r.pos, tmp, c.r.pos);
            //c.r.pos[1] -= 10.0;
            //and up for the rist
        } else {
            VecAdd(c.r.pos, c.direction, c.r.pos);
            //c.r.pos[1] += 1.0;
        }
    }
}

void drawCannon(Cannon &c)
{
    drawRectangleTextureAlpha(c.r, CannonTexture);

    //smoke
    if (c.firing) {
        c.smoke.state = 1;
        if (c.smoke.frame < SMOKE_SPRITES) {
            smokeAnimation(c.smoke, timeCurrent);

        } else {
            //animation finished
            c.smoke.frame = 0;
            VecCopy(c.r.pos, c.resting_pos);
            boom = false;
            c.smoke.state = 0;
            c.firing = 0;
        }
        drawCannonMovement(c);
    }

}
//function draws treasure chest object
void drawChest(TreasureChest &c)
{

    if (c.state == 1) {
        drawRectangleTextureAlpha(c.r, openChestTexture_alpha);
    }
    else if (c.state == 0) {
        if (c.HP == 3)
            drawRectangleTextureAlpha(c.r, closeChestTexture_alpha);
        if (c.HP == 2)
            drawRectangleTextureAlpha(c.r, closeChestTexture_alpha2);
        if (c.HP == 1)
            drawRectangleTextureAlpha(c.r, closeChestTexture_alpha3);
    }
}
void render(void)
{
    if (MainMenuOn) {
        showMainMenu();
        return;
    }

    if (pauseGame || !gameNotOver) {
        Rectangle screen;
        screen.width = yres / 2.0;
        
        if (!gameNotOver) {
            screen.width = yres / 2.0 - 50.0;
        }

        screen.height = xres / 2.0;
        screen.pos[0] = (double)xres / 2.0;
        screen.pos[1] = (double)yres / 2.0;
        screen.angle = 90.0;        
        glColor4d(1.0,1.0,1.0,1.0);
        drawRectangleTextureAlpha(screen, 
                gameNotOver ? controlsTexture: gameOverTexture);
        

        return;

    }

    //	Rect re;
    glClear(GL_COLOR_BUFFER_BIT);

    //Curve
    glLineWidth(2.5);
    glColor3ub(150, 10, 10);
    glPushMatrix();
    glBegin(GL_LINES);
    glVertex3f(curve.points[0][0], curve.points[0][1],0);
    glVertex3f(curve.points[1][0], curve.points[1][1],0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(curve.points[1][0], curve.points[1][1], 0);
    glVertex3f(curve.points[2][0], curve.points[2][1], 0);
    glEnd();

    glEnd();
    glPopMatrix();

    OceanBackground();
    flagAnimation(flag);
    drawChest(chest);//drawing chest
    drawSeaMonster(seaMonster);
    drawSteeringWheel(steeringWheel);
    drawCannon(cannon);//draw canon 
    drawCannon(boardCannon);

    drawScore();
    //show board edge for debug
    /*
       glPushMatrix();
       glColor3f(1.0f, 1.0f, 1.0f);
       glBegin(GL_QUADS);
       glVertex2i(monsterGutter, 0);
       glVertex2i(monsterGutter, yres);
       glVertex2i(monsterGutter + 10, yres);
       glVertex2i(monsterGutter + 10, 0);
       glEnd();
       glPopMatrix();
     */
    //draw collision rectangles
    glColor3ub(255, 255, 255);
    for (int i = 0; i < board.num_rectangles; i++) {
        //drawRectangle(board.rectangles[i]);
    }

    //draw deflectors
    for (int i = 0; i < board.num_deflectors; i++) { 
        drawDeflector(board.deflectors[i]);
    }

    //draw bumpers
    for (int i = 0; i < board.num_bumpers; i++) {
        drawBumper(board.bumpers[i]);
    }

    //draw balls
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslated(ball1.pos[0], ball1.pos[1], ball1.pos[2]);


    if (ball1.isVisible && launch) {
        drawBall();
    }
    //drawBall();
    glPopMatrix();

    drawFlipper(flipper);
    drawFlipper(flipper2);
    //drawScore();


}


