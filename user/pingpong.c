#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DEFAULT_N 100

int
main(int argc, char *argv[])
{
  int p1[2];  // parent -> child
  int p2[2];  // child  -> parent
  char buf = 'x';
  int n = DEFAULT_N;

  if (argc >= 2)
    n = atoi(argv[1]);
  if (n <= 0) {
    fprintf(2, "usage: pingpong [N]\n");
    exit(1);
  }

  if (pipe(p1) < 0 || pipe(p2) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // child: read from p1, write to p2
    close(p1[1]);
    close(p2[0]);

    for (int i = 0; i < n; i++) {
      if (read(p1[0], &buf, 1) != 1) {
        fprintf(2, "child: read failed at %d\n", i);
        exit(1);
      }
      if (i == 0)
        printf("%d: received ping\n", getpid());
      if (write(p2[1], &buf, 1) != 1) {
        fprintf(2, "child: write failed at %d\n", i);
        exit(1);
      }
    }

    close(p1[0]);
    close(p2[1]);
    exit(0);
  } else {
    // parent: write to p1, read from p2
    close(p1[0]);
    close(p2[1]);

    int t0 = uptime();
    for (int i = 0; i < n; i++) {
      if (write(p1[1], &buf, 1) != 1) {
        fprintf(2, "parent: write failed at %d\n", i);
        exit(1);
      }
      if (read(p2[0], &buf, 1) != 1) {
        fprintf(2, "parent: read failed at %d\n", i);
        exit(1);
      }
      if (i == 0)
        printf("%d: received pong\n", getpid());
    }
    int t1 = uptime();

    close(p1[1]);
    close(p2[0]);
    wait(0);

    int ticks = t1 - t0;
    // 1 tick ~= 10 ms  =>  exchanges/sec ~= n * 100 / ticks
    if (ticks <= 0)
      printf("%d exchanges in <1 tick (too fast to measure, try larger N)\n", n);
    else
      printf("%d exchanges in %d ticks (~ %d exchanges/sec)\n",
             n, ticks, n * 100 / ticks);

    exit(0);
  }
}