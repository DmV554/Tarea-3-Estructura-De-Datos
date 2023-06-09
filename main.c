#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h> 
#include "list.h"
#include "heap.h"
#include "treemap.h"
#include "stack.h"

typedef struct {
  char nombre[31];
  int prioridad;
  List*listaTareasPrecedentes;
  bool explorada;
}Tarea;

typedef struct {
  char nombre[31]; 
  bool completada;
}TareaPrecedente;

typedef struct {
  Tarea*estadoTarea;
  TareaPrecedente*precedente;
  char accion[31];
}Accion;


void agregarTarea(TreeMap*,Stack*);
void establecerPrecedencia(TreeMap*,Stack*);
TareaPrecedente* crearNodoPrecedente(char*);
void mostrarTareasPorHacer(TreeMap *);
void reiniciarValores(TreeMap*);
void marcarNodosComoCompletados(TreeMap*, char*);
void mostrarListaTareasPorHacer(List*) ;
void mostrarPrecedentes(List*);
void marcarTareaComoCompletada(TreeMap*,Stack*);
void borrarNodosPrecedentes(List*);
void eliminarNodoListasPrecedentes(TreeMap*, char*);
void deshacerAccion(TreeMap*, Stack*);
void eliminarTarea(TreeMap*, char*);
void eliminarPrecedente(TreeMap* ,char*, char*);
void importarTareas(TreeMap*);

/*
  función para comparar claves de tipo string
  retorna 1 si son iguales
*/
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo string
  retorna 1 si son key1<key2
*/
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son iguales
*/
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son key1<key2
*/
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}


int main() {
    char opcion[11];
    bool ejecucion = true;

    TreeMap *mapaTareas = createTreeMap(lower_than_string);
    Stack*pilaAcciones = stack_create();

   while (ejecucion) {
    printf("\n1. Agregar tarea\n");
    printf("2. Establecer precedencia entre tareas\n");
    printf("3. Mostrar tareas por hacer\n");
    printf("4. Marcar tarea como completada\n");
    printf("5. Deshacer última acción\n");
    printf("6. Cargar datos desde un archivo\n");
    printf("Presione 0 para salir del programa\n\n");

    scanf("%10s[^\n]", opcion);
    getchar();

    //Se comprueba que la opcion ingresada sea un número
    if (isdigit(opcion[0])) {
      int opcionNumero = atoi(opcion);

      switch (opcionNumero) {
      case 1:
        agregarTarea(mapaTareas, pilaAcciones);
        break;
        
      case 2:
        establecerPrecedencia(mapaTareas, pilaAcciones);
        break;

      case 3:  
        mostrarTareasPorHacer(mapaTareas);
        break;

      case 4:
        marcarTareaComoCompletada(mapaTareas, pilaAcciones);
        break;

      case 5:
        deshacerAccion(mapaTareas, pilaAcciones);
        break;

      case 6:
        importarTareas(mapaTareas);
        break;

      case 0:
          printf("¡Que tengas un buen día!\n");
          ejecucion = false;
          break;
  
        default:
          printf("\nSelecciona una opción válida\n\n");
          break;
      }
    } else {
      if (isalpha(opcion[0])){
        printf("\nSelecciona una opción válida\n\n");
      }
    }
  }
  return 0;
}

/*Funcion para agregar una tarea al arbol con mapa, no se permiten tareas repetidas.
Se agrega a la pila de acciones la acción agregar, para deshacerla si lo indica el usuario*/
void agregarTarea(TreeMap *mapaTareas, Stack*pilaAcciones) {
  Tarea* tarea = (Tarea*) malloc(sizeof(Tarea));
  
  printf("Ingrese nombre de la tarea a agregar: ");
  scanf("%30[^\n]s", tarea->nombre);
  getchar();

  if(searchTreeMap(mapaTareas, tarea->nombre) != NULL) {
    printf("\nLa tarea %s ya existe\n\n", tarea->nombre);
    free(tarea);
    return;
  }
  
  printf("Ingrese la prioridad de la tarea: ");
  scanf("%i", &tarea->prioridad);
  tarea->explorada = false;

  tarea->listaTareasPrecedentes = createList();
  
  insertTreeMap(mapaTareas, tarea->nombre, tarea);

  Accion*AccionAGregar = malloc(sizeof(Accion));
  AccionAGregar->estadoTarea = tarea;
  strcpy(AccionAGregar->accion, "agregar tarea");

  stack_push(pilaAcciones, AccionAGregar);
  printf("\nTAREA INSERTADA CON ÉXITO\n");
}

/*Función que establece precedencia entre dos tareas, la tarea precedente se agrega a la lista de ta
reas precedentes de una tarea en particular, se agrega a la pila de acciones la accion de establecer
precedencia para posteriormente deshacerla si es indicado. Si hay tareas que no existen la ejecucion 
de la funcion finaliza*/
void establecerPrecedencia(TreeMap *mapaTareas, Stack*pilaAcciones) {
  char tareaPrecedente[31];
  char tareaDependiente[31];

  printf("Ingrese el nombre de la tarea precedente: ");
  scanf("%30[^\n]s", tareaPrecedente);
  getchar();

  printf("Ingrese el nombre de la tarea dependiente: ");
  scanf("%30[^\n]s", tareaDependiente);
  getchar();

  Pair *tareaPrecedentePair = searchTreeMap(mapaTareas, tareaPrecedente);
  
  if(tareaPrecedentePair == NULL) {
   printf("Una de las tareas o ambas no existen");
     return;
 }
  Tarea*tareaP = tareaPrecedentePair->value;
  
  Pair *tareaDependientePair = searchTreeMap(mapaTareas, tareaDependiente);
  
  if(tareaDependientePair == NULL) {
    printf("Una de las tareas o ambas no existen");
     return;
 }
  
  Tarea*tareaD = tareaDependientePair->value;

  TareaPrecedente * tareaPrecedenteNodo =  crearNodoPrecedente(tareaP->nombre);
  pushBack(tareaD->listaTareasPrecedentes, tareaPrecedenteNodo);

  Accion*accionPrecedencia = malloc(sizeof(Accion));
  accionPrecedencia->estadoTarea = tareaD;
  accionPrecedencia->precedente = tareaPrecedenteNodo;
  strcpy(accionPrecedencia->accion, "establecer precedencia");
  stack_push(pilaAcciones, accionPrecedencia);

  printf("\nSE HA ESTABLECIDO LA PRECEDENCIA CON ÉXITO\n");
}

/*Crea un nodo precedente de una tarea y la retorna*/
TareaPrecedente*crearNodoPrecedente(char*nombre) {
  TareaPrecedente*tareaPrecedenteNodo = malloc(sizeof(TareaPrecedente));
      strcpy(tareaPrecedenteNodo->nombre,nombre);
      tareaPrecedenteNodo->completada = false;

    return tareaPrecedenteNodo;
}


void mostrarTareasPorHacer(TreeMap *mapaTareas) {
  // Crear un montículo de mínimos para ordenar las tareas según su prioridad
  
  Heap *monticuloMinimos = createHeap();

  // Crear una lista de tareas completadas
  List *tareasPorHacer = createList();

  reiniciarValores(mapaTareas);
  // Insertar en el montículo las tareas que no tienen precedentes y marcarlas como exploradas
  Pair *tareaPair = firstTreeMap(mapaTareas);
  if(tareaPair == NULL) {
     return;
 }

  Tarea*tarea = tareaPair->value;
  
  while (tarea != NULL) {
    if (tarea->prioridad > 0 && firstList(tarea->listaTareasPrecedentes) == NULL) {
      tarea->explorada = true;
      heap_push(monticuloMinimos, tarea, tarea->prioridad);
    }
     tareaPair = nextTreeMap(mapaTareas);
     if(tareaPair == NULL) break;
     tarea = tareaPair->value;
  }

  // Mientras el montículo no esté vacío:
  while (heap_top(monticuloMinimos) != NULL) {
    // Eliminar la raíz del montículo (tarea con prioridad más alta)
    
    Tarea *tareaTop = heap_top(monticuloMinimos);
    heap_pop(monticuloMinimos);
    // Agregar la tarea eliminada a la lista de tareas completadas
    
    pushBack(tareasPorHacer, tareaTop);
    marcarNodosComoCompletados(mapaTareas, tareaTop->nombre);
    
    // Verificar si las tareas no exploradas tienen relaciones de precedencia y agregarlas al montículo si corresponde
   Pair *tareaPair = firstTreeMap(mapaTareas);
   Tarea*tarea = tareaPair->value;

    while (tarea != NULL) {
      if (tarea->explorada == false) {
        TareaPrecedente *tareaPrecedente = firstList(tarea->listaTareasPrecedentes);
        
        bool todasCompletas = true;
        while(tareaPrecedente != NULL) {
         if(tareaPrecedente->completada == false) {
            todasCompletas = false;
            break;
         }
          
          tareaPrecedente = nextList(tarea->listaTareasPrecedentes);
        }
        
        if(todasCompletas == true) {
          tarea->explorada = true;
          heap_push(monticuloMinimos, tarea, tarea->prioridad);
        }
        
      }
      tareaPair = nextTreeMap(mapaTareas);
      if(tareaPair == NULL) break;
      tarea = tareaPair->value;
    }

 
  }

  // Mostrar la lista de tareas completadas
  mostrarListaTareasPorHacer(tareasPorHacer);
}

/*Función que recorre todos los nodos del mapa y sus listas poniendolos en su estado inicial, 
es decir, no explorados y no completados/visitados, esto se hace para que al volver a llamar a la 
opcion 3, el ordenamiento se realize de forma correcta, de otra manera, los nodos entrarian explorados y/o
visitados de la llamada anterior, por lo tanto el programa no funcionará correctamente*/
void reiniciarValores(TreeMap*mapaTareas) {
 Pair *tareaPair = firstTreeMap(mapaTareas);
  if(tareaPair == NULL) {
    printf("NO HAY TAREAS\n");
    return;
 }
  
 Tarea*tarea = tareaPair->value;
    while (tarea != NULL) {

    tarea->explorada = false;
    TareaPrecedente* nodoLista = firstList(tarea->listaTareasPrecedentes);
    while(nodoLista != NULL) {
        nodoLista->completada = false; 
      nodoLista = nextList(tarea->listaTareasPrecedentes);
    }
          
    tareaPair = nextTreeMap(mapaTareas);
    if(tareaPair == NULL) break;
    tarea = tareaPair->value;

  }
  
}

/*Función que marca un nodo como completado, es decir la tarea se "realizó" y no se considerará para el ordenamiento
en la opción 3. El nodo completado se tiene que completar en todas las listas que tengan a ese nodo*/
void marcarNodosComoCompletados(TreeMap*mapaTareas, char*nombreNodoACompletar) {
   Pair *tareaPair = firstTreeMap(mapaTareas);
   Tarea*tarea = tareaPair->value;
  
    while (tarea != NULL) {
  
      TareaPrecedente* nodoLista = firstList(tarea->listaTareasPrecedentes);

      while(nodoLista != NULL) {
        if(strcmp(nodoLista->nombre, nombreNodoACompletar) == 0) {
          nodoLista->completada = true;
        }
        
        nodoLista = nextList(tarea->listaTareasPrecedentes);
      }
            
      tareaPair = nextTreeMap(mapaTareas);
      if(tareaPair == NULL) break;
      tarea = tareaPair->value;
    }
  
}

void mostrarListaTareasPorHacer(List*tareasPorHacer) {
  if (firstList(tareasPorHacer) == NULL) {
    printf("No hay tareas por hacer.\n");
  } else {

    printf("\nTAREAS POR HACER:\n");
      Tarea*tareasPorHacerNodo = firstList(tareasPorHacer);
      while (tareasPorHacerNodo != NULL) {
          printf("%s (Prioridad %d)", tareasPorHacerNodo->nombre, tareasPorHacerNodo->prioridad);
          mostrarPrecedentes(tareasPorHacerNodo->listaTareasPrecedentes);
          tareasPorHacerNodo = nextList(tareasPorHacer);
        printf("\n");
      }
    
    }
  cleanList(tareasPorHacer);
}

void mostrarPrecedentes(List*listaPrecedentes) {
  TareaPrecedente* nodoLista = firstList(listaPrecedentes);
  if(nodoLista != NULL) {
     printf(" | Precedentes: ");
  }

  while(nodoLista != NULL) {
 
    printf("%s ", nodoLista->nombre);
    nodoLista = nextList(listaPrecedentes);
  } 
}

/*Función que marca un nodo como completado, es decir lo elimina de la lista de tareas por hacer(y de todo el arbol)
si tiene relaciones de precedencia pregunta al usuario si esta seguro de eliminarlo, se agrega la accion eliminar 
a la pila de acciones para deshacerla si es que es indicado.
Antes de borrar una tarea con precedentes, borramos su lista primero junto a sus nodos precedentes.
Luego ese nodo lo borramos de todas las listas de precedentes ese nodo en particular, ya que al eliminarse no existe en general*/
void marcarTareaComoCompletada(TreeMap*mapaTareas, Stack*pilaAcciones){
  char tareaAEliminar[31];

  printf("Ingrese el nombre de la tarea a marcar como completada: \n");
  scanf("%30[^\n]s", tareaAEliminar);
  getchar();

  Pair* tareaAEliminarNodoPair = searchTreeMap(mapaTareas, tareaAEliminar);
  
  if(tareaAEliminarNodoPair == NULL) {
    printf("La tarea %s no se encontró en la lista\n", tareaAEliminar);
    return;
 }
  
  Tarea*tareaAEliminarNodo = tareaAEliminarNodoPair->value;

  Accion*accionEliminar = malloc(sizeof(Accion));
  accionEliminar->estadoTarea = tareaAEliminarNodo;

  if(firstList(tareaAEliminarNodo->listaTareasPrecedentes) != NULL){
    printf("¿Estás seguro que deseas eliminar la tarea? (s/n)\n");
    char respuesta[11];
    scanf("%10[^\n]s", respuesta);
    getchar();
    
    if(strcmp(respuesta, "s") != 0 && strcmp(respuesta, "S") != 0){
      return;
    }

    borrarNodosPrecedentes(tareaAEliminarNodo->listaTareasPrecedentes);

    eliminarNodoListasPrecedentes(mapaTareas,tareaAEliminarNodo->nombre);

    eraseTreeMap(mapaTareas, tareaAEliminarNodo);
    
  } else {
    eliminarNodoListasPrecedentes(mapaTareas,tareaAEliminarNodo->nombre);
    eraseTreeMap(mapaTareas, tareaAEliminarNodo);
  }
  
  strcpy(accionEliminar->accion, "borrar tarea");
  stack_push(pilaAcciones, accionEliminar);
  
  printf("La tarea %s se ha marcado como completada y ha sido eliminada de la lista de tareas por hacer.\n", tareaAEliminar);

}

/*Función que borra los nodos precedentes de una lista de precedentes*/
void borrarNodosPrecedentes(List *listaTareasPrecedentes) {
  TareaPrecedente*nodoLista = firstList(listaTareasPrecedentes);

  while(nodoLista != NULL) {
    popCurrent(listaTareasPrecedentes);
    nodoLista = nextList(listaTareasPrecedentes);
  }
}

/*Función que elimina un nodo de todas las listas de precedentes de todas las tareas del arbol*/
void eliminarNodoListasPrecedentes(TreeMap*mapaTareas, char*tareaAEliminar) {
  Pair*tareaNodoPair = firstTreeMap(mapaTareas);
  Tarea*tareaNodo = tareaNodoPair->value;

  while(tareaNodo != NULL) { 
    TareaPrecedente*nodoLista = firstList(tareaNodo->listaTareasPrecedentes);

    while(nodoLista != NULL) {
      if(strcmp(tareaAEliminar, nodoLista->nombre) == 0) {
        popCurrent(tareaNodo->listaTareasPrecedentes);
      }
      nodoLista = nextList(tareaNodo->listaTareasPrecedentes);
    }

    tareaNodoPair =  nextTreeMap(mapaTareas);
    if(tareaNodoPair == NULL) break;
    tareaNodo = tareaNodoPair->value;
    
  }
}

/*Funcion que deshace una accion en particular, si la accion fue agregar, eliminamos el nodo.
Si fue establecer precedencia, eliminamos el precedente de la lista de precedentes del nodo
Si fue borrar tarea o marcar como completada, insertamos el nodo que fue previamente eliminado*/
void deshacerAccion(TreeMap*mapaTareas, Stack*pilaACciones) {
  Accion*ultimaAccion = stack_top(pilaACciones);

  if(ultimaAccion == NULL) {
    printf("NO HAY ACCIONES REALIZADAS\n");
    return;
  }

  if(strcmp(ultimaAccion->accion, "agregar tarea") == 0) {
    eliminarTarea(mapaTareas,ultimaAccion->estadoTarea->nombre);
    stack_pop(pilaACciones);
  }

  if(strcmp(ultimaAccion->accion, "establecer precedencia") == 0) {
    eliminarPrecedente(mapaTareas,ultimaAccion->estadoTarea->nombre, ultimaAccion->precedente->nombre);
    stack_pop(pilaACciones);
  }

  if(strcmp(ultimaAccion->accion, "borrar tarea") == 0) {
    insertTreeMap(mapaTareas, ultimaAccion->estadoTarea->nombre, ultimaAccion->estadoTarea);
    stack_pop(pilaACciones);
  }

  printf("Accion deshecha con exito\n");
}

/*Elimina una tarea del arbol, eliminando previamente sus precedentes y los nodos de las listas en las que
es precedente*/
void eliminarTarea(TreeMap*mapaTareas, char*nombreTareaAEliminar) {
  Pair* tareaAEliminarNodoPair = searchTreeMap(mapaTareas, nombreTareaAEliminar);
  Tarea*tareaAEliminarNodo = tareaAEliminarNodoPair->value;

  if(tareaAEliminarNodo == NULL){
    printf("La tarea %s no se encontró en la lista\n", nombreTareaAEliminar);
    return;
  }

  if(firstList(tareaAEliminarNodo->listaTareasPrecedentes) != NULL) {

    borrarNodosPrecedentes(tareaAEliminarNodo->listaTareasPrecedentes);
    eliminarNodoListasPrecedentes(mapaTareas,tareaAEliminarNodo->nombre);
    eraseTreeMap(mapaTareas, tareaAEliminarNodo);
    
  } else {
    eliminarNodoListasPrecedentes(mapaTareas,tareaAEliminarNodo->nombre);
    eraseTreeMap(mapaTareas, tareaAEliminarNodo);
  }    
  
}

/*Elimina un nodo de la lista de precedentes de una tarea en particular*/
void eliminarPrecedente(TreeMap*mapaTareas ,char*nombreTareaAEliminarPrecedente, char*nombreTareaPrecedenteAEliminar) {
  Pair*tareaNodoPair = searchTreeMap(mapaTareas, nombreTareaAEliminarPrecedente);
  Tarea*tareaNodo = tareaNodoPair->value;

  TareaPrecedente*nodoLista = firstList(tareaNodo->listaTareasPrecedentes);

  while(nodoLista!=NULL) {
    if(strcmp(nodoLista->nombre, nombreTareaPrecedenteAEliminar) == 0) {
      popCurrent(tareaNodo->listaTareasPrecedentes);
    }
    nodoLista = nextList(tareaNodo->listaTareasPrecedentes);
  }
  
}

/*Función que lee un archivo con los elementos de las tareas separadas por espacio y coma (", ")(precedentes solo con  espacio" ")
Leemos linea por linea separando sus campos con strtok(), añadimos las variables a sus respectivos nodos y se inserta en el mapa
El especificado (" \n") es usado debido a que el programa leia saltos de linea y los guardaba, cosa que no debe pasar*/
void importarTareas(TreeMap* mapaTareas){
  char nombreArchivo[101];

  printf("\nIngrese el nombre del archivo a cargar\n");
  scanf("%100[^\n]s", nombreArchivo);

  FILE* archivoTareas = fopen(nombreArchivo, "r");

  if(archivoTareas == NULL){
    printf("¡Hubo un error abriendo el archivo!\n");
    return;
  }

  char lineaArchivo[200];
  bool primeraVuelta = true;
  while (fgets(lineaArchivo, sizeof(lineaArchivo), archivoTareas)){
    if(primeraVuelta) {
      primeraVuelta = false;
      continue;
    }
    
    Tarea *tareaNodo = malloc(sizeof(Tarea));
    
    strcpy(tareaNodo->nombre, strtok(lineaArchivo, ", "));

    char *prioridadChar = strtok(NULL, ", ");
    int prioridad = atoi(prioridadChar);

    tareaNodo->prioridad = prioridad;
    tareaNodo->explorada = false;
    tareaNodo->listaTareasPrecedentes = createList();

    char*precedente = strtok(NULL, " \n");
    while(precedente != NULL) {
      TareaPrecedente* nodoPrecedente = malloc(sizeof(TareaPrecedente));
      if(nodoPrecedente == NULL) exit(EXIT_FAILURE);
      
      strcpy(nodoPrecedente->nombre, precedente);
      nodoPrecedente->completada = false;
        
      pushBack(tareaNodo->listaTareasPrecedentes, nodoPrecedente);

      precedente = strtok(NULL, " \n");
    }

    insertTreeMap(mapaTareas, tareaNodo->nombre, tareaNodo);
  }
  printf("\n¡Datos cargados de manera exitosa!\n");
  
  fclose(archivoTareas);
}
