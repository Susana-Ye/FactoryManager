#ifndef QUEUE_H
#define QUEUE_H


//Tamaño máximo que permite el array
#define MAX_QUEUE_SIZE 1000

struct element {
  int num_edition;
  int id_belt;
  int last; //flag de si es el ultimo elemento
};

//Defino la cola
struct queue {
  struct element queue[MAX_QUEUE_SIZE]; 
  int id;
  int max_q; //Valor real del tamaño (me lo pasan por parámetro)
  int inicio_q; //Índice del primer elemento de la cola
  int final_q; //Índice del último elemento de la cola
  int contador_q; //Número de elemento que contiene la cola
};

int queue_init(struct queue * q, int num_elements);
int queue_destroy(struct queue * q);
int queue_put(struct queue * q, struct element *x);
int queue_get(struct queue * q, struct element *x);
int queue_empty(struct queue * q);
int queue_full(struct queue * q);

#endif