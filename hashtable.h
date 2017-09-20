/**
	hashtable.h

	This declares the HashTable class. A HashTable stores a collection of
	keys with an associated value, which can be queried using get() or exists(),
	and added/updated/removed using set() and remove().

	Internally this uses a table of linked lists to store the items. Each key
	is assigned a "bucket" using a hash function, and this bucket is a linked
	list of all the items whose key hashes to the same value.

	The first item in each linked list is pointed to by buckets[i]. Each
	item in the list has a next pointer, which points to the next item in the
	list. The last item in a list has next = NULL, to denote that there are no
	further items.

	All operations run in linear time in the size of the bucket. If the largest
	bucket has size L, then all operations will require O(L) time. In the best
	case L is O(1), but could be O(n) in the worst case. On average, if there
	are m buckets, then L is O(n/m).
*/

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <Arduino.h>

/**
	To adapt for a different key type, you will need to define a hash function
	and a function to check if two keys are equal.
*/
typedef int KeyType;

/** This should generalize to any value type. */
typedef const char* ValType;

struct HashItem {
	HashItem *next; ///< Pointer to the next item in the linked list
	KeyType key;    ///< Key of this item
	ValType val;    ///< Value of this item

	/**
		Builds a new hash item, with the given key/value.
		next will be set to NULL, making this item an empty linked list.
	*/
	HashItem(KeyType key, ValType val);

	/**
		Deletes this item and the linked list that follows it.
	*/
	~HashItem();
};


class HashTable {
	/**
		A dynamic array of pointers.
		Each entry in buckets is a pointer to a HashItem, which we will use
		as the head of a linked list.
	*/
	HashItem** buckets;

	/**
		Number of buckets (i.e. the size of the buckets array).
	*/
	int nbuckets;

	/**
		Returns a pointer to the item associated with the key, or returns
		NULL if no item in the table has the given key.
	*/
	HashItem* getItem(KeyType key);

public:
	/**
		Builds a new (empty) table with the given number of buckets.
	*/
	HashTable(int nbuckets);
	/**
		Deletes all dynamic memory used by this structure.
	*/
	~HashTable();

	/**
	 	Returns the value associated with the given key, or NULL if not
		in the table.
	*/
	ValType get(KeyType key);

	/**
		Sets the value of the given key, or creates it if it doesn't exist.
	*/
	void set(KeyType key, ValType val);

	/**
	 	Returns true if the key is in the table.
	*/
	bool exists(KeyType key);

	/**
		Removes the key/value from the table (if it exists).
	*/
	void remove(KeyType key);

	/**
		Prints contents. These will be in an arbitrary order.
	*/
	void print();
};

#endif
