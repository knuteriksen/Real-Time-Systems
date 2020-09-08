#include <string.h>
#include <time.h>
#include <x86intrin.h>
#include <sys/sysinfo.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>

#include "timespec.h"


int main(int argc, char* argv[]){


  int N = 10000000;
  int ns_max = 50;
  int histogram[ns_max];
  memset(histogram, 0, sizeof(int)*ns_max);



  if(strcmp(argv[1],"-rdtsc") == 0){

    for(int i = 0; i < N; i++){
      __rdtsc();
    }
  }

  if(strcmp(argv[1],"get_time") == 0){

    for(int i = 0; i < N; i++){
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
    }
  }

  if(strcmp(argv[1],"-times") == 0){
    static struct tms st_cpu;

    for(int i = 0; i < N; i++){
      times(&st_cpu);
    }
  }

  if(strcmp(argv[1],"-res_rdtsc") == 0){
    for(int i = 0; i < 10*1000*1000; i++){
      unsigned long t1 = __rdtsc();
      unsigned long t2 = __rdtsc();
      int ns = (int)((t2 - t1) * 1000.0 / 2659.829);
      if(ns >= 0 && ns < ns_max){
        histogram[ns]++;
      }
    }for(int i = 0; i < ns_max; i++){
      printf("%d\n", histogram[i]);
    }
  }
  if(strcmp(argv[1],"-res_get_time") == 0){
    struct timespec t1;
    struct timespec t2;
    for(int i = 0; i < 10*1000*1000; i++){

      clock_gettime(CLOCK_MONOTONIC, &t1);
      clock_gettime(CLOCK_MONOTONIC, &t2);
      int ns = (int)((t2.tv_nsec - t1.tv_nsec));
      if(ns >= 0 && ns < ns_max){
        histogram[ns]++;
      }
    }for(int i = 0; i < ns_max; i++){
      printf("%d\n", histogram[i]);
    }
  }
  if(strcmp(argv[1],"-res_times") == 0){
    int one_sec_times = sysconf(_SC_CLK_TCK);
    clock_t t1;
    clock_t t2;
    struct tms cpu_1;
    struct tms cpu_2;
    for(int i = 0; i < 10*1000*1000; i++){
      t1 =  times(&cpu_1);
      t2 =  times(&cpu_2);
      int ns = (int)((double)(t2 - t1) * 1e9 / one_sec_times);
      if(ns >= 0 && ns < ns_max){
        histogram[ns]++;
      }
    }for(int i = 0; i < ns_max; i++){
      printf("%d\n", histogram[i]);
    }
  }

  return 0;
}
