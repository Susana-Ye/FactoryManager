/*
 *
 * factory_manager.c
 *
 * SE ENCARGA DE LANZAR N HILOS, FUNCIONES QUE SON LOS PROCESS MANAGER, 
 * el fichero de entrada nos dirá el número máximo de process_manager que crear
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdint.h>
#include "process_manager.h"
#include "factory_manager.h"

sem_t sem_factory_524789_525260;
sem_t sem_factory_wait_524789_525260; 
sem_t sem_factory_done_wait_524789_525260; 

int main (int argc, const char * argv[] ){
	
	
	//Control de argumentos
	if (argc != 2){
		return -1;
	}

	FILE *fich = fopen(argv[1], "r"); //Abro el fichero en modo lectura

    if (fich == NULL) { //Error apertura del fichero
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

	
    int max_cintas;
    if (fscanf(fich, "%d", &max_cintas) == EOF){ //Error en lectura del fichero
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        fclose(fich);
        return -1;
    }

    if (max_cintas <= 0){ //No puede haber <= 0 cintas, no tiene sentido llamar a factory manager para no crear ningun proceso
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        fclose(fich);
        return -1;
    }


    struct process procesos[max_cintas];
    int num_procesos = 0;
    //Leemos el primero
    int comprob_lectura;
    if ((comprob_lectura = fscanf(fich, "%d %d %d", &procesos[num_procesos].id, &procesos[num_procesos].tam, &procesos[num_procesos].nelem)) == EOF){
        //Error de lectura en el primer process_manager
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        fclose(fich);
        return -1;
    }
    
    
    while (comprob_lectura != EOF && num_procesos < max_cintas){
        
        for (int j = 0; j < num_procesos; j++){ //Comprobamos que no existan id's iguales
            if(procesos[j].id == procesos[num_procesos].id){
                fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
                fclose(fich);
                return -1;
            }
        }
        
        //No puede haber ids negativos
        //No puede haber una cinta de tamaño 0 o negativo
        //Tampoco tiene sentido generar 0 o un número negativo de elementos
        if (procesos[num_procesos].id <= 0 || procesos[num_procesos].tam <= 0 || procesos[num_procesos].nelem <= 0){ 
            fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
            fclose(fich);
            return -1;
        }

        num_procesos++;

        if(num_procesos < max_cintas){
            //Leemos el siguiente process manager

            //Si no puedo leer el primer dato, comprob_lectura será EOF y he terminado de leer los process_manager
            comprob_lectura = fscanf(fich, "%d", &procesos[num_procesos].id); 
            //Si he podido leer el primer dato del nuevo process_manager pero fallan los siguientes 2 datos, es error, estan incompletos los datos
            if(comprob_lectura != EOF && fscanf(fich, "%d %d", &procesos[num_procesos].tam, &procesos[num_procesos].nelem) == EOF){
                fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
                fclose(fich);
                return -1;
            }
        }
       
    }

    //Comprobamos y damos error si declaran max cintas = 4 y luego me dan información extra, ej de 5 cintas
    int a;
    if(num_procesos >= max_cintas && fscanf(fich, "%d", &a) != EOF){ //He podido leer un entero más cuando num_procesos ha alcanzado el máximo
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        fclose(fich);
        return -1;
    }

    //Control de que se cierra correctamente el archivo
    if (fclose(fich) != 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

  
    pthread_t threads[num_procesos];
    

    //Inicializamos los semáforos en 0
    sem_init(&sem_factory_524789_525260, 0, 0);
    sem_init(&sem_factory_wait_524789_525260, 0, 0);
    sem_init(&sem_factory_done_wait_524789_525260, 0, 0);

    //Creo los hilos process_manager
    for (int i = 0; i < num_procesos; i++) {
        if (pthread_create(&threads[i], NULL, process_manager, (void*)&procesos[i]) != 0) {
            fprintf(stderr, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", procesos[i].id);
            //Destruimos los semáforos
            sem_destroy(&sem_factory_524789_525260);
            sem_destroy(&sem_factory_wait_524789_525260);
            sem_destroy(&sem_factory_done_wait_524789_525260);
            return -1;
        }
        printf("[OK][factory_manager] Process_manager with id %d has been created.\n", procesos[i].id);
    }

    //Despierto a todos los hilos a la vez para que manden el mensaje de estar esperando
    for (int i = 0; i < num_procesos; i++) {
        sem_post(&sem_factory_wait_524789_525260); 
    }

    //Espero a que todos los hilos terminen de mandar el mensaje de estar esperando
    for (int i = 0; i < num_procesos; i++) {
        sem_wait(&sem_factory_done_wait_524789_525260);
    }
    
    //Despierto a todos los hilos a la vez para que trabajen
    for (int i = 0; i < num_procesos; i++) {
        sem_post(&sem_factory_524789_525260); 
    }
    
    
    void* result; //Variable que me almacena el estado de retorno de cada hilo

    //Esperamos y recogemos a todos los hilos
    for (int i = 0; i < num_procesos; i++) {
        if(pthread_join(threads[i], &result) != 0){ //Compruebo que no falla pthread_join
            fprintf(stderr,"[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", procesos[i].id);
            //Destruimos los semáforos
            sem_destroy(&sem_factory_524789_525260);
            sem_destroy(&sem_factory_wait_524789_525260);
            sem_destroy(&sem_factory_done_wait_524789_525260);
            return -1;
        }
        
        //Compruebo si el process_manager terminó con un error
        if (result != NULL && (intptr_t)result != 0) {
            fprintf(stderr,"[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", procesos[i].id);
            //Destruimos los semáforos
            sem_destroy(&sem_factory_524789_525260);
            sem_destroy(&sem_factory_wait_524789_525260);
            sem_destroy(&sem_factory_done_wait_524789_525260);
            return -1;
        }
        printf("[OK][factory_manager] Process_manager with id %d has finished.\n", procesos[i].id);

    }

    //Destruimos los semáforos
    sem_destroy(&sem_factory_524789_525260);
    sem_destroy(&sem_factory_wait_524789_525260);
    sem_destroy(&sem_factory_done_wait_524789_525260);

    printf("[OK][factory_manager] Finishing.\n");

	return 0;
}
