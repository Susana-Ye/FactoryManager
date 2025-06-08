#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"


//To create a queue
int queue_init(struct queue * q, int num_elements){ 
    // Control de error del valor de num_elements
    if (num_elements <= 0 || num_elements > MAX_QUEUE_SIZE){
        fprintf(stderr, "[ERROR][queue] There was an error while using queue with id %d.\n", q->id);
        return -1;
    } 
    q->max_q = num_elements;
    q->inicio_q = 0;
    q->final_q = 0;
    q->contador_q = 0;
    return 0;
}

// To Enqueue an element
int queue_put(struct queue * q, struct element *x) {
    q->queue[q->final_q] = *x;
    q->final_q = (q->final_q + 1) % q->max_q;
    q->contador_q++;
    printf("[OK][queue] Introduced element with id %d in belt %d.\n", x->num_edition, x->id_belt);
    return 0;
}

// To Dequeue an element.
int queue_get(struct queue * q, struct element *x) {
    *x = q->queue[q->inicio_q];
    
    q->inicio_q = (q->inicio_q + 1) % q->max_q;
    q->contador_q--;
    printf("[OK][queue] Obtained element with id %d in belt %d.\n", x->num_edition, x->id_belt);
    return 0;
}


//To check queue state
int queue_empty(struct queue * q) {
	int vacia = 0;
	if (q->contador_q == 0){
		vacia = 1;
	}
    return vacia;
}

//Compruebo si está llena la lista
int queue_full(struct queue * q) {
	int llena = 0;
	if (q->contador_q == q->max_q){
		llena = 1;
	}
    return llena;
}

//To destroy the queue and free the resources
int queue_destroy(struct queue * q) {
    //Como la cola es estática, no hay errores en su destrucción
    q->max_q = 0;
    q->inicio_q = 0;
    q->final_q = 0;
    q->contador_q = 0;
    return 0;
}

