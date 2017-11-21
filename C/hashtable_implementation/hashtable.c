#ifndef HASHTABLE
#define HASHTABLE
#define PERTURB_SHIFT 5

#include "hashtable.h"
#include <stdlib.h>
#include <stdint.h>

HashTable* create_table(){
  HashTable* ht = malloc(sizeof(HashTable));
  ht->n_used = 0;
  ht->size = 27; 
  ht->table = calloc(sizeof(Tuple), 27);
  mpz_init(ht->dummy); // Setea dummy a cero
  return ht;
}

Tuple* get_tuple(HashTable* ht, mpz_t key){
  // Paso a uint key, para luego sacarle el modulo con size para que no sea
  // un numero tan grande y poder buscarlo en la tabla.
  uint32_t perturb = (uint32_t) mpz_get_ui(key);
  uint32_t i,j;
  i = j = perturb % ht->size;
  Tuple next = ht->table[i];

  // Reviso si esa entrada no está vacía y si no tienen la misma key:no me sirve ese espacio y busco otro
  while (mpz_cmp(next.key, ht->dummy) != 0 && mpz_cmp(next.key, key) != 0){
    // Basado en código fuente de python
    perturb >>= PERTURB_SHIFT;
    j = (5*j) + 1 + perturb;
    i = j % ht->size;
    next = ht->table[i];
  }
  // Que este vacia
  // Que tengan misma key (mismo estado)
  // Las dos anteriores
  return &ht->table[i];
}

void hashtable_resize(HashTable* ht){
  Tuple* old_table = ht->table;
  ht->size = ht->size * 3;
  ht->table = calloc(sizeof(Tuple), ht->size);
  ht->n_used = 0;

  for (size_t i = 0; i < ht->size/3; i++) {
    if (mpz_cmp(old_table[i].key, ht->dummy) != 0){
      hashtable_insert(ht, old_table[i].key, old_table[i].value);
    }
    mpz_clear(old_table[i].key);
  }
  free(old_table);

}

void hashtable_insert(HashTable* ht, mpz_t key, int value){
  // Revisar si hay que hacer resize
  if (ht->n_used > 2*ht->size/3){
    hashtable_resize(ht);
  }
  Tuple* t = get_tuple(ht, key); // Esto me devuelve la referencia al lugar donde debo ponerlo
  if (mpz_cmp(t->key, ht->dummy) == 0){
    ht->n_used++;
  }
  mpz_set(t->key, key);
  t->value = value;
}

/* intenta encontrar la entrada con el <key> asociado. Si la encuentra,
asigna el valor en la dirección de <value> y retorna true. En caso contrario
retorn false y no modifica <value> */
bool hashtable_find(HashTable* ht, mpz_t key, int* value){
  // Debo revisar si la entrada encontrada esta vacia
  Tuple* t = get_tuple(ht, key); // Esto me devolverá una entrada valida
  // Si la entrada es vacia: no existe esa key
  if (mpz_cmp(t->key, ht->dummy) == 0) return false;
  // Entrada no vacia: si existe
  *value = t->value;
  return true;
}

void destroy_table(HashTable* ht){
  for (size_t i = 0; i < ht->size; i++) {
    mpz_clear(ht->table[i].key);
  }
  free(ht->table);
  mpz_clear(ht->dummy); // :)
  free(ht);
}
#endif
