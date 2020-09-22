#include <stdio.h>
#include <pthread.h>

long glob_var = 0;

void* create_local_variable(void* vargp){
  long local_var = 0;
  for (int i = 0; i < 50000000; i++){
    local_var ++;
    glob_var ++;
  }
  printf("Global: %lu\n", glob_var);
  printf("Local : %lu\n", local_var);
  return NULL;
}

int main(){
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, create_local_variable, NULL);
  pthread_create(&thread2, NULL, create_local_variable, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  return 0;
}
