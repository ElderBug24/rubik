#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"


#define LOOKM PI/4
#define LOOKACCEL 20.0f
#define ANIMATION_TIME 0.3f

const static Vector3 CUBEVERTICES[8] = {
  (Vector3) {  0.5,  0.5,  0.5 },
  (Vector3) {  0.5,  0.5, -0.5 },
  (Vector3) { -0.5,  0.5, -0.5 },
  (Vector3) { -0.5,  0.5,  0.5 },
  (Vector3) {  0.5, -0.5,  0.5 },
  (Vector3) {  0.5, -0.5, -0.5 },
  (Vector3) { -0.5, -0.5, -0.5 },
  (Vector3) { -0.5, -0.5,  0.5 }
};

const static uint8_t CUBEFACES[12][3] = {
  { 0, 3, 4 }, // RED
  { 4, 3, 7 },
  { 0, 1, 2 }, // WHITE
  { 0, 2, 3 },
  { 0, 5, 1 }, // BLUE
  { 0, 4, 5 },
  { 3, 2, 6 }, // GREEN
  { 3, 6, 7 },
  { 4, 7, 5 }, // YELLOW
  { 7, 6, 5 },
  { 1, 5, 6 }, // ORANGE
  { 1, 6, 2 },
};

typedef enum {
  CUBEMOVE_NONE = 0,
  CUBEMOVE_R, CUBEMOVE_R_PRIME,
  CUBEMOVE_L, CUBEMOVE_L_PRIME,
  CUBEMOVE_U, CUBEMOVE_U_PRIME,
  CUBEMOVE_D, CUBEMOVE_D_PRIME,
  CUBEMOVE_F, CUBEMOVE_F_PRIME,
  CUBEMOVE_B, CUBEMOVE_B_PRIME,
  CUBEMOVE_S, CUBEMOVE_S_PRIME,
  CUBEMOVE_M, CUBEMOVE_M_PRIME,
  CUBEMOVE_E, CUBEMOVE_E_PRIME,
  CUBEMOVE_X, CUBEMOVE_X_PRIME,
  CUBEMOVE_Y, CUBEMOVE_Y_PRIME,
  CUBEMOVE_Z, CUBEMOVE_Z_PRIME
} cubemove_e;

typedef struct {
  Color faces[6];
  Vector3 pos;
  Matrix rot;
} subcube_t;

typedef subcube_t cube_rig_t[3][3][3];

static const Matrix MATRIX_IDENTITY = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

const static cube_rig_t SOLVED_CUBE = {
  {
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          GREEN,
          YELLOW,
          ORANGE
        },
        .pos = { -1, -1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          GREEN,
          YELLOW,
          BLACK
        },
        .pos = { -1, -1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLACK,
          GREEN,
          YELLOW,
          BLACK
        },
        .pos = { -1, -1,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          GREEN,
          BLACK,
          ORANGE
        },
        .pos = { -1,  0, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          GREEN,
          BLACK,
          BLACK
        },
        .pos = { -1,  0,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLACK,
          GREEN,
          BLACK,
          BLACK
        },
        .pos = { -1,  0,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLACK,
          GREEN,
          BLACK,
          ORANGE
        },
        .pos = { -1,  1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLACK,
          GREEN,
          BLACK,
          BLACK
        },
        .pos = { -1,  1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          WHITE,
          BLACK,
          GREEN,
          BLACK,
          BLACK
        },
        .pos = { -1,  1,  1 },
        .rot = MATRIX_IDENTITY
      }
    }
  },
  {
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          YELLOW,
          ORANGE
        },
        .pos = {  0, -1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          YELLOW,
          BLACK
        },
        .pos = {  0, -1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLACK,
          BLACK,
          YELLOW,
          BLACK
        },
        .pos = {  0, -1,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          ORANGE
        },
        .pos = {  0,  0, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  0,  0,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLACK,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  0,  0,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLACK,
          BLACK,
          BLACK,
          ORANGE
        },
        .pos = {  0,  1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLACK,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  0,  1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          WHITE,
          BLACK,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  0,  1,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
  },
  {
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLUE,
          BLACK,
          YELLOW,
          ORANGE
        },
        .pos = {  1, -1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLUE,
          BLACK,
          YELLOW,
          BLACK
        },
        .pos = {  1, -1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLUE,
          BLACK,
          YELLOW,
          BLACK
        },
        .pos = {  1, -1,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLUE,
          BLACK,
          BLACK,
          ORANGE
        },
        .pos = {  1,  0, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          BLACK,
          BLUE,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  1,  0,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          BLACK,
          BLUE,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  1,  0,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
    {
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLUE,
          BLACK,
          BLACK,
          ORANGE
        },
        .pos = {  1,  1, -1 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          BLACK,
          WHITE,
          BLUE,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  1,  1,  0 },
        .rot = MATRIX_IDENTITY
      },
      (subcube_t) {
        .faces = {
          RED,
          WHITE,
          BLUE,
          BLACK,
          BLACK,
          BLACK
        },
        .pos = {  1,  1,  1 },
        .rot = MATRIX_IDENTITY
      }
    },
  },
};

void draw_subcube(subcube_t subcube, Vector3 added_rot_) {
  Matrix added_rot = QuaternionToMatrix(QuaternionFromEuler(added_rot_.x, added_rot_.y, added_rot_.z));
  Matrix rot = subcube.rot;

  Vector3 rotated_cube_vertices[8] = {
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[0], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[1], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[2], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[3], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[4], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[5], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[6], rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(CUBEVERTICES[7], rot)), added_rot)
  };

  for (size_t i = 0; i < 6; ++i) {
    DrawTriangle3D(
      rotated_cube_vertices[CUBEFACES[i * 2    ][0]],
      rotated_cube_vertices[CUBEFACES[i * 2    ][1]],
      rotated_cube_vertices[CUBEFACES[i * 2    ][2]],
      subcube.faces[i]
    );

    DrawTriangle3D(
      rotated_cube_vertices[CUBEFACES[i * 2 + 1][0]],
      rotated_cube_vertices[CUBEFACES[i * 2 + 1][1]],
      rotated_cube_vertices[CUBEFACES[i * 2 + 1][2]],
      subcube.faces[i]
    );
  }

  Vector3 rotated_cube_vertices_p[8] = {
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[0], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[1], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[2], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[3], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[4], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[5], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[6], 1.004), rot)), added_rot),
    Vector3Transform(Vector3Add(subcube.pos, Vector3Transform(Vector3Scale(CUBEVERTICES[7], 1.004), rot)), added_rot)
  };

  DrawLine3D(rotated_cube_vertices_p[0], rotated_cube_vertices[1], BLACK);
  DrawLine3D(rotated_cube_vertices_p[1], rotated_cube_vertices[2], BLACK);
  DrawLine3D(rotated_cube_vertices_p[2], rotated_cube_vertices[3], BLACK);
  DrawLine3D(rotated_cube_vertices_p[3], rotated_cube_vertices[0], BLACK);
  DrawLine3D(rotated_cube_vertices_p[0], rotated_cube_vertices[4], BLACK);
  DrawLine3D(rotated_cube_vertices_p[1], rotated_cube_vertices[5], BLACK);
  DrawLine3D(rotated_cube_vertices_p[2], rotated_cube_vertices[6], BLACK);
  DrawLine3D(rotated_cube_vertices_p[3], rotated_cube_vertices[7], BLACK);
  DrawLine3D(rotated_cube_vertices_p[4], rotated_cube_vertices[5], BLACK);
  DrawLine3D(rotated_cube_vertices_p[5], rotated_cube_vertices[6], BLACK);
  DrawLine3D(rotated_cube_vertices_p[6], rotated_cube_vertices[7], BLACK);
  DrawLine3D(rotated_cube_vertices_p[7], rotated_cube_vertices[4], BLACK);

  DrawLine3D(rotated_cube_vertices_p[0], rotated_cube_vertices_p[1], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[1], rotated_cube_vertices_p[2], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[2], rotated_cube_vertices_p[3], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[3], rotated_cube_vertices_p[0], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[0], rotated_cube_vertices_p[4], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[1], rotated_cube_vertices_p[5], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[2], rotated_cube_vertices_p[6], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[3], rotated_cube_vertices_p[7], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[4], rotated_cube_vertices_p[5], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[5], rotated_cube_vertices_p[6], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[6], rotated_cube_vertices_p[7], ColorAlpha(BLACK, 0.5));
  DrawLine3D(rotated_cube_vertices_p[7], rotated_cube_vertices_p[4], ColorAlpha(BLACK, 0.5));
}

void draw_cube(cube_rig_t cube, Vector3 added_rot) {
  for (size_t x = 0; x < 3; ++x) {
    for (size_t y = 0; y < 3; ++y) {
      for (size_t z = 0; z < 3; ++z) {
        draw_subcube(cube[x][y][z], added_rot);
      }
    }
  }
}

void format_secs(float seconds, char* buf, size_t max) {
  int m = ((int)(seconds / 60.0f)) % 60;
  int s = ((int)seconds) % 60;
  int ms = (int)((seconds - (int)seconds) * 1000.0f);

  snprintf(buf, max, "%02d:%02d.%03d", m, s, ms);
}

float ParametricBlend(float t) {
  float sqr = t * t;
  return sqr / (2.0f * (sqr - t) + 1.0f);
}

int main(void) {
  InitWindow(800, 600, "Rubik's cube");
  Image logo = LoadImage("./rubik.png");
  SetWindowIcon(logo);

  Camera3D camera = {0};
  camera.position = (Vector3){ 0, 0, 8 };
  camera.up = (Vector3) { 0, 1, 0};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(60);

  Vector2 look = {0};

  cube_rig_t cube;
  memcpy(cube, SOLVED_CUBE, sizeof(cube_rig_t));

  struct {
    cubemove_e lastmove;
    float progress;
  } animation = {0};

  struct {
    float time;
    bool active;
  } timer = {0};

  bool animations = true;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    Vector2 look_dir = {0};
    if (IsKeyDown(KEY_RIGHT)) look_dir.x += 1;
    if (IsKeyDown(KEY_LEFT)) look_dir.x -= 1;
    if (IsKeyDown(KEY_UP)) look_dir.y -= 1;
    if (IsKeyDown(KEY_DOWN)) look_dir.y += 1;
    look_dir = Vector2Normalize(look_dir);

    cubemove_e move = CUBEMOVE_NONE;
    if (IsKeyPressed(KEY_R)) move = CUBEMOVE_R;
    if (IsKeyPressed(KEY_L)) move = CUBEMOVE_L;
    if (IsKeyPressed(KEY_U)) move = CUBEMOVE_U;
    if (IsKeyPressed(KEY_D)) move = CUBEMOVE_D;
    if (IsKeyPressed(KEY_F)) move = CUBEMOVE_F;
    if (IsKeyPressed(KEY_B)) move = CUBEMOVE_B;
    if (IsKeyPressed(KEY_S)) move = CUBEMOVE_S;
    if (IsKeyPressed(KEY_M)) move = CUBEMOVE_M;
    if (IsKeyPressed(KEY_E)) move = CUBEMOVE_E;
    if (IsKeyPressed(KEY_X)) move = CUBEMOVE_X;
    if (IsKeyPressed(KEY_Y)) move = CUBEMOVE_Y;
    if (IsKeyPressed(KEY_Z)) move = CUBEMOVE_Z;
    move += (move && IsKeyDown(KEY_LEFT_SHIFT));

    animations ^= IsKeyPressed(KEY_A);

    if (IsKeyDown(KEY_LEFT_ALT)) {
      look_dir = (Vector2) {0};
      if (IsKeyPressed(KEY_RIGHT)) move = CUBEMOVE_Y;
      if (IsKeyPressed(KEY_LEFT )) move = CUBEMOVE_Y_PRIME;
      if (IsKeyPressed(KEY_UP   )) move = CUBEMOVE_X_PRIME;
      if (IsKeyPressed(KEY_DOWN )) move = CUBEMOVE_X;
    }

    if (animations) {
      if (animation.lastmove == CUBEMOVE_NONE) {
        animation.lastmove = move;
        animation.progress = 0;
      } else move = CUBEMOVE_NONE; // cancel the input
    }

    if (IsKeyPressed(KEY_SPACE)) {
      if (IsKeyDown(KEY_LEFT_SHIFT)) {
        timer.time = 0;
        timer.active = false;
      } else {
        timer.active = !timer.active;
      }
    }

    Vector2 look_target = Vector2Scale(look_dir, LOOKM * (1 + IsKeyDown(KEY_LEFT_CONTROL) * 2));
    Vector2 look_delta = Vector2Subtract(look_target, look);
    look = Vector2Add(look, Vector2Scale(look_delta, 1.0f - expf(-LOOKACCEL * dt)));

    if (timer.active) timer.time += dt;

    cube_rig_t old;
    memcpy(old, cube, sizeof(cube_rig_t));

    size_t x, y, z;
    switch (move) {
      case CUBEMOVE_NONE:
        break;
      case CUBEMOVE_R:
        x = 2;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(-PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = subcube.pos.z;
            subcube.pos.z = -temp_y;
            cube[x][z][2 - y] = subcube;
          }
        }
        break;
      case CUBEMOVE_R_PRIME:
        x = 2;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = -subcube.pos.z;
            subcube.pos.z = temp_y;
            cube[x][2 - z][y] = subcube;
          }
        }
        break;
      case CUBEMOVE_L:
        x = 0;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = -subcube.pos.z;
            subcube.pos.z = temp_y;
            cube[x][2 - z][y] = subcube;
          }
        }
        break;
      case CUBEMOVE_L_PRIME:
        x = 0;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(-PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = subcube.pos.z;
            subcube.pos.z = -temp_y;
            cube[x][z][2 - y] = subcube;
          }
        }
        break;
      case CUBEMOVE_U:
        for (x = 0; x < 3; ++x) {
          y = 2;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.z;
            subcube.pos.z = temp_x;
            cube[2 - z][y][x] = subcube;
          }
        }
        break;
      case CUBEMOVE_U_PRIME:
        for (x = 0; x < 3; ++x) {
          y = 2;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.z;
            subcube.pos.z = -temp_x;
            cube[z][y][2 - x] = subcube;
          }
        }
        break;
      case CUBEMOVE_D:
        for (x = 0; x < 3; ++x) {
          y = 0;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.z;
            subcube.pos.z = -temp_x;
            cube[z][y][2 - x] = subcube;
          }
        }
        break;
      case CUBEMOVE_D_PRIME:
        for (x = 0; x < 3; ++x) {
          y = 0;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.z;
            subcube.pos.z = temp_x;
            cube[2 - z][y][x] = subcube;
          }
        }
        break;
      case CUBEMOVE_F:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 2;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.y;
            subcube.pos.y = -temp_x;
            cube[y][2 - x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_F_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 2;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.y;
            subcube.pos.y = temp_x;
            cube[2 - y][x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_B:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 0;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.y;
            subcube.pos.y = temp_x;
            cube[2 - y][x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_B_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 0;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.y;
            subcube.pos.y = -temp_x;
            cube[y][2 - x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_S:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 1;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.y;
            subcube.pos.y = -temp_x;
            cube[y][2 - x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_S_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            z = 1;
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.y;
            subcube.pos.y = temp_x;
            cube[2 - y][x][z] = subcube;
          }
        }
        break;
      case CUBEMOVE_M:
        x = 1;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = -subcube.pos.z;
            subcube.pos.z = temp_y;
            cube[x][2 - z][y] = subcube;
          }
        }
        break;
      case CUBEMOVE_M_PRIME:
        x = 1;
        for (y = 0; y < 3; ++y) {
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(-PI/2));
            float temp_y = subcube.pos.y;
            subcube.pos.y = subcube.pos.z;
            subcube.pos.z = -temp_y;
            cube[x][z][2 - y] = subcube;
          }
        }
        break;
      case CUBEMOVE_E:
        for (x = 0; x < 3; ++x) {
          y = 1;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = subcube.pos.z;
            subcube.pos.z = -temp_x;
            cube[z][y][2 - x] = subcube;
          }
        }
        break;
      case CUBEMOVE_E_PRIME:
        for (x = 0; x < 3; ++x) {
          y = 1;
          for (z = 0; z < 3; ++z) {
            subcube_t subcube = old[x][y][z];
            subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(-PI/2));
            float temp_x = subcube.pos.x;
            subcube.pos.x = -subcube.pos.z;
            subcube.pos.z = temp_x;
            cube[2 - z][y][x] = subcube;
          }
        }
        break;
      case CUBEMOVE_X:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(-PI/2));
              float temp_y = subcube.pos.y;
              subcube.pos.y = subcube.pos.z;
              subcube.pos.z = -temp_y;
              cube[x][z][2 - y] = subcube;
            }
          }
        }
        break;
      case CUBEMOVE_X_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateX(PI/2));
              float temp_y = subcube.pos.y;
              subcube.pos.y = -subcube.pos.z;
              subcube.pos.z = temp_y;
              cube[x][2 - z][y] = subcube;
            }
          }
        }
        break;
      case CUBEMOVE_Y:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(-PI/2));
              float temp_x = subcube.pos.x;
              subcube.pos.x = -subcube.pos.z;
              subcube.pos.z = temp_x;
              cube[2 - z][y][x] = subcube;
            }
          }
        }
        break;
      case CUBEMOVE_Y_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateY(PI/2));
              float temp_x = subcube.pos.x;
              subcube.pos.x = subcube.pos.z;
              subcube.pos.z = -temp_x;
              cube[z][y][2 - x] = subcube;
            }
          }
        }
        break;
      case CUBEMOVE_Z:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(-PI/2));
              float temp_x = subcube.pos.x;
              subcube.pos.x = subcube.pos.y;
              subcube.pos.y = -temp_x;
              cube[y][2 - x][z] = subcube;
            }
          }
        }
        break;
      case CUBEMOVE_Z_PRIME:
        for (x = 0; x < 3; ++x) {
          for (y = 0; y < 3; ++y) {
            for (z = 0; z < 3; ++z) {
              subcube_t subcube = old[x][y][z];
              subcube.rot = MatrixMultiply(subcube.rot, MatrixRotateZ(PI/2));
              float temp_x = subcube.pos.x;
              subcube.pos.x = -subcube.pos.y;
              subcube.pos.y = temp_x;
              cube[2 - y][x][z] = subcube;
            }
          }
        }
        break;
    }
    memcpy(old, cube, sizeof(cube_rig_t));

    BeginDrawing();
    ClearBackground((Color) { 72, 72, 72, 255 });

    BeginMode3D(camera);

    if (animation.lastmove != CUBEMOVE_NONE) {
      if ((animation.progress += dt) >= ANIMATION_TIME) {
        animation.lastmove = CUBEMOVE_NONE;
        draw_cube(cube, (Vector3) { .x = -look.y, .y = -look.x });
      } else {
        float t = animation.progress / ANIMATION_TIME;
        t = ParametricBlend(t);
        t -= 1;

        switch (animation.lastmove) {
          case CUBEMOVE_R:
            x = 2;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_R_PRIME:
            x = 2;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_L:
            x = 0;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_L_PRIME:
            x = 0;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_U:
            for (x = 0; x < 3; ++x) {
              y = 2;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_U_PRIME:
            for (x = 0; x < 3; ++x) {
              y = 2;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_D:
            for (x = 0; x < 3; ++x) {
              y = 0;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_D_PRIME:
            for (x = 0; x < 3; ++x) {
              y = 0;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_F:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 2;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_F_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 2;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_B:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 0;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_B_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 0;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_S:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 1;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_S_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                z = 1;
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateZ(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_M:
            x = 1;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_M_PRIME:
            x = 1;
            for (y = 0; y < 3; ++y) {
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateX(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_E:
            for (x = 0; x < 3; ++x) {
              y = 1;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_E_PRIME:
            for (x = 0; x < 3; ++x) {
              y = 1;
              for (z = 0; z < 3; ++z) {
                subcube_t subcube = old[x][y][z];
                Matrix rot = MatrixRotateY(-PI/2 * t);
                subcube.rot = MatrixMultiply(subcube.rot, rot);
                subcube.pos = Vector3Transform(subcube.pos, rot);
                old[x][y][z] = subcube;
              }
            }
            break;
          case CUBEMOVE_X:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateX(-PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
          case CUBEMOVE_X_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateX(PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
          case CUBEMOVE_Y:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateY(-PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
          case CUBEMOVE_Y_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateY(PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
          case CUBEMOVE_Z:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateZ(-PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
          case CUBEMOVE_Z_PRIME:
            for (x = 0; x < 3; ++x) {
              for (y = 0; y < 3; ++y) {
                for (z = 0; z < 3; ++z) {
                  subcube_t subcube = old[x][y][z];
                  Matrix rot = MatrixRotateZ(PI/2 * t);
                  subcube.rot = MatrixMultiply(subcube.rot, rot);
                  subcube.pos = Vector3Transform(subcube.pos, rot);
                  old[x][y][z] = subcube;
                }
              }
            }
            break;
        }
        draw_cube(old, (Vector3) { .x = -look.y, .y = -look.x });
      }
    } else {
      draw_cube(cube, (Vector3) { .x = -look.y, .y = -look.x });
    }

    EndMode3D();

    char buf[1024];
    format_secs(timer.time, buf, 1024);
    int fontSize = 30;
    DrawText(buf, GetScreenWidth()/2 - MeasureText(buf, fontSize)/2, GetScreenHeight() - fontSize - 10, fontSize, timer.active ? GREEN : RED);

    if (animations) DrawText("A", 10, GetScreenHeight() - 36, 36, RAYWHITE);

    DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();
  UnloadImage(logo);

  return 0;
}

