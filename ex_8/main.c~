#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
#include <rtdk.h>

#define CORE_NUM 1


RT_MUTEX mutexA;
RT_MUTEX mutexB;

void busy_wait_us(unsigned long delay){
	for(; delay > 0; delay--){
		rt_timer_spin(1000);
	}
}


//functions to be executed by tasks

void low_func(void *arg){
	unsigned long duration = 30000000000;
	unsigned long endTime = rt_timer_read() + duration;
	rt_printf("Task %s start\n", arg);

	// Take mutex A		
	rt_printf("Task %s Waiting for Mutex A\n", arg);
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_printf("Task %s got Mutex A\n", arg);

	// Busy wait 3 time units
	rt_printf("Task %s busy wait 1\n", arg);	
	busy_wait_us(3000);

	// Take mutex B
	rt_printf("Task %s Waiting for Mutex B\n", arg);
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_printf("Task %s got Mutex B\n", arg);

	// Busy wait 3 time units
	rt_printf("Task %s busy wait 2\n", arg);	
	busy_wait_us(3000);
	
	// Release Mutex B then A
	rt_mutex_release(&mutexB);
	rt_mutex_release(&mutexA);
	
	// Busy wait 1 time unit
	rt_printf("Task %s busy wait 3\n", arg);	
	busy_wait_us(1000);

	rt_printf("Task %s done\n", arg);

	if (rt_timer_read() > endTime){
		rt_printf("Time expired\n");
		rt_task_delete(NULL);		
	}
	if (rt_task_yield()){
		rt_printf("No yield\n");
		rt_task_delete(NULL);
	}

	return;
}

void high_func(void *arg){
	unsigned long duration = 30000000000;
	unsigned long endTime = rt_timer_read() + duration;

	rt_printf("Task %s start\n", arg);

	// Sleep 1 time unit
	rt_printf("Task %s sleep\n", arg);	
	rt_task_sleep(1*10e6);

	// Take Mutex B	
	rt_printf("Task %s Waiting for Mutex B\n", arg);
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_printf("Task %s got Mutex B\n", arg);

	// Busy wait 2 Time unit
	rt_printf("Task %s busy wait 1\n", arg);	
	busy_wait_us(2000);

	// Take Mutex A
	rt_printf("Task %s Waiting for Mutex A\n", arg);
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_printf("Task %s got Mutex A\n", arg);

	// Busy wait 2 time units
	rt_printf("Task %s busy wait 2\n", arg);	
	busy_wait_us(2000);

	//Release mutex A then B
	rt_mutex_release(&mutexA);
	rt_mutex_release(&mutexB);

	// Busy wait 1 time unit
	rt_printf("Task %s busy wait 3\n", arg);	
	busy_wait_us(1000);

	rt_printf("Task %s done\n", arg);	

	if (rt_timer_read() > endTime){
		rt_printf("Time expired\n");
		rt_task_delete(NULL);		
	}
	if (rt_task_yield()){
		rt_printf("No yield\n");
		rt_task_delete(NULL);
	}

	return;
}


int main(int argc, char* argv[]){
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_print_auto_init(1);
	
	// Create mutex
	rt_mutex_create(&mutexA, "Mutex_A");
	rt_mutex_create(&mutexB, "Mutex_B");
	
	// Create tasks
	RT_TASK task_H;	
	RT_TASK task_L;	

	rt_task_create(&task_H, NULL, 0, 70, T_CPU(CORE_NUM));		
	rt_task_create(&task_L, NULL, 0, 50, T_CPU(CORE_NUM));

	// Start tasks
	rt_task_start(&task_H, high_func, (void*)("High"));
	rt_task_start(&task_L, low_func, (void*)("Low"));


	// Delete Mutex
	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);

	while(1);
	
	return 0;
}

