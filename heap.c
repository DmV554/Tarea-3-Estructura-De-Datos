#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "heap.h"

typedef struct nodo{
   void* data;
   int priority;
}heapElem;

typedef struct Heap{
  heapElem* heapArray;
  int size;
  int capac;
} Heap;


void* heap_top(Heap* pq){
    if(pq == NULL || pq->size == 0) return NULL;
    return pq->heapArray[0].data;
}



void heap_push(Heap* pq, void* data, int priority){
  if (pq->size == pq->capac) {
      pq->capac = pq->capac * 2 + 1;
      pq->heapArray = realloc(pq->heapArray, sizeof(heapElem) * pq->capac);
    }

    int indiceUltimoElemento = pq->size;
    pq->heapArray[indiceUltimoElemento].data = data;
    pq->heapArray[indiceUltimoElemento].priority = priority;
  
    pq->size++;

    int indice = indiceUltimoElemento;
  
    while(indice > 0 && pq->heapArray[indice].priority < pq->heapArray[(indice -1) / 2].priority) { 
      heapElem tempElemHeapArray = pq->heapArray[indice];
      pq->heapArray[indice] = pq->heapArray[(indice -1) / 2];
      pq->heapArray[(indice -1) / 2] = tempElemHeapArray;
    
      indice = (indice - 1) / 2;
    }
}


void heap_pop(Heap* pq){
  if (pq->size == 0) {
    return;
  }

  if (pq->size == 1) {
    pq->size = 0;
    return;
  }

  pq->heapArray[0] = pq->heapArray[pq->size - 1];
  pq->size--;

  int indice = 0;

  while(1) {
    int indicePrimerHijo = indice * 2 + 1;
    int indiceSegundoHijo = indice * 2 + 2;
    int indiceHijoMenor = indice;

    if(indicePrimerHijo < pq->size && pq->heapArray[indicePrimerHijo].priority < pq->heapArray[indice].priority) {
      indiceHijoMenor = indicePrimerHijo;
    }

    if(indiceSegundoHijo < pq->size && pq->heapArray[indiceSegundoHijo].priority < pq->heapArray[indiceHijoMenor].priority) {
      indiceHijoMenor = indiceSegundoHijo;
    }

    if(indiceHijoMenor == indice) {
      break;
    }

    heapElem tempElemHeapArray = pq->heapArray[indice];
    pq->heapArray[indice] = pq->heapArray[indiceHijoMenor];
    pq->heapArray[indiceHijoMenor] = tempElemHeapArray;

    indice = indiceHijoMenor;
    
  }
}

Heap* createHeap(){
  Heap*heap = malloc(sizeof(Heap));
  heap->capac = 3;
  heap->size = 0;
  heap->heapArray = malloc(sizeof(heapElem) * heap->capac);
  
  return heap;
}
