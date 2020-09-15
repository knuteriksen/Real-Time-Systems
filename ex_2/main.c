#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"


int main(){

  long stuff = 1;
  long size = 5;
  Array arr = array_new(size);

  for (int i = 0; i < 10; i++){
    array_insertBack(&arr, stuff);
  }

  printf("Length: %ld\n", array_length(arr));
  printf("Capacity: %ld\n", arr.capacity);
  
  return 0;
}
