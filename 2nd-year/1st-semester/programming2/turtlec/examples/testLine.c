#include "../turtlec.h"

void drawTriangle(Turtle *turtle, int length) {
  for (int i = 0; i < 3; i++) {
    turtleForward(turtle, length);
    turtleLeft(turtle, 120);
  }
}

// Triángulo de Sierpinski
void TriangleRec(Turtle *turtle, int length, int depth) {
  if (depth == 1) {
    drawTriangle(turtle, length);
    return;
  }

  TriangleRec(turtle, length / 2, depth - 1);

  turtleRight(turtle, 120);
  turtleBackward(turtle, length / 2);
  turtleLeft(turtle, 120);

  TriangleRec(turtle, length / 2, depth - 1);

  turtleForward(turtle, length / 2);
  turtleRight(turtle, 120);
  turtleForward(turtle, length / 2);
  turtleLeft(turtle, 120);

  TriangleRec(turtle, length / 2, depth - 1);

  turtleBackward(turtle, length / 2);
}

int main(void) {
  TurtleApp *app = turtleAppCreate(400, 200, "Test Line");

  if (app == NULL)
    return 1;

  Turtle *t = turtleAppGetTurtle(app);

  // Preparation
  turtlePenUp(t);
  turtleGoTo(t, 50, 190);
  turtlePenDown(t);

  TriangleRec(t, 200, 6);

  turtleAppRun(app);
  turtleAppDestroy(app);
  return 0;
}
