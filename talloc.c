#include <stdlib.h>
#include <assert.h>
#include "value.h"
#include "talloc.h"


// activeListHead is a global variable
// that stores the pointers of memory allocated by
// talloc in the object files in a linked list 
// that uses malloc 
Value *activeListHead;


// Create a new NULL_TYPE value node using malloc.
// This is a duplicate of the makeNull() function in linkedlist.c
Value *makeNullHelper(){
  Value *newVal = malloc(sizeof(Value));
  newVal->type = NULL_TYPE;
  return newVal;
}


// Create a PTR_TYPE value node using malloc.
Value *makePtrHelper(void *p){
  Value *newVal = malloc(sizeof(Value));
  newVal->type = PTR_TYPE;
  newVal->p = p;
  return newVal;
}


// Create a new CONS_TYPE value node using malloc.
// This is a duplicate of the cons() function in linkedlist.c
Value *consHelper(Value *newCar, Value *newCdr){
  Value *newVal = malloc(sizeof(Value));
  newVal->type = CONS_TYPE;
  (newVal->c).car = newCar;
  (newVal->c).cdr = newCdr;
  return newVal;
}


// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size){
  // let head point to a NULL_TYPE node if active list is empty
  if (activeListHead == NULL){
    activeListHead = makeNullHelper();
  }
  // malloc the memory for return
  void *newPointer = malloc(size);
  // store this pointer into the active list by modifying
  // activeListHead, where we insert a new node after the activeListHead
  activeListHead = consHelper(makePtrHelper(newPointer),activeListHead);
  return newPointer;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree(){
  assert(activeListHead != NULL && "Error (tfree): active list is empty");
  Value *cur = activeListHead;
  while (cur->type != NULL_TYPE){
    // save the next node's address
    Value *next = (cur->c).cdr;
    // free up the address stored in the Value pointed to by car, i.e. the talloc'd address
    free(((cur->c).car)->p);
    // free up the Value pointed to by car
    free((cur->c).car);
    // free the CONS_TYPE Value that holds the car and cdr
    free(cur);
    // advance the cur to the next CONS_TYPE Value
    cur = next;
  }
  // free up the final NULL_TYPE Value
  free(cur);
  // make the active list empty
  activeListHead = NULL;
  return;
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status){
  tfree();
  exit(status);
}