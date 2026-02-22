/*#include "types.h"
#include "stat.h"
#include "user.h"

static volatile int sink = 0;
static void burn(int n){ for(int i=0;i<n;i++) sink += i; }

int main(int argc, char **argv){
  int t = 10;
  if(argc == 2) t = atoi(argv[1]);

  if(settickets(0) == 0){
    printf(1, "FAIL: settickets(0) should fail\n");
    exit();
  }
  if(t >= 1 && settickets(t) != 0){
    printf(1, "FAIL: settickets(%d) should succeed\n", t);
    exit();
  }
  printf(1, "PASS: settickets validation\n");

  for(int k=0;k<200;k++) burn(200000);
  printf(1, "testlottery: done\n");
  exit();
}*/
#include "types.h"
#include "stat.h"
#include "user.h"

static volatile unsigned long long sink = 0;

static uint
work_for_ticks(int duration_ticks)
{
  int start = uptime();
  uint work = 0;

  while(uptime() - start < duration_ticks){
    // simple CPU work
    sink += work;
    work++;
  }
  return work;
}

int
main(int argc, char **argv)
{
  // Usage:
  //   testlottery                  (just syscall validation + default share test)
  //   testlottery D t1 t2 ... tn    (share test with duration D and tickets list)
  // Example:
  //   testlottery 2000 1 3
  //   testlottery 3000 10 20 70

  // ---- syscall validation ----
  if(settickets(0) == 0){
    printf(1, "FAIL: settickets(0) should fail\n");
    exit();
  }
  if(settickets(1) != 0){
    printf(1, "FAIL: settickets(1) should succeed\n");
    exit();
  }
  printf(1, "PASS: settickets validation\n");

  // ---- share experiment ----
  int duration = 2000; // default
  int first_ticket_arg = 1;

  if(argc >= 2){
    duration = atoi(argv[1]);
    first_ticket_arg = 2;
  }

  int nchildren = argc - first_ticket_arg;
  if(nchildren <= 0){
    // default experiment if no tickets provided
    static int default_tickets[] = {1, 3};
    nchildren = 2;

    for(int i = 0; i < nchildren; i++){
      int pid = fork();
      if(pid == 0){
        int t = default_tickets[i];
        if(settickets(t) < 0){
          printf(1, "child: settickets(%d) failed\n", t);
          exit();
        }
        unsigned long long w = work_for_ticks(duration);
        printf(1, "pid=%d tickets=%d work=%llu\n", getpid(), t, w);
        exit();
      }
    }
  } else {
    for(int i = 0; i < nchildren; i++){
      int t = atoi(argv[first_ticket_arg + i]);
      int pid = fork();
      if(pid == 0){
        if(settickets(t) < 0){
          printf(1, "child: settickets(%d) failed\n", t);
          exit();
        }
        uint w = work_for_ticks(duration);
        printf(1, "pid=%d tickets=%d work=%d\n", getpid(), t, w);
        exit();
      }
    }
  }

  for(int i = 0; i < nchildren; i++)
    wait();

  printf(1, "testlottery: done\n");
  exit();
}