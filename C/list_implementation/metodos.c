
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "estructuras.h"


List* create_list(char* sequence, int length){

  if (length == 0){
    return NULL;
  }

  List* list = malloc(sizeof(List));
  list->length = length;

  Node* head = malloc(sizeof(Node));
  Node* tail = malloc(sizeof(Node));

  Node* current = head;
  current->next = current->prev = NULL;
  current->value = sequence[0];

  for (int i = 1; i < length; i++) {
    current->next = malloc(sizeof(Node)); /* malloc devolverá la direccion de memoria a lo q apunta */
    current->next->prev = current;
    current->next->next = NULL;
    current->next->value = sequence[i];
    current = current->next;
  }
  tail = current;
  list->head = head;
  list->tail = tail;
  return list;
}


void destroy_list(List* list){
  Node* current = list->head;
  while (current != NULL){
    Node* temp = current->next;
    free(current);
    current = temp;
  }
  free(list);
}


void destroy_tuple(Tuple* ret){
  free(ret->right_node);
  free(ret->left_node);
  free(ret);
}


Node* take_node(List* list, int index){
  Node* current = list->head;
  int cont = 0;
  while (current != NULL) {
    if (cont == index){
      break;
    }
    cont += 1;
    current = current->next;
  }
  return current;
}


Tuple* step_1(Node* node, int* i, int* j){
  Node* current = node;
  while (current->next != NULL){
    current = current->next;
    if (current->value != node->value){
      current = current->prev; /* Vuelvo atras porque no me sirve este nodo*/
      break;
    }
    (*i)++;
  }
  Node* right_node = current;
  current = node;
  while (current->prev != NULL) {
    current = current->prev;
    if (current->value != node->value){
      current = current->next;
      break;
    }
    (*j)++;
  }

  Node* left_node = current;
  Tuple* ret = malloc(sizeof(Tuple));
  ret->left_node = left_node;
  ret->right_node = right_node;
  return ret;
}


Tuple* step_2(Tuple* ret, int* i , int* j){

  while (ret->right_node->next != NULL && ret->left_node->prev != NULL){ /* REVISAR SI NULL AA SE DEBE ELIMINAR DEPSUES DE 3*/
    ret->right_node = ret->right_node->next;
    ret->left_node = ret->left_node->prev;

    if (ret->right_node->value != ret->left_node->value){/* Pero alguno puede seguir siendo igual a su anterior*/

      if (ret->right_node->value == ret->right_node->prev->value){ /* Sigue funcionando la derecha*/
        (*i)++;
        ret->left_node = ret->left_node->next;

        while (ret->right_node->next != NULL){
          ret->right_node = ret->right_node->next;
          if (ret->right_node->value != ret->right_node->prev->value) {/* Ya no sigue funcionando*/
            ret->right_node = ret->right_node->prev;
            return step_2(ret, i , j);
          }
          (*i)++;
        }

        return ret;
      }
      else if (ret->left_node->value == ret->left_node->next->value){
        ret->right_node = ret->right_node->prev;
        (*j)++;

        while (ret->left_node->prev != NULL){
          ret->left_node = ret->left_node->prev;
          if (ret->left_node->value != ret->left_node->next->value) {/* Ya no sigue funcionando*/
            ret->left_node = ret->left_node->next;
            return step_2(ret, i , j);
          }
          (*j)++;
        }

        return ret;
      }
      ret->right_node = ret->right_node->prev;
      ret->left_node = ret->left_node->next;
      return ret;
    }

    (*i)++;
    (*j)++;
  }
  return ret;
}

Info* make_move(List* list, Node* node){ /* NO OLVIDARSE DEL LENGHT DE LA LISTA Y EL INDEX A INSERTAR*/
  int i = 0;
  int j = 0;

  /* Revisamos los grupos de 3 o mas */
  Tuple* ret = step_1(node, &i , &j);

  /* Revisamos si quedan grupos de a dos o mas*/
  if (i+j>1){
    ret = step_2(ret, &i, &j);
  }

  Node* right_node = ret->right_node;
  Node* left_node = ret->left_node;

  // destroy_tuple(ret);

  /* deleted_nodes */
  List* deleted_nodes = malloc(sizeof(List)); /* Creo la lista de lo que elimine */
  deleted_nodes->head = NULL;
  deleted_nodes->tail = NULL;
  deleted_nodes->length = 0;

  /* Si tengo deleted_nodes, estara el nodo insertado adentro, y si no, estara en alpha*/
  if (i+j>1){ /* Si hice whiles, encontre grupos, node_insert queda en deleted*/
      list->length = list->length-(i+j+1);
      deleted_nodes->length = i+j+1;

    /* Eliminacion de los nodos que son iguales*/
    /* Solo si encontre grupos a eliminar, sino la lista se queda igual */
    if (left_node->prev != NULL){
      left_node->prev->next = right_node->next;
    }
    else{
      list->head = right_node->next;
    }

    if (right_node->next != NULL){
      right_node->next->prev = left_node->prev;
    }
    else{
      list->tail = left_node->prev;
    }

    /* Arreglo deleted_nodes (corto enlace con alpha)*/
    deleted_nodes->head = left_node;
    deleted_nodes->head->prev = NULL;
    deleted_nodes->tail = right_node;
    deleted_nodes->tail->next = NULL;
  }

  Info* info = malloc(sizeof(Info));
  info->deleted_nodes = deleted_nodes;
  info->index = j;
  return info;
}


Info* insert(List* list, Node* node_insert, int index){

  if (index >= list->length){
    list->tail->next = node_insert;
    node_insert->prev = list->tail;
    list->tail = node_insert;
    list->tail->next = NULL;
  }

  else{
    /* Obtengo el nodo para insertar el que cree adelante del obtenido. Se asume que indice etá correcto */
    Node* next_node = take_node(list, index); /* puede retornar nulo (lista vacia)*/

    if (next_node == NULL) { /* Revisar que al eliminar ultimo elemento, quede null*/
      list->head = node_insert;
      list->length = 1; /* Ahora tiene un elemento*/
    }
    else{
      if (next_node == list->head){
        list->head = node_insert;
      }
      else{
        next_node->prev->next = node_insert;
      }
      node_insert->prev = next_node->prev;
      next_node->prev = node_insert;
      node_insert->next = next_node;
    }

  }

  list->length++;
  Info* info = make_move(list, node_insert);
  info->index = index - info->index; /*antes info->index era i, entonces a la pos dde inserte el nodo tengo que restarle lo que avanze en i */
  return info;
}

void delete_node(List* list, Node* node){
  if (list->length == 0){ /* El nodo es el unico*/
    list->head = NULL;
  }
  if (node == list->head){
    list->head = node->next;
    list->head->prev = NULL;
  }
  else{
    node->prev->next = node->next;
  }
  if (node == list->tail){
    list->tail = node->prev;
    list->tail->next = NULL;
  }
  else{
    node->next->prev = node->prev;
  }
}

void uninsert(List* list, Node* node, List* deleted_nodes, int index){
  if (deleted_nodes->length == 0){ /* Solo debo eliminar el nodo insertado en alpha*/
    delete_node(list, node); /* Verifico si el nodo insertado es cabeza o cola y lo elimino*/
    list->length--;
  }

  else{
    delete_node(deleted_nodes, node); /* Elimino el nodo insertado de delete nodes*/
    // printf("%d %d\n",index, list->length );

    if (index >= list->length){ /* Si debo insertar al final*/
      list->tail->next = deleted_nodes->head;
      deleted_nodes->head->prev = list->tail;
      list->tail = deleted_nodes->tail;
    }

    else{
      Node* next_node = take_node(list, index);

      if (next_node == NULL){ /* Si la lista habia quedado vacia, list = deleted_nodes*/
        list->head = deleted_nodes->head;
        list->tail = deleted_nodes->tail;
      }
      else{ /* Si la lista aun tiene cosas*/
        if(next_node == list->head){ /* O index == 0*/
          list->head = deleted_nodes->head; /* modifico las cabezas */
        }
        else{
          next_node->prev->next = deleted_nodes->head; /* uno la cabeza de deleted*/
          deleted_nodes->head->prev = next_node->prev;
        }
        deleted_nodes->tail->next = next_node; /* uno la cosa de deleted*/
        next_node->prev = deleted_nodes->tail;
      }

    }
    list->length += deleted_nodes->length - 1;
  }

}
