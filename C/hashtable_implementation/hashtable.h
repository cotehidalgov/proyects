#include <gmp.h>
#include <stdbool.h>

typedef struct tuple{
  mpz_t key;
  int value;
}Tuple;

typedef struct hashtable {
  int size;
  Tuple* table;
  int n_used;
  mpz_t dummy;
}HashTable;


HashTable* create_table();
void destroy_table(HashTable* ht);

void hashtable_insert(HashTable* ht, mpz_t key, int value);

bool hashtable_find(HashTable* ht, mpz_t key, int* value);
