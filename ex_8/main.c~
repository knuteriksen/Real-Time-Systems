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

RT_SEM sem;

typedef struct {
	RT_MUTEX mut;
	int priority;
	int isA;
} Resource;

typedef struct {
	RT_TASK task;
	int base_priority;
	int cur_priority;
	int ownsA;
	int ownsB;
}PRI_Task;

Resource res_a;
Resource res_b;

PRI_Task task_low;
PRI_Task task_high;

int a_pri = 40;
int b_pri = 60;

void icpp_lock(Resource* res, PRI_Task* task){
	int new_priority = NULL;
	if (res->priority > task->cur_priority){
		new_priority = res->priority;
	}
	else {
		new_priority = task->cur_priority		;
	}
	
	rt_mutex_acquire(&(res->mut), TM_INFINITE);
	rt_task_set_priority(&(task->task), new_priority);

	if (res->isA){
		task->ownsA = 1;	
	}
	else {
		task->ownsB = 1;	
	}

	char s[] = "H locking B";
	
	if (task->base_priority == 1){
		s[0] = 'L';	
	}
	if (res->isA){
		s[10] = 'A';	
	}
	
	rt_printf("%s - New prio: %i\n", s, new_priority);
}


void icpp_unlock(Resource* res, PRI_Task* task){
	rt_mutex_release(&(res->mut));
	int new_priority = task->base_priority;

	if (res->isA && task->ownsB){
		new_priority = b_pri;	
	}
	if (!(res->isA) && task->ownsA){
		new_priority = a_pri;	
	}
	if (res->isA){
		task->ownsA = 0;	
	}
	else{
		task->ownsB = 0;	
	}
	char s[] = "H unlocking B";
	
	if (task->base_priority == 1){
		s[0] = 'L';	
	}
	if (res->isA){
		s[12] = 'A';	
	}
	
	rt_printf("%s - New prio: %i\n", s, new_priority);
	rt_task_set_priority(&(task->task), new_priority);
}


void busy_wait_us(unsigned long delay){
	for(; delay > 0; delay--){
		rt_timer_spin(1000);
	}
}


void low_func(void *arg){
	unsigned long duration = 30000000000;
	unsigned long endTime = rt_timer_read() + duration;

	rt_sem_p(&sem, TM_INFINITE);

	rt_printf("Task %s start\n", arg);

	// Take mutex A		
	icpp_lock(&res_a, &task_low);

	// Busy wait 3 time units
	busy_wait_us(30);

	// Take mutex B
	icpp_lock(&res_b, &task_low);

	// Busy wait 3 time units
	busy_wait_us(30);
	
	// Release Mutex B then A
	icpp_unlock(&res_b, &task_low);
	icpp_unlock(&res_a, &task_low);

	// Busy wait 1 time unit
	busy_wait_us(10);


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

	rt_sem_p(&sem, TM_INFINITE);

	rt_printf("Task %s start\n", arg);

	// Sleep 1 time unit
	rt_printf("Task %s start sleep\n", arg);	
	rt_task_sleep(10*1000);
	rt_printf("Task %s end sleep\n", arg);	

	// Take Mutex B	
	icpp_lock(&res_b, &task_high);

	// Busy wait 1 Time unit
	busy_wait_us(10);

	// Take Mutex A
	icpp_lock(&res_a, &task_high);

	// Busy wait 2 time units
	busy_wait_us(20);

	//Release mutex A then B
	icpp_unlock(&res_a, &task_high);
	icpp_unlock(&res_b, &task_high);

	// Busy wait 1 time unit
	busy_wait_us(10);

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

	rt_task_shadow(NULL, "MainTask", 80, T_CPU(CORE_NUM));

	// Create semaphore
	rt_sem_create(&sem, "CoolSemaphore", 0, S_PRIO);
	
	// Create mutex
	res_a.priority = a_pri;
	res_a.isA = 1;
	res_b.priority = b_pri;

	rt_mutex_create(&res_a.mut, "Mutex_A");
	rt_mutex_create(&res_b.mut, "Mutex_B");

	task_low.base_priority = 1;
	task_high.base_priority = 35;

	rt_task_create(&task_high.task, "HIGH", 0, 35, T_CPU(CORE_NUM));		
	rt_task_create(&task_low.task, "LOW", 0, 1, T_CPU(CORE_NUM));

	// Start tasks
	rt_task_start(&task_high.task, high_func, (void*)("High"));
	rt_task_start(&task_low.task, low_func, (void*)("Low"));

	// Sleep 100ms
	RTIME sleep_time = 100*1000*1000;
	rt_task_sleep(sleep_time);
	
	// Broadcast Semaphore
	rt_sem_broadcast(&sem);
	rt_printf("start test\n");
	
	rt_task_sleep(sleep_time);	
	rt_printf("end test\n");

	// Delete Mutex
	rt_mutex_delete(&res_a.mut);
	rt_mutex_delete(&res_b.mut);
	rt_sem_delete(&sem);

	
	return 0;
}

