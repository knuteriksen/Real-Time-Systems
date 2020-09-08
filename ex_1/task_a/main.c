#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include "timespec.h"
#include <sys/times.h>



void busy_wait_get_time(struct timespec t){
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  struct timespec then = timespec_add(now, t);
  while(timespec_cmp(now, then) < 0){
    for(int i = 0; i < 10000; i++){}
    clock_gettime(CLOCK_MONOTONIC, &now);
  }
}

void busy_wait_times(int t){
  static clock_t st_time;
  static clock_t en_time;
  static struct tms st_cpu;
  static struct tms en_cpu;

  st_time = times(&st_cpu);
  en_time = times(&en_cpu);

  while(en_time - st_time < t){
    en_time = times(&en_cpu);
  }
}

int main(int argc, char* argv[]){

  if(strcmp(argv[1], "-sleep") == 0 ){
    sleep(1);
  }

  else if(strcmp(argv[1], "-get_time") == 0){

    struct timespec one_sec_get_time = {
      .tv_sec = 1,
      .tv_nsec = 0
    };

    busy_wait_get_time(one_sec_get_time);
  }

  else if(strcmp(argv[1], "-times") == 0){
    int one_sec_times = sysconf(_SC_CLK_TCK);
    busy_wait_times(one_sec_times);
  }

  return 0;
}
