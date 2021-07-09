#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"


// Create a new NULL_TYPE value node.
Value *makeNull(){
  Value *newVal = talloc(sizeof(Value));
  newVal->type = NULL_TYPE;
  return newVal;
}


// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
  Value *newVal = talloc(sizeof(Value));
  newVal->type = CONS_TYPE;
  // note: car and cdr of a CONS_TYPE Value
  // are pointers to other Values
  (newVal->c).car = newCar;
  (newVal->c).cdr = newCdr;
  return newVal;
}


// Helper function for display():
// Takes in a Value that is not CONS_TYPE or PTR_TYPE and prints the content
void printAtomHelper(Value val){
  switch (val.type) {
  case INT_TYPE:
    printf("%i  ", val.i);
    break;
  case DOUBLE_TYPE:
    printf("%f  ", val.d);
    break;
  case STR_TYPE:
    printf("%s  ", val.s);
    break;
  case NULL_TYPE:
    printf("NULL  ");
    break;
  case OPEN_TYPE:
  case CLOSE_TYPE:
  case BOOL_TYPE:
  case SYMBOL_TYPE:
  case CONS_TYPE:
  case PTR_TYPE:
  case OPENBRACKET_TYPE:
  case CLOSEBRACKET_TYPE:
  case DOT_TYPE:
  case SINGLEQUOTE_TYPE:
  case VOID_TYPE:
  case CLOSURE_TYPE:
  case PRIMITIVE_TYPE:
    break;    
  }
  return;
}


// Helper Function for display():
// does the recursion for display
void displayRecursive(Value *list){
  // print empty list
  if (list->type == NULL_TYPE){
    printf("()");
    return;
  }

  Value thisVal = *list;
  int close = 0;
  // if the Value is an atom, print it using a helper
  if (thisVal.type != CONS_TYPE){
    printAtomHelper(thisVal);
  }
  // if the value is CONS_TYPE, recursively display
  // the car and the cdr
  else{
    // add a "(" if car points to an inner pair
    if ((thisVal.c.car)->type == CONS_TYPE){
      printf("(");
      close = 1;
    }
    // display the car recursively
    displayRecursive(thisVal.c.car);
    
    // display the cdr recursively if thisVal is not the
    // ending node (i.e the cdr does not point to a Value
    // whose type is NULL_TYPE)
    if ((thisVal.c.cdr)->type != NULL_TYPE){
      // if the cdr is an atom, add "."
      if ((thisVal.c.cdr)->type != CONS_TYPE){
        printf(".");
      }
      displayRecursive(thisVal.c.cdr);
    }
    // close off with ")" if needed
    if (close == 1){
      printf(")");
    }
  }
  return;
}


// Display the contents of the linked list to the screen in some kind of
// readable format
// (interpreter 1): note that I assumed each node in the linked list is a cons cell
// and I assumed that we are also displaying nested lists
void display(Value *list){
  // print the empty list
  if (list->type == NULL_TYPE){
    printf("()");
    return;
  }
  printf("(");
  displayRecursive(list);
  printf(")");
  return;
}


// Return a new list that is the reverse of the one that is passed in. The new list will be a new set of CONS_TYPE Values, but the content stored in the car of each CONS_TYPE Value will not be changed.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list){
  // if the list is empty (i.e. head points to a NULL_TYPE
  // Value), return a NULL_TYPE Value
  if (list->type == NULL_TYPE){
    return makeNull();
  }

  // set the current node to be the first node
  Value *curVal = list;
  // save the 2nd node's address
  Value *next = (curVal->c).cdr;
  // create a new node with car pointing to the same 
  // Value that the old car points to and cdr pointing
  // to a NULL_TYPE Value
  Value *newCurVal = cons((curVal->c).car, makeNull());
  // consider the next node
  curVal = next;
  // while the node is not NULL (i.e. the ending)
  // keep creating new nodes with the same car and updated cdr
  while (curVal->type != NULL_TYPE){
    // save the next node's address
    next = (curVal->c).cdr;
    // create a new node with car pointing to the same 
    // Value that the old car points to and cdr pointing 
    //to the previously made new node
    newCurVal = cons((curVal->c).car, newCurVal);
    // move to the next node
    curVal = next;
  }
  // return the address of the lastly-made new node
  return newCurVal;
}


// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
// Make sure have <assert.h> in your #include statements in linkedlist.c in order
// to use assert (see assignment for more details).
Value *car(Value *list){
  assert(list != NULL && "Error (car): input list is NULL");
  assert(list->type == CONS_TYPE && "Error (car): first Value is not of CONS_TYPE");
  return (list->c).car;
}


// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list){
  assert(list != NULL && "Error (cdr): input list is NULL");
  assert(list->type == CONS_TYPE && "Error (cdr): input list is not of CONS_TYPE");
  return (list->c).cdr;
}


// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value){
  assert(value != NULL && "Error (isNull): input pointer is NULL");
  return (value->type == NULL_TYPE);
}


// Measure length of list. Use assertions to make sure that this is a legitimate operation.
// remaining question: if value (a pointer to a Value) is NULL, does that mean it is an empty list or is that invalid?
int length(Value *value){
  assert(value != NULL && "Error (length): input pointer is NULL"); 
  int len = 0;
  while (value->type != NULL_TYPE){
    len++;
    value = (value->c).cdr;
  }
  return len;
}