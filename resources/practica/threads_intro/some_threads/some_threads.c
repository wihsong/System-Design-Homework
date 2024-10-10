#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

void delay(int tenths) {
  volatile int a = 0; 
  for (long i = 0; i < 20000000 * tenths; ++i) { 
    a = a + 1;
  }
}

/*
* An infinite loop!  This task function does not return.
*/
void* thread(void *args) {
  uintptr_t threadid = (uintptr_t)args;
  while (1) {
    delay(threadid);
    fprintf(stderr, "I am thread %*s%lu!\n", (int) threadid, " ", threadid);
    fflush(stderr);
  }
  return args;
}

/*
* Start three threads, all of which fail to yield (return).
*/
int main() {
  pthread_t t1, t2, t3;
  uintptr_t rc1, rc2, rc3;

  pthread_create(&t1, NULL, thread, (void*)1);
  pthread_create(&t2, NULL, thread, (void*)2);
  pthread_create(&t3, NULL, thread, (void*)3);

  fprintf(stderr, "I am main\n");

  /*
  * Ignore everything after here for now . . .
  */
  pthread_join(t1, (void **)&rc1);
  pthread_join(t2, (void **)&rc2);
  pthread_join(t3, (void **)&rc3);

  printf("thread1 returned %lu; thread 2 returned %lu, thread 3 returned %lu\n", rc1, rc2, rc3);

  return 0;
}

