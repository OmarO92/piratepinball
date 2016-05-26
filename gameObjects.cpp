#include "gameObjects.h"
#include "vector.h"
#include "alexR.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <GL/glx.h>
#include <cstdlib>

extern int xres, yres;

void initBumpers(GameBoard &g)
{
    Bumper b;
    MakeVector(g.center[0], yres - 300, 0, b.c.pos);
    b.c.radius = 20;
    b.state = 0;
    addBumperToBoard(b, g);

    MakeVector(g.center[0] - 75, yres - 200, 0, b.c.pos);
    addBumperToBoard(b, g);

    MakeVector(g.center[0] + 75, yres - 200, 0, b.c.pos);
    addBumperToBoard(b, g);
    
    MakeVector(g.center[0] - 220, 550, 0, b.c.pos);
    addBumperToBoard(b, g);

    MakeVector(g.center[0] - 220, 450, 0, b.c.pos);
    addBumperToBoard(b, g);
    
    MakeVector(g.center[0] - 320, 500, 0, b.c.pos);
    b.c.radius = 10.0;
    addBumperToBoard(b, g);

}

void addBumperToBoard(Bumper &b, GameBoard &g)
{
    Bumper *currentBumper = &g.bumpers[g.num_bumpers];
    currentBumper->c.pos[0] = b.c.pos[0];
    currentBumper->c.pos[1] = b.c.pos[1];
    currentBumper->c.radius = b.c.radius;
    currentBumper->state = b.state;
    g.num_bumpers++;
}

void addRectangleToBoard(Rectangle &r, GameBoard &g)
{
    Rectangle *rectangle = &g.rectangles[g.num_rectangles];
    rectangle->pos[0] = r.pos[0];
    rectangle->pos[1] = r.pos[1];
    rectangle->width  = r.width;
    rectangle->height = r.height;
    rectangle->angle = r.angle;
    g.num_rectangles++;
}


/* This draws rectangles for debugging */
void drawRectangle(Rectangle &r)
{

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslated(r.pos[0], r.pos[1], r.pos[2]);
    glRotatef(r.angle, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2i(-r.width, -r.height);
    glVertex2i(-r.width, r.height);
    glVertex2i(r.width, r.height);
    glVertex2i(r.width, - r.height);
    glEnd();
    glPopMatrix();
}

/* This draws circles for debugging */
void drawCircle(Circle &c)
{
    glPushMatrix();
    glTranslated(c.pos[0], c.pos[1], 0);
    int i;
    double verts[32][2];
    int n=32;
    double ang=0.0;
    double inc = 3.14159 * 2.0 / (double)n;
    for (i=0; i<n; i++) {
        verts[i][0] = sin(ang);
        verts[i][1] = cos(ang);
        ang += inc;
    }
    glColor3ub(250, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    for (i=0; i<n; i++) {
        glVertex2f(verts[i][0]*c.radius, verts[i][1]*c.radius);
    }
    glEnd();
    glPopMatrix();
}
/*void makeWater(Water *water, int x, int y) {
    if (water->n >= MAX_PARTICLES) {
        return;
		}
		else {
    Particle *p = &water->particle[water->n];
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y =  -0.5;
    p->velocity.x = + 0.5;
    water->n++;
    water.pos[0] = 150;
    water.pos[1] = 150;
		}
}*/
//function builds Alpha channel for image with no background
unsigned char *buildAlphaData(Ppmimage *img)
{
        //add 4th component to RGB stream...
        int i;
        int a,b,c;
        unsigned char *newdata, *ptr;
        unsigned char *data = (unsigned char *)img->data;
        newdata = (unsigned char *)malloc(img->width * img->height * 4);
        ptr = newdata;
        for (i=0; i<img->width * img->height * 3; i+=3) {
                a = *(data+0);
                b = *(data+1);
                c = *(data+2);
                *(ptr+0) = a;
                *(ptr+1) = b;
                *(ptr+2) = c;
                //get largest color component...
                //*(ptr+3) = (unsigned char)((
                //		(int)*(ptr+0) +
                //		(int)*(ptr+1) +
                //		(int)*(ptr+2)) / 3);
                //d = a;
                //if (b >= a && b >= c) d = b;
                //if (c >= a && c >= b) d = c;
                //*(ptr+3) = d;
                *(ptr+3) = (a|b|c);
                ptr += 4;
                data += 3;
        }
        return newdata;
}

void drawRectangleTextureAlpha(Rectangle &r, GLuint &textureId)
{

    glPushMatrix();
    //glColor3d(1.0, 1.0, 1.0);
    glTranslated(r.pos[0], r.pos[1], r.pos[2]);
    glRotatef(r.angle, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    //glColor4ub(255,255,255,255);

    glBegin(GL_QUADS);
    glVertex2d(-r.width, -r.height); glTexCoord2f(0.0f, 1.0f);
    glVertex2d(-r.width,  r.height); glTexCoord2f(0.0f, 0.0f); 
    glVertex2d( r.width,  r.height); glTexCoord2f(1.0f, 0.0f); 
    glVertex2d( r.width, -r.height); glTexCoord2f(1.0f, 1.0f); 
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

double timeDiff(struct timespec *start, struct timespec *end) 
{
    return (double)(end->tv_sec - start->tv_sec ) +
        (double)(end->tv_nsec - start->tv_nsec) * (1.0/1e9);
}

void timeCopy(struct timespec *dest, struct timespec *source) 
{
    std::memcpy(dest, source, sizeof(struct timespec));
}
