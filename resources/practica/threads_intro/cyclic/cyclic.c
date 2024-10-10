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
* This task yields (returns) to let other tasks run.
*/
void task(int task_number) {
  for (int j = 1; j <= 3; ++j) {
    /*
    * Do something "useful".
    */
    delay(task_number);

    fprintf(stderr, "I am task %*s%d!\n", task_number, " ", task_number);
    fflush(stderr);
  }
}

int main() {
  /*
  * Cyclic Executive:  Give each task a chance to run.
  * Each task function must yield (return) at some point.
  */
  while (1) {
    task(1);
    task(2);
    task(3);
  }

  return 0;
}

