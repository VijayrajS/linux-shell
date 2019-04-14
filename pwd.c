#include "shell.h"

void pwd()
{
  char path[10000];
  getcwd(path,sizeof(path));
  printf("%s\n", path);
}
