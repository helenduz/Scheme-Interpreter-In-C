#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"


// Prints an error message for the two types of parsing
// errors (too many/few closing parentheses) and exit the program
void syntaxError(int type){
  if (type == 0){
    printf("Syntax error: too many close parentheses\n");
  }
  else{
    printf("Syntax error: not enough close parentheses\n");
  }
  texit(1);
}


// push a Value onto a stack (managed as a linked list)
// returns pointer to the stack after pushing the Value
// Note: this method assumes that stack is a linked list // (i.e. always ends with a NULL_TYPE Value)
Value *push(Value *stack, Value *newVal){
  assert(stack != NULL && "Error (push): stack is null");
  assert(newVal != NULL && "Error (push): item is null");
  // add the value to the start of the list
  return cons(newVal, stack);
}


// pops a Value off a stack (maintained as a linked list)
// Takes in a pointer to a stack (a pointer to the head)
// and modifies the stack, and returns a pointer to the popped Value
// Note: Popping off an empty stack is allowed
// (i.e You can pop off the NULL_TYPE Value at the end)
Value *pop(Value **stackPointer){
  assert(*stackPointer != NULL && "Error (pop): stack is null");
  Value *poppedVal;
  if ((*stackPointer)->type == NULL_TYPE){
    // pop off the NULL_TYPE Value
    poppedVal = *stackPointer;
    *stackPointer = NULL;
  }
  else{
    // the first item on the list should be the one that pops out first because it is added last
    poppedVal = car(*stackPointer);
    // modify the stack
    *stackPointer = cdr(*stackPointer);
  }
  return poppedVal;
}


// pop items of the stack until we popped off an open
// parenthesis, adding each item to the head of a 
// new linked list which represents the subtree
// and return this new linked list
Value *popSubtree(Value **stack){
  Value *subtree = makeNull();
  // note that pop() changes the stack variable's
  // value in popSubtree()
  Value *curVal = pop(stack);
  while (curVal->type != OPEN_TYPE){
    // if reach bottom of stack, raise error
    // because there are too many closing parentheses
    if (curVal->type == NULL_TYPE){
      syntaxError(0);
    }
    subtree = cons(curVal, subtree);
    curVal = pop(stack);
  }
  // this is to account for the empty subtree
  // i.e. an empty s-expression ()
  if (subtree->type == NULL_TYPE){
    subtree = cons(makeNull(), subtree);
  }
  return subtree;
}


// Add the current token to the current tree
// (the tree and the stack is maintained together in 
// the same linked list)
// depth is updated to represent the number of unclosed open parentheses in the parse tree
Value *addToParseTree(Value *tree, int *depth, Value *token){
  // push onto stack if token is not a close parenthesis
  if (token->type != CLOSE_TYPE){
    tree = push(tree, token);
    if (token->type == OPEN_TYPE){
      (*depth)++;
    }
  }
  else{
    // note: we don't need to push the CLOSE_TYPE Value onto the stack
    Value *subtree = popSubtree(&tree);
    // push subtree back onto the stack
    tree = push(tree, subtree);
    // decrease depth by 1
    (*depth)--;
  }
  // return current tree
  return tree;
}


// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens){
  Value *tree = makeNull();
  int depth = 0;

  Value *current = tokens;
  assert(current != NULL && "Error (parse): null pointer");
  while (current->type != NULL_TYPE) {
    Value *token = car(current);
    tree = addToParseTree(tree, &depth, token);
    current = cdr(current);
  }
  if (depth != 0) {
    // if depth is not 0, then there is open parenthesis
    // remaining in the stack, thus raise error
    syntaxError(1);
  }
  // note: tokens are added to the tree in reverse order 
  return reverse(tree);
}


void printAtom(Value *tree){
  switch (tree->type){
    case BOOL_TYPE:
      if (tree->i){
        printf("#t ");
      }
      else{
        printf("#f ");
      }
      break;
    case STR_TYPE:
      printf("\"%s\" ", tree->s);
      break;
    case INT_TYPE:
      printf("%i ", tree->i);
      break;
    case DOUBLE_TYPE:
      printf("%f ", tree->d);  
      break; 
    case SYMBOL_TYPE:
      printf("%s ", tree->s); 
      break;
    case CLOSURE_TYPE:
      printf("#<procedure> ");
      break;
    case CONS_TYPE:
    case NULL_TYPE:
    case PTR_TYPE:
    case OPENBRACKET_TYPE:
    case CLOSEBRACKET_TYPE:
    case DOT_TYPE:
    case SINGLEQUOTE_TYPE:
    case OPEN_TYPE:
    case CLOSE_TYPE:
    case VOID_TYPE:
    case PRIMITIVE_TYPE:
      break;    
  }
} 


// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
// Note: tree passed in should always be either
// CONS_TYPE or NULL_TYPE
void printTree(Value *tree){
  if (tree->type == NULL_TYPE){
    return;
  }
  if (tree->type != CONS_TYPE){
    printAtom(tree);
    return;
  }
  else{
    if ((car(tree))->type == CONS_TYPE){
      printf("( ");
      printTree(car(tree));
      printf(") ");
    }
    else{
      printTree(car(tree));
    }
    if ((cdr(tree))->type != NULL_TYPE){
      if ((cdr(tree))->type != CONS_TYPE){
        printf(". ");
      }
    }
    printTree(cdr(tree));
  }
  return;
}