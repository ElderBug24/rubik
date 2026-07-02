#define NOB_IMPLEMENTATION
#include "../nob.h"


int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};

  nob_mkdir_if_not_exists("build");
  nob_cmd_append(&cmd, "gcc", "./cube.c", "-o", "build/main", "-I../raylib/raylib-6.0_win64_mingw-w64/include", "-L../raylib/raylib-6.0_win64_mingw-w64/lib", "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm");
  if (!nob_cmd_run(&cmd)) return 1;
  nob_cmd_append(&cmd, "build/main");
  if (!nob_cmd_run(&cmd)) return 1;

  return 0;
}

