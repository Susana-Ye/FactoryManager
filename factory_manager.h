#ifndef FACTORY_MANAGER_H
#define FACTORY_MANAGER_H


#include <semaphore.h>

struct process{
    int id;
    int tam;
    int nelem;
};

//Semáforos globales para sincronización
extern sem_t sem_factory_524789_525260;
extern sem_t sem_factory_wait_524789_525260;
extern sem_t sem_factory_done_wait_524789_525260; 
int main (int argc, const char * argv[]);

#endif