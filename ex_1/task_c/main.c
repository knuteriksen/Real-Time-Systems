#include <sched.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>
#include <sys/sysinfo.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>
#include "timespec.h"

int main(){

  int N = 10000000;
  int ns_max = 500;
  int histogram[ns_max];
  memset(histogram, 0, sizeof(int)*ns_max);

  for(int i = 0; i < 10*1000*1000; i++){
    unsigned long t1 = __rdtsc();
    sched_yield();
    unsigned long t2 = __rdtsc();
    int ns = (int)((t2 - t1) * 1000.0 / 2659.829);
    if(ns >= 0 && ns < ns_max){
      histogram[ns]++;
    }
  }for(int i = 0; i < ns_max; i++){
    printf("%d\n", histogram[i]);
  }
}
