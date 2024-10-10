#include <assert.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>

#define N 5

intptr_t forks[N];
void get_fork(int i, intptr_t thread_id) {

  if (forks[i] != -1) {
    // this should never happen
    printf("Oh No! Someone already has fork %d\n", i);
  }
  else {
    forks[i] = thread_id;
  }
}

void put_fork(int i, intptr_t thread_id) {

  if (forks[i] != thread_id) {
    // this should never happen
    printf("Oh No! %lu does not have this fork\n", thread_id);
  }
  forks[i] = -1;
}


void * thread(void *arg) {

  intptr_t thread_id = (intptr_t) arg;
  int left_fork = thread_id, right_fork = (thread_id + 1) % N;

  while (1) {

    // Get forks.
    get_fork(left_fork, thread_id);
    get_fork(right_fork, thread_id);

    int eat = rand() % 5 + 1;
    printf("thread %lu eating for %d secs\n", thread_id, eat);
    sleep(eat);

    // Put Forks down.
    put_fork(right_fork, thread_id);
    put_fork(left_fork, thread_id);

    int think = rand() % 5 + 1;
    printf("thread %lu thinking for %d secs\n", thread_id, think);
    sleep(think);
  }
  return NULL;
}

int main() {
  pthread_t philos[N];

  // initialize some stuff
  srand(time(NULL));

  for (uintptr_t i = 0; i < N; ++i) {
    forks[i] = -1;
  }

  for (uintptr_t i = 0; i < N; ++i) {
    pthread_create(philos + i, NULL, (void *(*)(void*))thread, (void*)i);
  }

  for (int i = 0; i < N; ++i) {
    pthread_join(philos[i], NULL);
    // this code is never really reached
  }

  return 0;
}
