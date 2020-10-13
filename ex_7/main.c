#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <native/task.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>

#include "io.h"

#define NUM_TASK 3
#define NUM_DISTURB 10
#define CORE_NUM 1


RTIME period_ns = 1000000; // 1 ms
RTIME pin_wait_us = 5000; // 5 us

const char* name = "PERIODIC TASK";


int set_cpu(int cpu_number){
    cpu_set_t cpu;

    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

//function to be executed by task

void periodic_func(void *arg){
	set_cpu(CORE_NUM);
	unsigned long duration = 10000000000;
	unsigned long endTime = rt_timer_read() + duration;

	// rt_task_set_periodic(NULL, TM_NOW, period_ns);

	while(1){
		if (!io_read((int)arg)){
			io_write((int)arg, 0);
			rt_timer_spin(pin_wait_us);
			io_write((int)arg, 1);
		}
		if (rt_timer_read() > endTime){
			rt_printf("Time expired\n");
			rt_task_delete(NULL);		
		}
		if (rt_task_yield()){
			rt_printf("No yield\n");
			rt_task_delete(NULL);
		}
		rt_task_wait_period(NULL);
	}
	return;
}

void* disturb(void* vargp){
    set_cpu(CORE_NUM);
    while (1) {
        asm volatile("" ::: "memory");
    }
}


int main(int argc, char* argv[]){
	io_init();

	mlockall(MCL_CURRENT|MCL_FUTURE);

	RT_TASK periodic_tasks[NUM_TASK];
//	pthread_t d_threads[NUM_DISTURB];
	
	for (int i = 0; i < NUM_TASK; i++){
		rt_task_create(&periodic_tasks[i], name, 0, 50, 0);
	}
/*
	for (int i = 0; i < NUM_DISTURB; i++){
        pthread_create(&d_threads[i], NULL, disturb, NULL);
    }
*/	
	for (int i = 0; i < NUM_TASK; i++){
		rt_task_start(&periodic_tasks[i], &periodic_func, i+1);
	}
/*
	for (int i = 0; i < NUM_DISTURB; i++){
        pthread_join(d_threads[i], NULL);
    }	
*/
	while(1);

	return 0;
}


















