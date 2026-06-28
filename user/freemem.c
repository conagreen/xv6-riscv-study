#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int bytes = freemem();
  printf("free memory: %d bytes (%d KB, %d pages)\n",
         bytes, bytes / 1024, bytes / 4096);
  exit(0);
}