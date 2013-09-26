#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include "errors.h"

#define CREW_SIZE 4

typedef struct work_tag {
    struct work_tag *next;
    char *path;
    char *string;
} work_t, *work_p;

typedef struct worker_tag {
    int index;
    pthread_t thread;
    struct crew_tag *crew;
} worker_t, *worker_p;

typedef struct crew_tag {
    int crew_size;
    worker_t crew[CREW_SIZE];
    long work_count;
    work_t *first, *last;
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
} crew_t, *crew_p;

size_t path_max;
size_t name_max;

void *worker_routine(void *arg)
{
    worker_p mine = (worker_t*)arg;
    crew_p crew = mine->crew;
    work_p work, new_work;
    struct stat filestat;
    struct dirent *entry;
    int status;

    entry = (struct dirent*)malloc(sizeof(struct dirent) + name_max);
    if (entry == NULL)
        errno_abort("Allocating dirent");

    status = pthread_mutex_lock(&crew->mutex);
    if (status != 0)
        err_abort(status, "Lock crew mutex");

    while (crew->work_count == 0) {
        status = pthread_cond_wait(&crew->go, &crew->mutex);
        if (status != 0)
            err_abort(status, "Wait for go");
    }

    status = pthread_mutex_unlock(&crew->mutex);
    if (status != 0)
        err_abort(status, "Unlock mutex");
    DPRINTF(("Crew %d starting\n", mine->index));

    while (1) {
        status = pthread_mutex_lock(&crew->mutex);
        if (status != 0)
            err_abort(status, "Lock crew mutex");
        DPRINTF(("Crew %d top: first is %#lx, count is %d\n",
                mine->index, crew->first, crew->work_count));

        while (crew->first == NULL) {
            status = pthread_cond_wait(&crew->go, &crew->mutex);
            if (status != 0)
                err_abort(status, "Wait for work");
        }

        work = crew->first;
        crew->first = work->next;
        if (crew->first == NULL)
            crew->last == NULL;

        DPRINTF(("Crew %d took %#lx, leaves first %#lx, last %#lx\n",
                mine->index, work, crew->first, crew->last));

        status = pthread_mutex_unlock(&crew->mutex);
        if (status != 0)
            err_abort(status, "Unlock mutex");

        status = lstat(work->path, &filestat);

        if (S_ISLNK(filestat.st_mode))
            printf("Thread %d: %s is a link, skipping.\n",
                    mine->index,
                    work->path);
        else if (S_ISDIR(filestat.st_mode)) {
            DIR *directory;
            struct dirent *result;

            directory = opendir(work->path);
            if (directory == NULL) {
                fprintf(stderr, "Unable to open directory %s: %d (%s)\n",
                        work->path,
                        errno, strerror(errno));
                continue;
            }

            while (1) {
                status = readdir_r(directory, entry, &result);
                if (status != 0) {
                    fprintf(stderr, "Unable to read directory %s: %d (%s)\n",
                            work->path,
                            status, strerror(status));
                    break;
                }
                if (result == NULL)
                    break;

                if (strcmp(entry->d_name, ".") == 0)
                    continue;
                if (strcmp(entry->d_name, "..") == 0)
                    continue;
                new_work = (work_p)malloc(sizeof(work_t));
                if (new_work == NULL)
                    errno_abort("Unable to allocate space");
                new_work->path = (char*)malloc(path_max);
                if (new_work->path == NULL)
                    errno_abort("Unable to allocate path");
                strcpy(new_work->path, work->path);
                strcat(new_work->path, "/");
                strcat(new_work->path, entry->d_name);
                new_work->string = work->string;
                new_work->next = NULL;
                status = pthread_mutex_lock(&crew->mutex);

                if (status != 0)
                    err_abort(status, "Lock mutex");
                if (crew->first == NULL) {
                    crew->first = new_work;
                    crew->last = new_work;
                } else {
                    crew->last->next = new_work;
                    crew->last = new_work;
                }
                crew->work_count++;
                DPRINTF(("Crew %d: add %#lx, first %#lx, last %#lx, %d\n",
                        mine->index, new_work, crew->first, 
                        crew->last, crew->work_count));
            }
        }
    }
}
