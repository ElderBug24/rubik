#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"


#define W 800
#define H 600
#define M 200

#define ACCEL 40.0f

int main(void) {
  InitWindow(W, H, "Input example");

  SetTargetFPS(60);

  Vector2 X = {0};

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    Vector2 dir = {0};
    if (IsKeyDown(KEY_RIGHT)) dir.x += 1;
    if (IsKeyDown(KEY_LEFT)) dir.x -= 1;
    if (IsKeyDown(KEY_UP)) dir.y -= 1;
    if (IsKeyDown(KEY_DOWN)) dir.y += 1;
    dir = Vector2Normalize(dir);

    Vector2 target = Vector2Scale(dir, M);
    Vector2 delta = Vector2Subtract(target, X);
    X = Vector2Add(X, Vector2Scale(delta, 1.0f - expf(-ACCEL * dt)));

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawLineDashed((Vector2) { W/2 - M, H/2 - M }, (Vector2) { W/2 - M, H/2 + M }, 10, 5, BLACK);
    DrawLineDashed((Vector2) { W/2 + M, H/2 - M }, (Vector2) { W/2 + M, H/2 + M }, 10, 5, BLACK);
    DrawLineDashed((Vector2) { W/2 - M, H/2 - M }, (Vector2) { W/2 + M, H/2 - M }, 10, 5, BLACK);
    DrawLineDashed((Vector2) { W/2 - M, H/2 + M }, (Vector2) { W/2 + M, H/2 + M }, 10, 5, BLACK);
    DrawCircleLines(W/2, H/2, M, BLACK);
    DrawLine(W/2, H/2, W/2 + X.x, H/2 + X.y, BLACK);
    DrawCircle(W/2 + X.x, H/2 + X.y, 10, RED);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

