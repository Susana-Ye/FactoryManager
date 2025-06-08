#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H


#include "queue.h"

struct th_arg {
	int id;
	int nelem;
	int nelem_processed;
	struct queue * q;
    pthread_mutex_t * mutex_524789_525260;
	pthread_cond_t * not_empty_524789_525260;
	pthread_cond_t * not_full_524789_525260;
};

void* Productor (void *args);
void* Consumidor (void *args);
void *process_manager(void *arg);

#endif