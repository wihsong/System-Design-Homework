#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef enum { READERS, WRITERS, N_WAY } PRIORITY;

typedef struct rwlock {
    pthread_mutex_t lock;
    pthread_cond_t readers_ok;
    pthread_cond_t writers_ok;
    int active_readers;
    int waiting_writers;
    int active_writers;
    PRIORITY priority;
    int n_way;
} rwlock_t;

rwlock_t *rwlock_new(PRIORITY p, int n);
void rwlock_delete(rwlock_t **l);
void reader_lock(rwlock_t *rw);
void reader_unlock(rwlock_t *rw);
void writer_lock(rwlock_t *rw);
void writer_unlock(rwlock_t *rw);

rwlock_t *rwlock_new(PRIORITY p, int n) {
    rwlock_t *rw = malloc(sizeof(rwlock_t));
    if (rw == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(&rw->lock, NULL) != 0) {
        free(rw);
        return NULL;
    }

    if (pthread_cond_init(&rw->readers_ok, NULL) != 0) {
        pthread_mutex_destroy(&rw->lock);
        free(rw);
        return NULL;
    }

    if (pthread_cond_init(&rw->writers_ok, NULL) != 0) {
        pthread_cond_destroy(&rw->readers_ok);
        pthread_mutex_destroy(&rw->lock);
        free(rw);
        return NULL;
    }

    rw->active_readers = 0;
    rw->waiting_writers = 0;
    rw->active_writers = 0;
    rw->priority = p;
    rw->n_way = n;

    return rw;
}

void rwlock_delete(rwlock_t **l) {
    if (l == NULL || *l == NULL) {
        return;
    }

    pthread_mutex_destroy(&(*l)->lock);
    pthread_cond_destroy(&(*l)->readers_ok);
    pthread_cond_destroy(&(*l)->writers_ok);

    free(*l);
    *l = NULL;
}

void reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    while (rw->active_writers > 0 || (rw->priority == WRITERS && rw->waiting_writers > 0)) {
        pthread_cond_wait(&rw->readers_ok, &rw->lock);
    }

    rw->active_readers++;

    pthread_mutex_unlock(&rw->lock);
}

void reader_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->active_readers--;

    if (rw->active_readers == 0 && rw->waiting_writers > 0) {
        pthread_cond_signal(&rw->writers_ok);
    }

    pthread_mutex_unlock(&rw->lock);
}

void writer_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->waiting_writers++;

    while (rw->active_readers > 0 || rw->active_writers > 0) {
        pthread_cond_wait(&rw->writers_ok, &rw->lock);
    }

    rw->waiting_writers--;
    rw->active_writers++;

    pthread_mutex_unlock(&rw->lock);
}

void writer_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->active_writers--;

    if (rw->priority == READERS || rw->waiting_writers == 0) {
        pthread_cond_broadcast(&rw->readers_ok);
    } else {
        pthread_cond_signal(&rw->writers_ok);
    }

    pthread_mutex_unlock(&rw->lock);
}
