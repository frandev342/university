#ifndef TURTLEC_H
#define TURTLEC_H

#include <SFML/Graphics.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Turtle Turtle;
typedef struct TurtleApp TurtleApp;
typedef void (*TurtleDrawFunc)(Turtle *turtle);

struct Turtle {
  sfRenderWindow *window;
  sfVertexArray *lines;
  sfConvexShape *shape;
  sfVector2f pos;
  float heading;
  bool penIsDown;
  sfColor penColor;
  float speed;
  unsigned int width;
  unsigned int height;
};

struct TurtleApp {
  sfRenderWindow *window;
  Turtle turtle;
};

void turtleInit(Turtle *turtle, sfRenderWindow *window, unsigned int width, unsigned int height);
void turtleDestroy(Turtle *turtle);

void turtleForward(Turtle *turtle, float dist);
void turtleBackward(Turtle *turtle, float dist);
void turtleRight(Turtle *turtle, float angle);
void turtleLeft(Turtle *turtle, float angle);
void turtlePenUp(Turtle *turtle);
void turtlePenDown(Turtle *turtle);
void turtleSetColor(Turtle *turtle, uint8_t r, uint8_t g, uint8_t b);
void turtleGoTo(Turtle *turtle, float x, float y);
void turtleSetSpeed(Turtle *turtle, float speed);
void turtleCircle(Turtle *turtle, float radius);
void turtleClear(Turtle *turtle);
void turtleHome(Turtle *turtle);
void turtleDraw(Turtle *turtle);
void turtleUpdateDisplay(Turtle *turtle);

TurtleApp *turtleAppCreate(unsigned int width, unsigned int height, const char *title);
Turtle *turtleAppGetTurtle(TurtleApp *app);
void turtleAppRun(TurtleApp *app);
void turtleAppDestroy(TurtleApp *app);

void turtleRun(TurtleDrawFunc drawFunc, unsigned int width, unsigned int height, const char *title);

#ifdef __cplusplus
}
#endif

#endif
