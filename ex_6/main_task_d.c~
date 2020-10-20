#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>
#include <sched.h>

#include "io.h"
#include "timespec.h"

#define NUM_THREADS 3
#define NUM_DISTURB 5

#define CORE_NUM 1

struct timespec waketime;


int set_cpu(int cpu_number){
    cpu_set_t cpu;

    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* func_resp(void* vargp){
    set_cpu(CORE_NUM);

    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec = 0, .tv_nsec = 1 * 1000 * 1000};

    // do peridic work
    while(1){
        if (!io_read((int)vargp)) {
            io_write((int)vargp, 0);
            io_write((int)vargp, 1);
        }
        //sleep
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }

}

void* disturb(void* vargp){
    set_cpu(CORE_NUM);
    while (1) {
        asm volatile("" ::: "memory");
    }
}


int main() {
    io_init();

    pthread_t f_threads[NUM_THREADS];
    pthread_t d_threads[NUM_DISTURB];

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_create(&f_threads[i], NULL, func_resp, (void *)(i+1));
    }

    for (int i = 0; i < NUM_DISTURB; i++){
        pthread_create(&d_threads[i], NULL, disturb, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(f_threads[i], NULL);
    }

    for (int i = 0; i < NUM_DISTURB; i++){
        pthread_join(d_threads[i], NULL);
    }

    return 0;
}
