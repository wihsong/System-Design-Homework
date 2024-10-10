#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


struct thread_parms {
  uintptr_t arg;
  char *name;
};

void* thread(void *args) {
  //uintptr_t threadid = (uintptr_t)args;
  struct thread_parms *tp = (struct thread_parms*) args;

  fprintf(stderr, "I am thread %lu (%s)!\n", tp->arg, tp->name);
  return (void*)tp->arg;
}

int main() {
  pthread_t t1, t2;
  uintptr_t rc1, rc2;

  struct thread_parms tp;
  tp.arg = 1;
  tp.name = "bananas";

  struct thread_parms tp2;
  tp2.arg = 2;
  tp2.name = "apples";

  pthread_create(&t1, NULL, thread, (void*)&tp);
  pthread_create(&t2, NULL, thread, (void*)&tp2);

  fprintf(stderr, "I am main\n");

  pthread_join(t1, (void **)&rc1);
  pthread_join(t2, (void **)&rc2);

  printf("thread1 returned %lu; thread 2 returned %lu\n", rc1, rc2);

  return 0;
}
