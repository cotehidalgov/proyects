
#include <stdbool.h>

typedef struct node {
    char value;
    struct node* next;
    struct node* prev;
} Node;

typedef struct list {
  Node* head;
  Node* tail;
  int length;
} List;

typedef struct tuple {
  Node* left_node;
  Node* right_node;
} Tuple;

typedef struct info{
  List* deleted_nodes;
  int index;
} Info;



List* create_list(char* sequence, int length);
void destroy_list(List* list);
void destroy_tuple(Tuple* tuple);
Node* take_node(List* list, int index);
Tuple* step_1(Node* node, int* i, int* j);
Tuple* step_2(Tuple* ret, int* i , int* j);
Info* make_move(List* list, Node* node);
Info* insert(List* list, Node* node_insert, int index);
void uninsert(List* list, Node* node, List* deleted_nodes, int index);
