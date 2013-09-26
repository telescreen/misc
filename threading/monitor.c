#include <pthread.h>
#include "errors.h"

#define SPIN 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;


void *counter_thread(void *arg)
{
        int status;
        int spin;

        while (time < end_time) {
                status = pthread_mutex_lock(&mutex);
                if (status < 0)
                        err_abort(status, "Lock mutex");
                for(spin = 0; spin < SPIN; ++spin) {
                        counter++;
                }
                status = pthread_mutex_unlock(&mutex);
                if (status != 0)
                        err_abort(status, "Unlock mutex");
                sleep(1);
        }
        return NULL;
}

void *monitor_thread(void *arg)
{
        int status;
        int misses = 0;

        while (time(NULL) < end_time) {
                sleep(3);
                status = pthread_mutex_trylock(&mutex);
                if (status != EBUSY) {
                        if (status != 0)
                                err_abort(status, "Trylock mutex");
                        printf("Counter is %ld\n", counter/SPIN);
                        status = pthread_mutex_unlock(&mutex);
                        if (status != 0)
                                err_abort(status, "Unlock mutex");
                } else
                        misses++;
        }
        printf("Monitor thread missed update %d times.\n", misses);
        return NULL;
}

int main(int argc, char *argv[])
{
        
}
