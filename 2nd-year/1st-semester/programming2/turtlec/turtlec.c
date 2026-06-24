#include "turtlec.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static const float PI = 3.14159265359f;

static void turtleMoveTo(Turtle *turtle, sfVector2f newPos, bool drawLine){
  if(drawLine){
    sfVertex a;
    sfVertex b;

    a.position = turtle->pos;
    a.color = turtle->penColor;
    a.texCoords = (sfVector2f){0.0f, 0.0f};

    b.position = newPos;
    b.color = turtle->penColor;
    b.texCoords = (sfVector2f){0.0f, 0.0f};

    sfVertexArray_append(turtle->lines, a);
    sfVertexArray_append(turtle->lines, b);
  }

  turtle->pos = newPos;
  sfConvexShape_setPosition(turtle->shape, turtle->pos);
}

static bool turtleProcessEvents(Turtle *turtle){
  sfEvent event;

  while(sfRenderWindow_pollEvent(turtle->window, &event)){
    if(event.type == sfEvtClosed){
      sfRenderWindow_close(turtle->window);
      return false;
    }
  }

  return sfRenderWindow_isOpen(turtle->window) == sfTrue;
}

void turtleInit(Turtle *turtle, sfRenderWindow *window, unsigned int width, unsigned int height){
  turtle->window = window;
  turtle->lines = sfVertexArray_create();
  turtle->shape = sfConvexShape_create();
  turtle->pos = (sfVector2f){width / 2.0f, height / 2.0f};
  turtle->heading = 0.0f;
  turtle->penIsDown = true;
  turtle->penColor = sfWhite;
  turtle->speed = 5.0f;
  turtle->width = width;
  turtle->height = height;

  sfVertexArray_setPrimitiveType(turtle->lines, sfLines);

  sfConvexShape_setPointCount(turtle->shape, 3);
  sfConvexShape_setPoint(turtle->shape, 0, (sfVector2f){0.0f, -5.0f});
  sfConvexShape_setPoint(turtle->shape, 1, (sfVector2f){10.0f, 0.0f});
  sfConvexShape_setPoint(turtle->shape, 2, (sfVector2f){0.0f, 5.0f});
  sfConvexShape_setFillColor(turtle->shape, sfGreen);
  sfConvexShape_setPosition(turtle->shape, turtle->pos);
  sfConvexShape_setRotation(turtle->shape, turtle->heading);
}

void turtleDestroy(Turtle *turtle){
  if(turtle->shape != NULL)
    sfConvexShape_destroy(turtle->shape);
  if(turtle->lines != NULL)
    sfVertexArray_destroy(turtle->lines);

  turtle->shape = NULL;
  turtle->lines = NULL;
  turtle->window = NULL;
}

void turtleForward(Turtle *turtle, float dist){
  float rad = turtle->heading * PI / 180.0f;
  float remaining = fabsf(dist);
  float direction = dist >= 0.0f ? 1.0f : -1.0f;
  float stepLength = turtle->speed;

  if(stepLength <= 0.0f)
    stepLength = 1.0f;

  while(remaining > 0.0f && sfRenderWindow_isOpen(turtle->window)){
    float currentStep = remaining < stepLength ? remaining : stepLength;
    sfVector2f newPos = {
      turtle->pos.x + cosf(rad) * currentStep * direction,
      turtle->pos.y + sinf(rad) * currentStep * direction
    };

    turtleMoveTo(turtle, newPos, turtle->penIsDown);
    turtleUpdateDisplay(turtle);
    remaining -= currentStep;
  }
}

void turtleBackward(Turtle *turtle, float dist){
  turtleForward(turtle, -dist);
}

void turtleRight(Turtle *turtle, float angle){
  turtle->heading += angle;
  sfConvexShape_setRotation(turtle->shape, turtle->heading);
  turtleUpdateDisplay(turtle);
}

void turtleLeft(Turtle *turtle, float angle){
  turtle->heading -= angle;
  sfConvexShape_setRotation(turtle->shape, turtle->heading);
  turtleUpdateDisplay(turtle);
}

void turtlePenUp(Turtle *turtle){
  turtle->penIsDown = false;
}

void turtlePenDown(Turtle *turtle){
  turtle->penIsDown = true;
}

void turtleSetColor(Turtle *turtle, uint8_t r, uint8_t g, uint8_t b){
  turtle->penColor = sfColor_fromRGB(r, g, b);
  sfConvexShape_setFillColor(turtle->shape, turtle->penColor);
  turtleUpdateDisplay(turtle);
}

void turtleGoTo(Turtle *turtle, float x, float y){
  sfVector2f newPos = {x, y};
  turtleMoveTo(turtle, newPos, turtle->penIsDown);
  turtleUpdateDisplay(turtle);
}

void turtleSetSpeed(Turtle *turtle, float speed){
  if(speed <= 0.0f)
    speed = 1.0f;
  turtle->speed = speed;
}

void turtleCircle(Turtle *turtle, float radius){
  const int steps = 36;
  float stepAngle = 360.0f / steps;
  float stepLength = 2.0f * PI * radius / steps;

  for(int i = 0; i < steps && sfRenderWindow_isOpen(turtle->window); i++){
    turtleRight(turtle, stepAngle);
    turtleForward(turtle, stepLength);
  }
}

void turtleClear(Turtle *turtle){
  sfVertexArray_clear(turtle->lines);
  turtleUpdateDisplay(turtle);
}

void turtleHome(Turtle *turtle){
  turtle->pos = (sfVector2f){turtle->width / 2.0f, turtle->height / 2.0f};
  turtle->heading = 0.0f;
  sfConvexShape_setPosition(turtle->shape, turtle->pos);
  sfConvexShape_setRotation(turtle->shape, turtle->heading);
  turtleUpdateDisplay(turtle);
}

void turtleDraw(Turtle *turtle){
  sfRenderWindow_drawVertexArray(turtle->window, turtle->lines, NULL);
  sfRenderWindow_drawConvexShape(turtle->window, turtle->shape, NULL);
}

void turtleUpdateDisplay(Turtle *turtle){
  if(!turtleProcessEvents(turtle))
    return;

  sfRenderWindow_clear(turtle->window, sfBlack);
  turtleDraw(turtle);
  sfRenderWindow_display(turtle->window);

  sfSleep(sfMilliseconds(16));
}

TurtleApp *turtleAppCreate(unsigned int width, unsigned int height, const char *title){
  TurtleApp *app = malloc(sizeof(TurtleApp));
  sfVideoMode mode = {width, height, 32};

  if(app == NULL)
    return NULL;

  app->window = sfRenderWindow_create(mode, title, sfResize | sfClose, NULL);
  if(app->window == NULL){
    free(app);
    return NULL;
  }

  turtleInit(&app->turtle, app->window, width, height);
  return app;
}

Turtle *turtleAppGetTurtle(TurtleApp *app){
  if(app == NULL)
    return NULL;

  return &app->turtle;
}

void turtleAppRun(TurtleApp *app){
  if(app == NULL || app->window == NULL)
    return;

  while(sfRenderWindow_isOpen(app->window)){
    sfEvent event;

    while(sfRenderWindow_pollEvent(app->window, &event)){
      if(event.type == sfEvtClosed)
        sfRenderWindow_close(app->window);
    }

    sfRenderWindow_clear(app->window, sfBlack);
    turtleDraw(&app->turtle);
    sfRenderWindow_display(app->window);
  }
}

void turtleAppDestroy(TurtleApp *app){
  if(app == NULL)
    return;

  turtleDestroy(&app->turtle);
  if(app->window != NULL)
    sfRenderWindow_destroy(app->window);

  free(app);
}

void turtleRun(TurtleDrawFunc drawFunc, unsigned int width, unsigned int height, const char *title){
  TurtleApp *app = turtleAppCreate(width, height, title);

  if(app == NULL)
    return;

  drawFunc(turtleAppGetTurtle(app));
  turtleAppRun(app);
  turtleAppDestroy(app);
}
