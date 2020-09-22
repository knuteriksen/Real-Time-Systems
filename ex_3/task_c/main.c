#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

long glob_var = 0;

sem_t mutex;

void* create_local_variable(void* vargp){
  long local_var = 0;
  for (int i = 0; i < 50000000; i++){
    local_var ++;
    sem_wait(&mutex);
    glob_var ++;
    sem_post(&mutex);
  }
  sem_wait(&mutex);
  printf("Global: %lu\n", glob_var);
  sem_post(&mutex);
  printf("Local : %lu\n", local_var);
  return NULL;
}

int main(){
  sem_init(&mutex, 0, 1);
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, create_local_variable, NULL);
  pthread_create(&thread2, NULL, create_local_variable, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sem_destroy(&mutex);
  return 0;
}
