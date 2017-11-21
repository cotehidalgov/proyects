/* Módulo de Input / Output */
#include <stdio.h>
/* Módulo standard, syscalls */
#include <stdlib.h>
/* Módulo para booleanos*/
#include <stdbool.h>

#include "estructuras.h"

bool function(List* a, List* b, int* insertions, int* count);

void print_list(List* list);

void write_output(FILE* output_file, int* insertions, int* count);

int main(int argc, char *argv[])
{
  /* Si la cantidad de parámetros no es la correcta */
  if(argc != 3)
  {
    printf("Faltan parámetros. Modo de uso:\n");
    printf("%s <puzzle> <output>\n", argv[0]);
    printf("\t<puzzle> es la ruta al archivo de puzzle\n");
    printf("\t<output> es la ruta al archivo de output\n");
    exit(2);
  }

  /* ESTE CÓDIGO ASUME QUE TODOS LOS PARÁMETROS ESTÁN BIEN FORMADOS */

  /* Abrimos el primer archivo en modo solo lectura */
  FILE* puzzle_file = fopen(argv[1], "r");
  /* Abrimos el primer archivo en modo solo escritura */
  FILE* output_file = fopen(argv[2], "w");

  /* Leemos alpha */
  int length_alpha;
  fscanf(puzzle_file, "%d", &length_alpha);
  char* sequence_alpha = malloc(sizeof(char) * (length_alpha + 1));
  fscanf(puzzle_file, "%s", sequence_alpha);

  /* Leemos beta */
  int length_beta;
  fscanf(puzzle_file, "%d", &length_beta);
  char* sequence_beta = malloc(sizeof(char) * (length_beta + 1));
  fscanf(puzzle_file, "%s", sequence_beta);

  /* Cerramos el stream de lectura */
  fclose(puzzle_file);

  List* a = create_list(sequence_alpha, length_alpha);
  List* b = create_list(sequence_beta, length_beta);

  int insertions[length_beta];
  int count = 0;

  bool value = function(a, b, insertions, &count); /* insertions = [null, null, ..., null] */

  if (value == 1){
    write_output(output_file, insertions, &count);
    printf("Problema solucionado!\n");
    printf("Se ha creado el archivo de salida\n");
  }
  else{
    printf("Problema no solucionado. No se a creado el archivo de salida.\n");
  }

  /* Liberamos ambas secuencias */
  free(sequence_alpha);
  free(sequence_beta);
  return 0;
}

bool function(List* a, List* b, int* insertions, int* count){
  // printf("%i   ", a->length);
  // print_list(a);
  if (a->length <= 0){
    return true;
  }
  if (b->length == 0) {
    return false;
  }
  /* Obtengo nodo a insertar*/
  Node* node_insert = b->head;
  /* Modifico b*/
  b->head = node_insert->next;
  b->length--;
  int index = 0;

  while (index <= a->length) {
    Info* info = insert(a, node_insert, index);
    if (function(a, b, insertions, count)){
      printf("Insert %c in %i\n", node_insert->value, index);
      insertions[(*count)] = index;
      (*count)++;
      return true;
    }
    uninsert(a, node_insert, info->deleted_nodes, info->index);
    index++;
  }

  /* Modifico b si no funciona */
  node_insert->next = b->head;
  node_insert->prev = NULL;
  b->head = node_insert;
  b->length++;
  return false;
}

void print_list(List* list){
  Node* current = list->head;
  while (current != NULL){
    printf("%c", current->value);
    current = current->next;
  }
  printf("\n");
}

void write_output(FILE* output_file, int* insertions, int* count){
  /* Imprimimos cosas al archivo de output */
  fprintf(output_file, "%d\n", (*count));
  for (int i = (*count)-1; i >= 0; i--) {
    fprintf(output_file, "%d\n", insertions[i]);
  }
  /* Cerramos el stream de escritura */
  fclose(output_file);
}
