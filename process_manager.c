/*
 *
 * process_manager.c
 *
 *
 * SE ENCARGA DE LANZAR 2 HILOS, EL PRODUCTOR Y CONSUMIDOR UNA COLA CIRCULAR (EDA, cola DE MEMORIA PLANA)
 * SE LLAMA CIRCULAR PERO ES PLANO, SE LLAMA ASÍ PORQUE LA VENTAJA ES QUE PERMITE SEGUIR ESCRIBIENDO SOBRE
 * EL PRINICPIO DEL cola CUANDO LLEGAS AL FINAL (INFINITO)
 * A MEDIDA QUE EL CONSUMIDOR VACÍA LOS ELEMENTOS DEL PRINCIPIO DE LA COLA, EL PRODUCTOR PUEDE SEGUIR PRODUCIENDO
 * PARA SABER QUE EL cola ESTA VACIO PUEDES USAR 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include <stdint.h>
#include "process_manager.h"
#include "factory_manager.h"
#include <semaphore.h>


void* Productor (void *args){
	struct th_arg *arg = (struct th_arg *)args;
	int items_to_produce = arg->nelem;
	struct element elemento = {0}; //Produzco el elemento
	elemento.id_belt = arg->id;
	elemento.last = 0;
	for (int i = 0; i < items_to_produce; i++){
		elemento.num_edition = i; //Asigno al elemento su orden de creación
		if (i == items_to_produce){
			elemento.last = 1;
		}
		pthread_mutex_lock(arg->mutex_524789_525260); //Controlo el acceso a la cinta
		while (queue_full(arg->q)){ //si cola llena
			pthread_cond_wait(arg->not_full_524789_525260, arg->mutex_524789_525260); //se bloquea	
		}
		queue_put(arg->q, &elemento);
		arg->nelem_processed++; //Contabilizamos el elemento producido
		pthread_cond_signal(arg->not_empty_524789_525260); //cola no vacia
		pthread_mutex_unlock(arg->mutex_524789_525260);

	}
	pthread_exit(0);
}

void* Consumidor (void *args){
	struct th_arg *arg = (struct th_arg *)args;
	int items_to_produce = arg->nelem;

	struct element elemento = {0}; //El elemento que consumo
	
	for (int i = 0; i< items_to_produce; i++){
		pthread_mutex_lock(arg->mutex_524789_525260); //Controlo el acceso a la cinta
		while (queue_empty(arg->q)) { //si cola vacio
			pthread_cond_wait(arg->not_empty_524789_525260, arg->mutex_524789_525260); //se bloquea	--> se reactiva cuando recibe la señal del productor: hace cond_signal y tras hacer su unlock yo me activo
			//Si el hilo que me hace la señal empieza a actuar antes que yo, me quedaré bloqueado para siempre
			//El pthread wait lleva un unlock implícito
		}
		queue_get(arg->q, &elemento);
		arg->nelem_processed++; //Contabilizamos el elemento consumido
		
		pthread_cond_signal(arg->not_full_524789_525260); //cola no llena
    	pthread_mutex_unlock(arg->mutex_524789_525260);
	}
	pthread_exit(0);
}


void *process_manager(void *args){
	struct process *arg = (struct process *)args;
	int id = arg->id;
	int belt_size = arg->tam;
	int items_to_produce = arg->nelem;

	if (id <= 0 || belt_size <= 0 || items_to_produce <= 0){ //Los ids, el tamaño maximo y el numero de elementos a producir del process_manager no pueden ser 0 o negativo
		fprintf(stderr, "[ERROR][process_manager] Arguments not valid.\n");
		sem_post(&sem_factory_done_wait_524789_525260); 
		pthread_exit((void *)(intptr_t)-1);
	}

	pthread_t th_prod, th_cons;

	//Espero a que el factory_manager me avise para mandar el mensaje de esperando
	sem_wait(&sem_factory_wait_524789_525260);
	printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n", id, items_to_produce);
	
	//Ya he terminado de mandar el mensaje de esperando
	sem_post(&sem_factory_done_wait_524789_525260); 

	//Espero a que el factory_manager me avise para empezar
	sem_wait(&sem_factory_524789_525260);

	pthread_mutex_t mutex_524789_525260;
	pthread_cond_t not_empty_524789_525260; //Para controlar si la cola está vacía
	pthread_cond_t not_full_524789_525260; //Para controlar si la cola está llena

	//Inicializo el mutex y las variables de condición
    pthread_mutex_init(&mutex_524789_525260, NULL);
    pthread_cond_init(&not_empty_524789_525260, NULL);
    pthread_cond_init(&not_full_524789_525260, NULL);
	
	//Creo la cinta
	struct queue q;
	q.id = id;
	if(queue_init(&q, belt_size) != 0){
		pthread_exit((void *)(intptr_t)-1);
	}

	printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", id, belt_size);
	

	
	struct th_arg dataprod;
	dataprod.q = &q;
	dataprod.id = id;
	dataprod.nelem = items_to_produce;
	dataprod.nelem_processed = 0;
	dataprod.mutex_524789_525260 = &mutex_524789_525260;
	dataprod.not_empty_524789_525260 = &not_empty_524789_525260;
	dataprod.not_full_524789_525260 = &not_full_524789_525260;



	struct th_arg datacons;
	datacons.q = &q;
	datacons.id = id;
	datacons.nelem = items_to_produce;
	datacons.nelem_processed = 0;
	datacons.mutex_524789_525260 = &mutex_524789_525260;
	datacons.not_empty_524789_525260 = &not_empty_524789_525260;
	datacons.not_full_524789_525260 = &not_full_524789_525260;



	//Creo los hilos para productor y consumidor 
	if (pthread_create(&th_prod, NULL, Productor, (void*)&dataprod) != 0) {
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		//Destruimos los recursos
		queue_destroy(&q);
		pthread_mutex_destroy(&mutex_524789_525260);
    	pthread_cond_destroy(&not_empty_524789_525260);
    	pthread_cond_destroy(&not_full_524789_525260);
		pthread_exit((void *)(intptr_t)-1);
	}

	if (pthread_create(&th_cons, NULL, Consumidor, (void*)&datacons) != 0) {
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		//Destruimos los recursos
		queue_destroy(&q);
		pthread_mutex_destroy(&mutex_524789_525260);
    	pthread_cond_destroy(&not_empty_524789_525260);
    	pthread_cond_destroy(&not_full_524789_525260);
		pthread_exit((void *)(intptr_t)-1);
	}


	//Compruebo si el hilo productor y consumidor terminó correctamente
	if (pthread_join(th_prod, NULL) != 0 || pthread_join(th_cons, NULL) != 0) {
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		//Destruimos los recursos
		queue_destroy(&q);
		pthread_mutex_destroy(&mutex_524789_525260);
    	pthread_cond_destroy(&not_empty_524789_525260);
    	pthread_cond_destroy(&not_full_524789_525260);
		pthread_exit((void *)(intptr_t)-1);
	} 
	

	//Compruebo que el número de los elementos producidos y consumidos coincidan
	if(dataprod.nelem_processed != datacons.nelem_processed || dataprod.nelem_processed != items_to_produce || datacons.nelem_processed != items_to_produce){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		//Destruimos los recursos
		queue_destroy(&q);
		pthread_mutex_destroy(&mutex_524789_525260);
    	pthread_cond_destroy(&not_empty_524789_525260);
    	pthread_cond_destroy(&not_full_524789_525260);
		pthread_exit((void *)(intptr_t)-1);
	}

	printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", id, items_to_produce);
	
	//Control de errores a la hora de destruir los recursos
	queue_destroy(&q); //No hay errores a la hora de destruirlo porque hemos hecho la cola estática
	//Control de error a la hora de destruir el mutex y variables de condición
	if (pthread_mutex_destroy(&mutex_524789_525260) != 0 || pthread_cond_destroy(&not_empty_524789_525260) != 0 || pthread_cond_destroy(&not_full_524789_525260) != 0){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		pthread_exit((void *)(intptr_t)-1);
	}



	pthread_exit(0);
}
