#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_PHILOSOPHERS 5
#define EAT_TIME 2
#define THINK_TIME 5


void* eat(void* phnum);
void* think(void* phnum);

// Philosphers go from 0 to 4
// Forks go from 0 to 4


int get_left_fork(int phnum){
  return phnum;
}

int get_right_fork(int phnum){
  return (phnum + 1) % NUM_PHILOSOPHERS;
}

// pthread_mutex_t fork_0, fork_1, fork_2, fork_3, fork_4;


pthread_mutex_t forks[5];
pthread_mutex_t fix;

void* eat(void* phnum){
  while(1){
    // sleep(1);
    pthread_mutex_lock(&fix);
    pthread_mutex_lock(&forks[(int)phnum]);
    pthread_mutex_lock(&forks[((int)phnum + 1) % NUM_PHILOSOPHERS]);
    pthread_mutex_unlock(&fix);
    printf("Philosopher %d is now eating with fork %d and %d\n",(int)phnum, (int)phnum, ((int)phnum + 1) % NUM_PHILOSOPHERS);
    sleep(EAT_TIME);
    pthread_mutex_unlock(&forks[(int)phnum]);
    pthread_mutex_unlock(&forks[((int)phnum + 1) % NUM_PHILOSOPHERS]);
    printf("Philosopher %d is done eating \n", (int)phnum);
    // think(phnum);
  }
  return NULL;
}

int main() {
  pthread_mutex_init(&fix, NULL);
  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
      pthread_mutex_init(&forks[i], NULL);
  }

  pthread_t threads[5];

  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
     pthread_create(&threads[i], NULL, eat, (void *)i);
  }

  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
    pthread_join(threads[i], NULL);

  }

  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
      pthread_mutex_destroy(&forks[i]);
  }
  pthread_mutex_destroy(&fix);

  return 0;
}
