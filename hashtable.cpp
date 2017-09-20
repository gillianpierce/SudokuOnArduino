#include "hashtable.h"
#include "ArduinoExtras.h"

/**
	A simple hash function for uint32_t keys.

	We need the key to be unsigned, otherwise the mod operator might return
	a negative value (which will cause indexing issues).
*/
int hash(uint32_t key, int nbuckets) {
  return key % nbuckets;
}


HashItem::HashItem(KeyType key, ValType val) {
  this->key = key;
  this->val = val;
  this->next = NULL;
}

HashItem::~HashItem() {
  if (next != NULL)
    delete next;
}

HashTable::HashTable(int nbuckets) {
  this->nbuckets = nbuckets;
  this->buckets = new HashItem*[nbuckets];
  assert( this->buckets != NULL );
  for (int i = 0; i < nbuckets; ++i)
    buckets[i] = NULL;
}

HashTable::~HashTable() {
  for (int i = 0; i < nbuckets; ++i) {
    if (buckets[i] != NULL)
      delete buckets[i];
  }
  delete[] buckets;
}

HashItem* HashTable::getItem(KeyType key) {
  int h = hash(key, nbuckets);
  for (HashItem *it = buckets[h]; it != NULL; it = it->next) {
    if (it->key == key) {
      return it;
    }
  }
  return NULL;
}

ValType HashTable::get(KeyType key) {
  HashItem *it = getItem(key);
  if (it != NULL) return it->val;
  // didn't find it.
  else return NULL;
}

void HashTable::set(KeyType key, ValType val) {
  HashItem *it = getItem(key);
  // Update the key.
  if (it != NULL) {
    it->val = val;
  }
  // Create it.
  else {
    int h = hash(key, nbuckets);
    HashItem *it = new HashItem(key, val);
    assert( it != NULL );
    it->next = buckets[h];
    buckets[h] = it;
  }
}

bool HashTable::exists(KeyType key) {
  return getItem(key) != NULL;
}

void HashTable::remove(KeyType key) {
  int h = hash(key, nbuckets);
  HashItem *it, *prev = NULL;

  for (it = buckets[h]; it != NULL; it = it->next) {
    if (it->key == key) {
      break;
    }
    prev = it;
  }

  // Item not in the table
  if (it == NULL)
    return;
  // Item is the first in the list
  else if (prev == NULL)
    buckets[h] = it->next;
  // Otherwise
  else
    prev->next = it->next;
  it->next = NULL;
  delete it;
}


void HashTable::print() {
  for (int i = 0; i < nbuckets; ++i) {
    for (HashItem *it = buckets[i]; it != NULL; it = it->next) {
      Serial.print(it->key);
      Serial.print(": ");
      Serial.println(it->val);
    }
  }
}
