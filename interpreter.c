#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "interpreter.h"


void evaluationError(int type){
  if (type == 0){
    printf("Evaluation error (evalIf): if expression does not have 3 args\n");
  }
  else if (type == 1){
    printf("Evaluation error (evalIf): condition of if expression does not evaluate to a Boolean\n");
  }
  else if (type == 2){
    printf("Evaluation error (checkLetBindings): illegal let/let*/lectrec bindings list\n");
  }
  else if (type == 3){
    printf("Evaluation error (checkLetBody): illegal let/let*/lectrec body\n");
  }
  else if (type == 4){
    printf("Evaluation error (evalLet/evalLet*/evalLetrec): illegal let/let*/letrec arguments (not CONS_TYPE)\n");
  }
  else if (type == 5){
    printf("Evaluation error (evalLet/evalLet*/evalLetrec): let/let*/letrec bindings has duplicate symbols\n");
  }
  else if (type == 6){
    printf("Evaluation error (lookUpSymbol): unbound variable found\n");
  }
  else if (type == 7){
    printf("Evaluation error (eval): not a recognized special form\n");
  }
  else if (type == 8){
    printf("Evaluation error: +, -, =, <, > only takes in real numbers\n");
  }
  else if (type == 9){
    printf("Evaluation error (evalQuote): illegal quote arguments\n");
  }
  else if (type == 10){
    printf("Evaluation error (evalDefine/evalSet): illegal define/set! arguments\n");
  }
  else if (type == 11){
    printf("Evaluation error (evalDefine): symbol already defined in frame\n");
  }
  else if (type == 12){
    printf("Evaluation error (evalLambda): illegal lambda arguments\n");
  }
  else if (type == 13){
    printf("Evaluation error (evalLambda): illegal lambda body\n");
  }

  else if (type == 14){
    printf("Evaluation error (apply): first argument in combination is not a closure\n");
  }
  else if (type == 15){
    printf("Evaluation error (apply): number of actual arguments does not match with number of formal arguments\n");
  }
  else if (type == 16){
    printf("Evaluation error: + only takes integers and reals\n");
  }
  else if (type == 17){
    printf("Evaluation error: null? takes one argument\n");
  }
  else if (type == 18){
    printf("Evaluation error: car takes one argument of type pair\n");
  }
  else if (type == 19){
    printf("Evaluation error: cdr takes one argument of type pair\n");
  }
  else if (type == 20){
    printf("Evaluation error: cons takes two arguments\n");
  }
  else if (type == 21){
    printf("Evaluation error: - takes >=1 arguments\n");
  }
  else{
    printf("Evaluation error\n");
  }
  texit(1);
}


// for the Scheme primitive function "+"
// (but this version does not deal with complex numbers)
// takes in a list of reals (double/integer)
// and return the sum. If any of the arguments are reals,
// returns a real. Else returns an integer.
// we assumed args is already evaluated.
Value *primitiveAdd(Value *args) {
  // args should be a proper list
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveAdd): args is not a list");
  // if args is empty, return 0 (int)
  Value *returnVal = talloc(sizeof(Value));
  if (args->type == NULL_TYPE){
    returnVal->type = INT_TYPE;
    returnVal->i = 0;
    return returnVal;
  }
  double sum = 0;
  int hasDouble = 0; // record the type (0 = not double)
  Value *curArg = args;
  while (curArg->type != NULL_TYPE){
    Value *num = car(curArg);
    // checking type and adding to sum
    if (num->type != INT_TYPE){
      if (num->type == DOUBLE_TYPE){
        hasDouble = 1;
        sum += num->d;
      }
      else{
        evaluationError(16);
      }
    }
    else{
      sum += num->i;
    }
    curArg = cdr(curArg);
  }
  if (!hasDouble){
    returnVal->type = INT_TYPE;
    // typecast sum to int if args only have integers
    returnVal->i = (int) sum;
  }
  else{
    returnVal->type = DOUBLE_TYPE;
    returnVal->d = sum;
  }
  return returnVal;
}


// for the Scheme primitive function "null?"
// takes in one argument, returns true if the argument
// is an empty list and false if otherwise.
// we assumed args is already evaluated.
Value *primitiveCheckNull(Value *args){
  // args should be either CONS_TYPE or NULL_TYPE
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveCheckNull): args is not a list");
  // length of args should be one
  if (args->type == NULL_TYPE || length(args)!= 1){
    evaluationError(17);
  }

  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = BOOL_TYPE;
  // argument is not a list
  if ((car(args))->type != CONS_TYPE){
    returnVal->i = 0;
    return returnVal;
  }
  Value *list = car(car(args));
  // argument is an non-empty list
  if (!((car(list))->type == NULL_TYPE && (cdr(list))->type == NULL_TYPE)){
    returnVal->i = 0;
    return returnVal;
  }
  // argument is an empty list
  returnVal->i = 1;
  return returnVal;
}


// for the Scheme primitive function "car"
// takes in one argument (must be a CONS_TYPE Value)
// and returns a pointer to the Value stored in car
Value *primitiveCar(Value *args){
  // args should be either CONS_TYPE or NULL_TYPE
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveCar): args is not a list");
  // length of args should be one
  if (args->type == NULL_TYPE || length(args)!= 1){
    evaluationError(18);
  }
  if (car(args)->type != CONS_TYPE){
    evaluationError(18);
  }
  Value *list = car(car(args));
  // argument is an empty list
  if ((car(list))->type == NULL_TYPE && (cdr(list))->type == NULL_TYPE){
    evaluationError(18);
  }
  // argument is not an empty list - then return the car
  // consing with a NULL cell is for printing formatting
  return cons(car(list), makeNull());
}


// for the Scheme primitive function "cdr"
// takes in one argument (must be a CONS_TYPE Value)
// and returns a pointer to the Value stored in cdr
Value *primitiveCdr(Value *args){
  // args should be either CONS_TYPE or NULL_TYPE
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveCdr): args is not a list");
  // length of args should be one
  if (args->type == NULL_TYPE || length(args)!= 1){
    evaluationError(19);
  }
  if (car(args)->type != CONS_TYPE){
    evaluationError(19);
  }
  Value *list = car(car(args));
  // argument is an empty list
  if ((car(list))->type == NULL_TYPE && (cdr(list))->type == NULL_TYPE){
    evaluationError(19);
  }
  // argument is not an empty list - then return the cdr
  // consing with a NULL cell is for printing formatting
  if ((cdr(list))->type == NULL_TYPE){
    return cons(cons(makeNull(), makeNull()), makeNull());
  }
  return cons(cdr(list), makeNull());
}


// for the Scheme function "cons"
// takes in two arguments and returns a CONS_TYPE Value
// whose car is the CONS_TYPE Value we want 
// (where the car is the first argument and cdr is the second)
Value *primitiveCons(Value *args){
  // args should be either CONS_TYPE or NULL_TYPE
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveCons): args is not a list");
  // length of args should be 2
  if (args->type == NULL_TYPE || length(args)!= 2){
    evaluationError(20);
  }
  Value *newCar = car(args);
  Value *newCdr = car(cdr(args));
  // un-nesting for printing formatting
  if (newCar->type == CONS_TYPE){
    newCar = car(newCar);
  }
  if (newCdr->type == CONS_TYPE){
    newCdr = car(newCdr);
  }
  // consing with a NULL cell for printing out the outer parenthesis
  return cons(cons(newCar, newCdr), makeNull());
}


// for the Scheme primitive function "=" 
// (but this version does not deal with complex numbers)
// takes in a list of reals (double/integer)
// and return true if they are numerically equal.
// we assumed args is already evaluated.
Value *primitiveEqual(Value *args) {
  // args should be a proper list
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveEqual): args is not a list");
  // if args is empty, return true
  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = BOOL_TYPE;
  if (args->type == NULL_TYPE){
    returnVal->i = 1;
    return returnVal;
  }

  Value *firstNum = car(args);
  double firstNumVal;
  // check the first number's type is correct
  if ((firstNum->type != DOUBLE_TYPE) && (firstNum->type != INT_TYPE)){
    evaluationError(8);
  }
  // if there is only one number, return true
  if (length(args) < 2){
    returnVal->i = 1;
    return returnVal;
  }
  // get the first number's value for comparison
  if (firstNum->type == DOUBLE_TYPE){
    firstNumVal = firstNum->d;
  }
  else{
    firstNumVal = (firstNum->i)*1.0; // cast integer to double
  }
  // compare all other numbers in the args list to the first number
  Value *curArg = cdr(args);
  while (curArg->type != NULL_TYPE){
    Value *curNum = car(curArg);
    double curNumVal;
    // check the current number's type is correct
    if (curNum->type != DOUBLE_TYPE && curNum->type != INT_TYPE){
      evaluationError(8);
    }
    // get the current number's value
    if (curNum->type == DOUBLE_TYPE){
      curNumVal = curNum->d;
    }
    else{
      curNumVal = (curNum->i)*1.0;
    }
    // if current number is not equal to the first number, return false
    if (curNumVal != firstNumVal){
      returnVal->i = 0;
      return returnVal;
    }
    curArg = cdr(curArg);
  }
  // if all other numbers are equal to the first number, return true
  returnVal->i = 1;
  return returnVal;
}


// for the Scheme primitive function "-" 
// (but this version does not deal with complex numbers)
// takes in a list of reals (double/integer).
// if there is only one number, return the 
// additive inverse of num. Otherwise, return the
// difference between num1 and the sum of num2 num3 ...
Value *primitiveMinus(Value *args) {
  // args should be a proper list
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveMinus): args is not a list");
  Value *returnVal = talloc(sizeof(Value));
  // must take >=1 arguments
  if (args->type == NULL_TYPE){
    evaluationError(21);
  }
  Value *firstNum = car(args);
  int hasDouble = 0;
  double firstNumVal;
  // check the first number's type is correct
  if ((firstNum->type != DOUBLE_TYPE) && (firstNum->type != INT_TYPE)){
    evaluationError(8);
  }
  // get the first number's value
  if (firstNum->type == DOUBLE_TYPE){
    firstNumVal = firstNum->d;
    hasDouble = 1;
  }
  else{
    firstNumVal = (firstNum->i)*1.0; // cast integer to double
  }

  // if there is only one number
  if (length(args) == 1){
    // return the negative of the first number
    if (!hasDouble){
      returnVal->type = INT_TYPE;
      returnVal->i = (int) (firstNumVal*(-1)); // cast back to an integer
    }
    else{
      returnVal->type = DOUBLE_TYPE;
      returnVal->d = firstNumVal*(-1);
    }
    return returnVal;
  }
  // multiple numbers: calculate the sum of all other
  // numbers and then its difference with the first num
  else{
    Value *sumOther = primitiveAdd(cdr(args));
    double sumOtherVal;
    if (sumOther->type == DOUBLE_TYPE){
      hasDouble = 1;
      sumOtherVal = sumOther->d;
    }
    else{
      sumOtherVal = sumOther->i;
    }

    if (!hasDouble){
      returnVal->type = INT_TYPE;
      returnVal->i = (int) (firstNumVal - sumOtherVal);
    }
    else{
      returnVal->type = DOUBLE_TYPE;
      returnVal->d = firstNumVal - sumOtherVal;
    }
    return returnVal;
  }
}


// for the Scheme primitive function "<" 
// takes in a list of reals (double/integer)
// and return true if its arguments are monotonically
// increasing, i.e., each argument is greater than the preceding ones
Value *primitiveLessThan(Value *args) {
  // args should be a proper list
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveLessThan): args is not a list");
  // if args is empty, return true
  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = BOOL_TYPE;
  if (args->type == NULL_TYPE){
    returnVal->i = 1;
    return returnVal;
  }

  Value *firstNum = car(args);
  double firstNumVal;
  // check the first number's type is correct
  if ((firstNum->type != DOUBLE_TYPE) && (firstNum->type != INT_TYPE)){
    evaluationError(8);
  }
  // if there is only one number, return true
  if (length(args) < 2){
    returnVal->i = 1;
    return returnVal;
  }
  // get the first number's value for comparison
  if (firstNum->type == DOUBLE_TYPE){
    firstNumVal = firstNum->d;
  }
  else{
    firstNumVal = (firstNum->i)*1.0; // cast integer to double
  }
  // check if each argument is greater than the preceding one
  Value *curArg = cdr(args);
  double prevNumVal = firstNumVal;
  while (curArg->type != NULL_TYPE){
    Value *curNum = car(curArg);
    double curNumVal;
    // check the current number's type is correct
    if (curNum->type != DOUBLE_TYPE && curNum->type != INT_TYPE){
      evaluationError(8);
    }
    // get the current number's value
    if (curNum->type == DOUBLE_TYPE){
      curNumVal = curNum->d;
    }
    else{
      curNumVal = (curNum->i)*1.0;
    }
    // if current number is not greater than the previous number, return false
    if (!(curNumVal > prevNumVal)){
      returnVal->i = 0;
      return returnVal;
    }
    prevNumVal = curNumVal;
    curArg = cdr(curArg);
  }
  returnVal->i = 1;
  return returnVal;
}


// for the Scheme primitive function ">" 
// takes in a list of reals (double/integer)
// and return true if its arguments are monotonically
// decreasing, i.e., each argument is greater than the preceding ones
Value *primitiveGreaterThan(Value *args) {
  // args should be a proper list
  assert((args->type == CONS_TYPE || args->type == NULL_TYPE) && "Error (primitiveGreaterThan): args is not a list");
  // if args is empty, return true
  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = BOOL_TYPE;
  if (args->type == NULL_TYPE){
    returnVal->i = 1;
    return returnVal;
  }

  Value *firstNum = car(args);
  double firstNumVal;
  // check the first number's type is correct
  if ((firstNum->type != DOUBLE_TYPE) && (firstNum->type != INT_TYPE)){
    evaluationError(8);
  }
  // if there is only one number, return true
  if (length(args) < 2){
    returnVal->i = 1;
    return returnVal;
  }
  // get the first number's value for comparison
  if (firstNum->type == DOUBLE_TYPE){
    firstNumVal = firstNum->d;
  }
  else{
    firstNumVal = (firstNum->i)*1.0; // cast integer to double
  }
  // check if each argument is less than the preceding one
  Value *curArg = cdr(args);
  double prevNumVal = firstNumVal;
  while (curArg->type != NULL_TYPE){
    Value *curNum = car(curArg);
    double curNumVal;
    // check the current number's type is correct
    if (curNum->type != DOUBLE_TYPE && curNum->type != INT_TYPE){
      evaluationError(8);
    }
    // get the current number's value
    if (curNum->type == DOUBLE_TYPE){
      curNumVal = curNum->d;
    }
    else{
      curNumVal = (curNum->i)*1.0;
    }
    // if current number is not smaller than the previous number, return false
    if (!(curNumVal < prevNumVal)){
      returnVal->i = 0;
      return returnVal;
    }
    prevNumVal = curNumVal;
    curArg = cdr(curArg);
  }
  returnVal->i = 1;
  return returnVal;
}


// Adds a binding between the given name (a string)
// and the pointer to the input function. Used to add
// bindings for primitive funtions to the top-level
// bindings list. Each binding is a two-item list.
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
  Value *nameVal = talloc(sizeof(Value));
  nameVal->type = SYMBOL_TYPE;
  nameVal->s = name;
  Value *functionVal = talloc(sizeof(Value));
  functionVal->type = PRIMITIVE_TYPE;
  functionVal->primFn = function;
  // create new binding and add to frame
  Value *newBinding = cons(nameVal, cons(functionVal, makeNull()));
  frame->bindings = cons(newBinding, frame->bindings);
}


// initialize the top-level Frame, where there are no
// bindings (bindings is an empty list) and the parent is NULL;
Frame *initTopFrame(){
  Frame *topFrame = talloc(sizeof(Frame));
  topFrame->bindings = makeNull();
  topFrame->parent = NULL;
  return topFrame;
}


//Interprets each top level S-expression in the tree
//and prints out the results.
void interpret(Value *tree){
  Value *curExpr = tree;
  Value* result;
  Frame *topFrame = initTopFrame();
  // binding primitive functions to their Scheme names
  bind("+", primitiveAdd, topFrame);
  bind("null?", primitiveCheckNull, topFrame);
  bind("car", primitiveCar, topFrame);
  bind("cdr", primitiveCdr, topFrame);
  bind("cons", primitiveCons, topFrame);
  bind("=", primitiveEqual, topFrame);
  bind("-", primitiveMinus, topFrame);
  bind("<", primitiveLessThan, topFrame);
  bind(">", primitiveGreaterThan, topFrame);
  // the tree should be either NULL_TYPE or CONS_TYPE
  // as created by the parser
  while (curExpr->type != NULL_TYPE){
    result = eval(car(curExpr), topFrame);
    // using the printTree() function in the parser
    printTree(result);
    printf("\n");
    curExpr = cdr(curExpr);
  }
  return;
}


// returns a Value that the if statement evaluates to
Value *evalIf(Value *args, Frame *frame){
  if (args->type != CONS_TYPE){
    evaluationError(0);
  }
  // check args length
  if (length(args) != 3){
    evaluationError(0);
  }
  Value *test = car(args);
  Value *conseq = car(cdr(args));
  Value *alt = car(cdr(cdr(args)));
  Value *testValue = eval(test, frame);
  // test should evaluate to a boolean?
  if (testValue->type != BOOL_TYPE){
    evaluationError(1);
  }
  if (testValue->i == 0){
    return eval(alt, frame);
  }
  else{
    return eval(conseq, frame);
  }
}


// check if a binding is legal (must be a list of length
// 2) and the car must be SYMBOL_TYPE
void checkOneBinding(Value *binding){
  if (binding->type != CONS_TYPE){
    evaluationError(2);
  }
  else{
    if ((car(binding))->type != CONS_TYPE){
      evaluationError(2);
    }
    if (length(car(binding)) != 2){
      evaluationError(2);
    }
    if ((car(car(binding)))->type != SYMBOL_TYPE){
      evaluationError(2);
    }
  }
  return;
}


// check the binding list passed into the evalLet() function
void checkLetBindings(Value *args){
  // first argument should be the biding list 
  // which is a nested list
  Value *bindingsList = car(args);
  // check if binding list is a CONS_TYPE
  if (bindingsList->type != CONS_TYPE){
    evaluationError(2);
  }
  else{
    // this is to account for the empty bindings list (which is allowed)
    if (((car(bindingsList))->type == NULL_TYPE) && ((cdr(bindingsList))->type == NULL_TYPE)){
      return;
    }
    // each binding should be a list of length 2
    Value *curBinding = bindingsList;
    while (curBinding->type != NULL_TYPE){
      checkOneBinding(curBinding);
      curBinding = cdr(curBinding);
    }
  }
  return;
}


// takes in the arguments of let (including the bindings)
// and check if all bodies are CONS_TYPE
void checkLetBody(Value *args){
  // let should have at least one body
  if (length(args) <= 1){
    evaluationError(3);
  }
  Value *curBody = cdr(args);
  while (curBody->type != NULL_TYPE){
    if (curBody->type != CONS_TYPE){
      evaluationError(3);
    }
    curBody = cdr(curBody);
  }
  return;
}


Value *evalLet(Value *args, Frame *frame){
  if (args->type != CONS_TYPE){
    evaluationError(4);
  }
  checkLetBindings(args);
  checkLetBody(args);

  Value *bindingsList = car(args);
  Value *body = cdr(args);
  Frame *localFrame = talloc(sizeof(Frame));
  Value *localBindings = makeNull();
  
  // if bindings list is empty, it should be stored
  // as just a NULL_TYPE Value in the local frame
  // or else, we need to add the bindings to the local frame
  if (!(((car(bindingsList))->type == NULL_TYPE) && ((cdr(bindingsList))->type == NULL_TYPE))){
    // loop through all pairs of bindings and add to 
    // localBindings (for the local frame)
    Value *curBinding = bindingsList;
    while (curBinding->type != NULL_TYPE){
      Value *curSymbol = car(car(curBinding));
      // evaluate the binding in the passed in frame
      Value *curSymbolValue = eval(car(cdr(car(curBinding))), frame);
      // checking whether the symbol is already defined
      // previously in the bindings list of the let frame
      Value *curLocalBinding = localBindings;
      while (curLocalBinding->type != NULL_TYPE){
        if (strcmp(curSymbol->s, car(car(curLocalBinding))->s)==0){
          evaluationError(5);
        }
        curLocalBinding = cdr(curLocalBinding);
      }

      // create new binding and add to localBindings
      Value *newBinding = cons(curSymbol, cons(curSymbolValue, makeNull()));
      localBindings = cons(newBinding, localBindings);

      curBinding = cdr(curBinding);
    }
  }

  // evaluate the bodies in the local frame
  localFrame->bindings = localBindings;
  localFrame->parent = frame;
  Value *curBody = body;
  Value *result;
  while (curBody->type != NULL_TYPE){
    // evaluate each body
    result = eval(car(curBody), localFrame);
    curBody = cdr(curBody);
  }
  // return the last body
  return result;
}


Value *evalLetStar(Value *args, Frame *frame){
  // same error checking as in evalLet
  if (args->type != CONS_TYPE){
    evaluationError(4);
  }
  checkLetBindings(args);
  checkLetBody(args);

  Value *bindingsList = car(args);
  Value *body = cdr(args);
  Frame *localFrame = talloc(sizeof(Frame));
  Value *localBindings = makeNull();
  localFrame->bindings = localBindings;
  localFrame->parent = frame;

  // if bindings list is empty, it should be stored
  // as just a NULL_TYPE Value in the local frame
  // or else, we need to add the bindings to the local frame one by one
  if (!(((car(bindingsList))->type == NULL_TYPE) && ((cdr(bindingsList))->type == NULL_TYPE))){
    // loop through all pairs of bindings and add to 
    // localBindings after each evaluation
    Value *curBinding = bindingsList;
    while (curBinding->type != NULL_TYPE){
      Value *curSymbol = car(car(curBinding));
      // evaluate the binding in the new local frame
      Value *curSymbolValue = eval(car(cdr(car(curBinding))), localFrame);
      // checking whether the symbol is already defined
      // previously in the bindings list of the new frame
      Value *curLocalBinding = localBindings;
      while (curLocalBinding->type != NULL_TYPE){
        if (strcmp(curSymbol->s, car(car(curLocalBinding))->s)==0){
          evaluationError(5);
        }
        curLocalBinding = cdr(curLocalBinding);
      }

      // create new binding and add to localBindings
      Value *newBinding = cons(curSymbol, cons(curSymbolValue, makeNull()));
      localBindings = cons(newBinding, localBindings);
      // update the bindings list in the frame
      localFrame->bindings = localBindings;

      curBinding = cdr(curBinding);
    }
  }
  // evaluate the bodies in the local frame
  Value *curBody = body;
  Value *result;
  while (curBody->type != NULL_TYPE){
    // evaluate each body
    result = eval(car(curBody), localFrame);
    curBody = cdr(curBody);
  }
  // return the last body
  return result;
}


Value *evalLetrec(Value *args, Frame *frame){
  // same error checking as in evalLet()
  if (args->type != CONS_TYPE){
    evaluationError(4);
  }
  checkLetBindings(args);
  checkLetBody(args);

  Value *bindingsList = car(args);
  Value *body = cdr(args);
  Frame *localFrame = talloc(sizeof(Frame));
  Value *localBindings = makeNull();
  localFrame->bindings = localBindings;
  localFrame->parent = frame;
  
  // if bindings list is empty, it should be stored
  // as just a NULL_TYPE Value in the local frame
  // or else, we need to add the bindings to the local frame
  if (!(((car(bindingsList))->type == NULL_TYPE) && ((cdr(bindingsList))->type == NULL_TYPE))){
    // loop through all pairs of letrec bindings
    Value *curBinding = bindingsList;
    while (curBinding->type != NULL_TYPE){
      Value *curSymbol = car(car(curBinding));
      // evaluate the binding in the new local frame
      Value *curSymbolValue = eval(car(cdr(car(curBinding))), localFrame);
      // checking whether the symbol is already defined
      // previously in the bindings list of the new frame
      Value *curLocalBinding = localBindings;
      while (curLocalBinding->type != NULL_TYPE){
        if (strcmp(curSymbol->s, car(car(curLocalBinding))->s)==0){
          evaluationError(5);
        }
        curLocalBinding = cdr(curLocalBinding);
      }

      // create new binding and add to localBindings
      // (but does not yet update the bindings in the new frame)
      Value *newBinding = cons(curSymbol, cons(curSymbolValue, makeNull()));
      localBindings = cons(newBinding, localBindings);

      curBinding = cdr(curBinding);
    }
  }

  // evaluate the bodies in the local frame
  localFrame->bindings = localBindings;
  Value *curBody = body;
  Value *result;
  while (curBody->type != NULL_TYPE){
    // evaluate each body
    result = eval(car(curBody), localFrame);
    curBody = cdr(curBody);
  }
  // return the last body
  return result;
}


// returns the value of expr in the environment
// expr passed in must be SYMBOL_TYPE
Value *lookUpSymbol(Value *expr, Frame *frame){
  Value *curBinding = frame->bindings;
  // look up in the curParament frame
  while (curBinding->type != NULL_TYPE){
    if (strcmp(expr->s, car(car(curBinding))->s)==0){
      return car(cdr(car(curBinding)));
    }
    curBinding = cdr(curBinding);
  }
  // look up in the parent frame
  if (frame->parent != NULL){
    return lookUpSymbol(expr, frame->parent);
  } 
  else{
    // unbound variable
    evaluationError(6);
  }
  return NULL;
}


// returns the Value whose car is the value of expr
// in the environment
Value *getSymbolBinding(Value *expr, Frame *frame){
  Value *curBinding = frame->bindings;
  // look up in the curParament frame
  while (curBinding->type != NULL_TYPE){
    if (strcmp(expr->s, car(car(curBinding))->s)==0){
      return cdr(car(curBinding));
    }
    curBinding = cdr(curBinding);
  }
  // look up in the parent frame
  if (frame->parent != NULL){
    return lookUpSymbol(expr, frame->parent);
  } 
  else{
    // unbound variable
    evaluationError(6);
  }
  return NULL;
}


Value *evalQuote(Value *args){
  // sanity checking
  if (args->type != CONS_TYPE){
    evaluationError(9);
  }
  if (length(args) != 1){
    evaluationError(9);
  }
  if (car(args)->type == CONS_TYPE){
    return args;
  }
  return car(args);
}


// checks whether the arguments of define is a list of
// two items, where the first item is a symbol
void checkDefineArgs(Value *args){
  if (args->type != CONS_TYPE){
    evaluationError(10);
  }
  if (length(args) != 2){
    evaluationError(10);
  }
  if ((car(args))->type != SYMBOL_TYPE){
    evaluationError(10);
  }
  return;
}


// creates binding in the top-level frame
// if frame passed in is not the top-level frame,
// define will just create bindings in the frame passed in
// if symbol already exists, error will be raised
Value *evalDefine(Value *args, Frame *frame){
  checkDefineArgs(args);
  Value *var = car(args);
  Value *expr = eval(car(cdr(args)), frame); //expr is evaluated

  // first check if the variable is already in 
  // the current frame. if so, raise error
  Value *curLocalBinding = frame->bindings;
  while (curLocalBinding->type != NULL_TYPE){
    if (strcmp(var->s, car(car(curLocalBinding))->s)==0){
      evaluationError(11);
    }
    curLocalBinding = cdr(curLocalBinding);
  }

  // add new binding to the frame
  Value *binding = cons(var, cons(expr, makeNull()));
  frame->bindings = cons(binding, frame->bindings);
  // return a VOID_TYPE value to avoid printing
  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = VOID_TYPE;
  return returnVal;
}


void checkLambdaParam(Value *args){
  if (args->type != CONS_TYPE){
    evaluationError(12);
  }
  // lambda must have at least one body
  if (length(args) <= 1){
    evaluationError(12);
  }
 
  Value *paramList = car(args);
  // check if param list is a list
  if (paramList->type != CONS_TYPE){
    evaluationError(12);
  }
  else{
    // this is to account for the empty param list (which is allowed)
    if (((car(paramList))->type == NULL_TYPE) && ((cdr(paramList))->type == NULL_TYPE)){
      return;
    }

    // each item in paramList should be SYMBOL_TYPE
    Value *curParam = paramList;
    while (curParam->type != NULL_TYPE){
      if ((car(curParam))->type != SYMBOL_TYPE){
        evaluationError(12);
      }
      curParam = cdr(curParam);
    }

    //check if parameters are all distinct
    curParam = paramList;    
    while (curParam->type != NULL_TYPE){
      Value *compare = cdr(curParam);
      while (compare->type != NULL_TYPE){
        if (strcmp(car(compare)->s, car(curParam)->s)==0){
          //found duplicate identifiers in param list
          evaluationError(12);
        }
        compare = cdr(compare);
      }
      curParam = cdr(curParam);
    }
  }
  return;
}


void checkLambdaBody(Value *args){
  //args should be CONS_TYPE and of length >1
  //(checkLambdaParam has already dealth with this)
  Value *curBody = cdr(args);
  while (curBody->type != NULL_TYPE){
    if (curBody->type != CONS_TYPE){
      evaluationError(13);
    }
    curBody = cdr(curBody);
  }
  return;
}


// returns a closure representing the function containing // (1) a list of formal parameter names; 
// (2) a pointer to the function body; 
// (3) a pointer to the environment frame in which the function was created
Value *evalLambda(Value *args, Frame *frame){
  checkLambdaParam(args);
  checkLambdaBody(args);

  Value *paramList = car(args);
  Value *body = cdr(args); // note: we can have >1 body!
  Value *fn = talloc(sizeof(Value));
  fn->type = CLOSURE_TYPE;
  fn->closure.paramNames = paramList;
  fn->closure.fnBody = body;
  fn->closure.frame = frame;
  return fn; 
}


// alters existing binding in the environment
Value *evalSet(Value *args, Frame *frame){
  // same error checking as Define (same syntax)
  checkDefineArgs(args);
  Value *var = car(args);
  Value *expr = eval(car(cdr(args)), frame); //expr is evaluated

  // get the Value whose car is the value of var 
  // in the environment, if symbol does not exists, 
  // error will be raised by getSymbolBinding()
  Value *oldVal = getSymbolBinding(var, frame);
  (oldVal->c).car = expr;

  // return a VOID_TYPE value to avoid printing
  Value *returnVal = talloc(sizeof(Value));
  returnVal->type = VOID_TYPE;
  return returnVal;
}


// evaluates a sequence of expressions in order
// return the value of the last expression
// args should be CONS_TYPE or NULL_TYPE
Value *evalBegin(Value *args, Frame *frame){
  // if there is no argument to evaluate, return VOID_TYPE
  if (length(args) == 0){
    Value *returnVal = talloc(sizeof(Value));
    returnVal->type = VOID_TYPE;
    return returnVal;
  }
  Value *curExpr = args;
  Value *result;
  while (curExpr->type != NULL_TYPE){
    // evaluate each expression
    result = eval(car(curExpr), frame);
    curExpr = cdr(curExpr);
  }
  // return the last body
  return result;
}


// evaluates all expressions in order until one is #f
// and returns #f in that case. Return the last expression
// if no expressions evaluate to #f
// args should be CONS_TYPE or NULL_TYPE
Value *evalAnd(Value *args, Frame *frame){
  // if there are no arguments, return true
  if (length(args) == 0){
    Value *returnVal = talloc(sizeof(Value));
    returnVal->type = BOOL_TYPE;
    returnVal->i = 1;
    return returnVal;  
  }
  // evaluate the expressions in order
  Value *curExpr = args;
  Value *result;
  while (curExpr->type != NULL_TYPE){
    // evaluate each expression
    result = eval(car(curExpr), frame);
    // if the current expression evals to #f, return #f
    if (result->type == BOOL_TYPE && result->i == 0){
      return result;  
    }
    curExpr = cdr(curExpr);
  }
  // return the last expression's value
  return result;
}


// evaluates all expressions in order until one is not #f
// and returns that value in that case. 
// Return #f if all expressions evaluate to #f
// args should be CONS_TYPE or NULL_TYPE
Value *evalOr(Value *args, Frame *frame){
  // if there are no arguments, return false
  if (length(args) == 0){
    Value *returnVal = talloc(sizeof(Value));
    returnVal->type = BOOL_TYPE;
    returnVal->i = 0;
    return returnVal;  
  }
  // evaluate the expressions in order
  Value *curExpr = args;
  Value *result;
  while (curExpr->type != NULL_TYPE){
    // evaluate each expression
    result = eval(car(curExpr), frame);
    // if current expression evals to not #f, return it
    if (!(result->type == BOOL_TYPE && result->i == 0)){
      return result;  
    }
    curExpr = cdr(curExpr);
  }
  // return #f
  return result;
}


//returns a list with all the items in args evaluated (in order)
Value *evalArgs(Value *args, Frame *frame){
  Value *evaledArgList = makeNull();
  Value *curArg = args;
  while (curArg->type != NULL_TYPE) {
    Value *curArgEvaled = eval(car(curArg), frame);
    evaledArgList = cons(curArgEvaled, evaledArgList);
    curArg = cdr(curArg);
  }
  evaledArgList = reverse(evaledArgList);
  return evaledArgList;
}


//Evaluate the function body (found in the closure) with the new frame as its environment, and return the result of the call to eval.
Value *apply(Value *function, Value *args){
  // apply primitive functions
  if (function->type == PRIMITIVE_TYPE){
    return (*(function->primFn))(args);
  }
  // for closures
  if (function->type != CLOSURE_TYPE){
    evaluationError(14);
  }
  // Construct a new frame with parent being 
  // the environment of the closure
  Frame *fnFrame = talloc(sizeof(Frame));
  fnFrame->parent = (function->closure).frame;

  // add bindings to the new frame
  // mapping each formal param in closure
  // to the actual params in args
  Value *curFormal = (function->closure).paramNames;
  Value *curActual = args;
  Value *localBindings = makeNull();

  // a variable marking whether the 
  // function's argument list is empty (1 is empty)
  int emptyArgs = 0; 
  if (length(curFormal) != length(curActual)){
    if ((car(curFormal)->type) != NULL_TYPE){
        // not the same number of formal and actual
        // arguments (and argument list is non-empty)
      evaluationError(15);
    }
    else{
      // argument list is empty
      emptyArgs = 1;
    }
  }
  
  // binding step
  if (!emptyArgs){
    while (curFormal->type != NULL_TYPE){
      Value *oneBinding = cons(car(curFormal), cons(car(curActual), makeNull()));
      localBindings = cons(oneBinding, localBindings);
      curFormal = cdr(curFormal);
      curActual = cdr(curActual);
    }
  }

  fnFrame->bindings = localBindings;
  Value *curBody = (function->closure).fnBody;
  Value *result;
  // here we assumed fnBody have at least one body
  // this should be checked while evaulating lambda
  while (curBody->type != NULL_TYPE){
    result = eval(car(curBody), fnFrame);
    curBody = cdr(curBody);
  }
  return result; // result of last body returned
}


//Evaluates the S-expression referred to by expr
//in the given frame.
Value *eval(Value *expr, Frame *frame) {
  switch (expr->type)  {

    case NULL_TYPE:
    case PTR_TYPE:
    case OPENBRACKET_TYPE:
    case CLOSEBRACKET_TYPE:
    case DOT_TYPE:
    case SINGLEQUOTE_TYPE:
    case OPEN_TYPE:
    case CLOSE_TYPE:
    case VOID_TYPE:
    case CLOSURE_TYPE:
    case PRIMITIVE_TYPE:
      break;  

    case INT_TYPE: 
    case DOUBLE_TYPE:
    case STR_TYPE:
    case BOOL_TYPE:
    // evaluates to itself
    return expr;
    break;

    case SYMBOL_TYPE: {
      return lookUpSymbol(expr, frame);
      break;
    }  
    
    // CONS_TYPE means the expression is a combination
    // (including special forms)
    case CONS_TYPE: {
      Value *first = car(expr);
      Value *args = cdr(expr);
      if (first->type == SYMBOL_TYPE){
        // special forms
        if (!strcmp(first->s,"if")) {
          return evalIf(args,frame);
        }
        else if (!strcmp(first->s,"let")){
          return evalLet(args,frame);
        }
        else if (!strcmp(first->s,"let*")){
          return evalLetStar(args,frame);
        }
        else if (!strcmp(first->s,"letrec")){
          return evalLetrec(args,frame);
        }
        else if (!strcmp(first->s,"quote")){
          return evalQuote(args);
        }
        else if (!strcmp(first->s,"define")){
          // note: frame should be top-level frame
          return evalDefine(args, frame);
        }
        else if (!strcmp(first->s,"lambda")){
          return evalLambda(args, frame);
        }
        else if (!strcmp(first->s,"set!")){
          return evalSet(args, frame);
        }
        else if (!strcmp(first->s,"begin")){
          return evalBegin(args, frame);
        }
        else if (!strcmp(first->s,"and")){
          return evalAnd(args, frame);
        }
        else if (!strcmp(first->s,"or")){
          return evalOr(args, frame);
        }
        else {
          //combination
          Value *function = eval(first, frame);
          Value *evaledArgs = evalArgs(args, frame);
          return apply(function, evaledArgs);
        }
      }
      else{
        Value *function = eval(first, frame);
        Value *evaledArgs = evalArgs(args, frame);
        return apply(function, evaledArgs);
      }
      break;
    }
  }
  return NULL;
}