#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"


Value *readPar(Value *list, char type){
  Value *newVal = talloc(sizeof(Value));
  // parenthesis are stored as strings "(" or ")"
  // in the s field of the new value
  char *parString = talloc(2*sizeof(char));
  if (type == '('){
    newVal->type = OPEN_TYPE;
    parString[0] = '(';
    parString[1] = '\0';
  }
  else{
    newVal->type = CLOSE_TYPE;
    parString[0] = ')';
    parString[1] = '\0';
  }
  newVal->s = parString;
  return cons(newVal, list);
}


Value *readBoolean(Value *list){
  char next = (char)fgetc(stdin);
  Value *newVal = talloc(sizeof(Value));
  if ((next == 't') || (next == 'f')){
    newVal->type = BOOL_TYPE;
    // booleans are stored as 0 or 1 in the i field
    if (next == 't'){
      newVal->i = 1;
    }
    else{
      newVal->i = 0;
    }
  }
  else{
    printf("Syntax error (readBoolean): boolean was not #t or #f\n");
    texit(1);
  }
  return cons(newVal, list);
}


Value *readString(Value *list){
  char next = (char)fgetc(stdin);
  // strArr will store the string read (without the quotes)
  char *strArr = talloc(301*sizeof(char));
  int index = 0;
  while (next != '"'){
    // raise error if ending quotes are not detected
    if (next == EOF){
      printf("Syntax error (readString): string is not ended with a double quote\n");
      texit(1);
    }
    // read the next character into the string array
    strArr[index] = next;
    next = (char)fgetc(stdin);
    index++;
  }
  // store the string terminator
  strArr[index] = '\0';
  // store the string array in a value and update the parsed list
  Value *newVal = talloc(sizeof(Value));
  newVal->type = STR_TYPE;
  newVal->s = strArr;
  return cons(newVal, list);
}


// check if curChar is a digit
int checkDigit(char curChar){
  if ((curChar >= 48) && (curChar <= 57)){
    return 1;
  }
  else{
    return 0;
  }
}


Value *readNumber(Value *list, char curChar, char prefix, char *nextToProcessInTokenize){
  // initialize the value to be INT_TYPE
  Value *newVal = talloc(sizeof(Value));
  newVal->type = INT_TYPE;
  // this is an array to store the current number
  char *tokenArr = talloc(301*sizeof(char));
  int index = 0;
  int countDot = 0; 

  // read the number's characters into tokenArr
  // note: I did not handle cases like 0.3a, which is
  // not a number or a symbol. In this tokenizer, 
  // it will read 0.3 as a number and a as a symbol
  while (checkDigit(curChar) || (curChar == '.')){
    // if '.' appears, set the value type to double
    if (curChar == '.'){
      // numbers only allow one dot
      if (countDot > 0){
        printf("Syntax error (readNumber): numbers with >1 decimal dots are not allowed\n");
        texit(1);
      }
      newVal->type = DOUBLE_TYPE;
      countDot++;
    }
    // read the char into the tokenArr
    tokenArr[index] = curChar;
    curChar = (char)fgetc(stdin);
    index++;
  }
  tokenArr[index] = '\0';

  // convert from string to number and store it
  char *ptr; // for using strtol and strtod
  if (newVal->type == INT_TYPE){
    long num = strtol(tokenArr, &ptr, 10);//base 10
    if (prefix == '-'){
      num = num * (-1);
    }
    newVal->i = num;
  }
  else{
    double num = strtod(tokenArr, &ptr);
    if (prefix == '-'){
      num = num * (-1);
    }
    newVal->d = num;
  }
  // modify the next character to process in tokenize()
  *nextToProcessInTokenize = curChar;
  return cons(newVal, list);
}


int checkSymbolInitial(char curChar){
  // check if the char is a letter
  if (((curChar >= 97) && (curChar <= 122)) || ((curChar >= 65) && (curChar <= 90))){
    return 1;
  }
  // check if the char is an allowed punctuation
  char puncList[15] = "!$%&*/:<=>?~_^";
  for (int i = 0; i < 14; i++){
    if (curChar == puncList[i]){
      return 1;
    }
  }
  return 0;
}


// check if curChar is a valid subsequent in a symbol
int checkSymbolSubsequent(char curChar){
  // <subsequent> ->  <initial> | <digit> | . | + | -
  if (checkDigit(curChar) || checkSymbolInitial(curChar) || (curChar == '.') || (curChar == '+') || (curChar == '-')){
    return 1;
  }
  else{
    return 0;
  }
}


// need to pass in nextToProcessInTokenize for
// the end checking because we might encounter
// a character that is the end, which must 
// be processed in the next iteration of the
// while loop in tokenize()
int checkSymbolEnd(char curChar, char *nextToProcessInTokenize){
  // Identifiers are delimited by whitespace, comments, parentheses, brackets - did not deal with this, string (double) quotes ( " ), and hash marks( # )
  char endList[7] = "\n\"();#";
  for (int i = 0; i < 6; i++){
    if ((curChar == endList[i]) || (curChar == EOF) || (curChar == ' ')){
      // modify the next character to proceess in the tokenize() function
      *nextToProcessInTokenize = curChar;
      return 1;
    }
  }
  return 0;
}


// reads the symbols + and -
Value *readPlusMinus(Value *list, char curChar){
  char *tokenArr = talloc(2*sizeof(char));
  tokenArr[0] = curChar;
  tokenArr[1] = '\0';
  // store the symbol in the s field of the value
  Value *newVal = talloc(sizeof(Value));
  newVal->type = SYMBOL_TYPE;
  newVal->s = tokenArr;
  return cons(newVal, list);
}


Value *readSymbol(Value *list, char curChar, char *nextToProcessInTokenize){
  // this is an array to store the current symbol
  char *tokenArr = talloc(301*sizeof(char));
  int index = 0; 

  while (checkSymbolEnd(curChar, nextToProcessInTokenize) != 1){
    if (checkSymbolSubsequent(curChar) != 1){
      printf("Syntax error (readSymbol): symbol with %c is not allowed", curChar);
      texit(1);
    }
    tokenArr[index] = curChar;
    curChar = (char)fgetc(stdin);
    index++;
  }
  tokenArr[index] = '\0';

  // store the symbol in the s field of the value
  Value *newVal = talloc(sizeof(Value));
  newVal->type = SYMBOL_TYPE;
  newVal->s = tokenArr;
  return cons(newVal, list);
}


void skipComments(){
  char next = (char)fgetc(stdin);
  // ignore everything after ; in a line
  while ((next != '\n') && (next!= EOF)){
    next = (char)fgetc(stdin);
  }
  return;
}


// Read all of the input from stdin, and return a linked list consisting of the tokens.
Value *tokenize() {
  // use a memory on heap to track the character read
  char *charRead = talloc(2*sizeof(char));
  Value *list = makeNull();
  *charRead = (char)fgetc(stdin);

  while (*charRead != EOF) {
    // read in parentheses
    if ((*charRead == '(' )|| (*charRead == ')')){
      list = readPar(list, *charRead);
    }

    // read in booleans
    else if (*charRead == '#'){
      list = readBoolean(list);
    } 

    // read in string
    else if (*charRead == '"'){
      list = readString(list);
    }

    // read in unsigned numbers
    // i.e. the token will start with digits 0-9 or .
    // whose ASCII character is a decimal value between 48 to 57
    else if (checkDigit(*charRead) || (*charRead == '.')){
      list = readNumber(list, *charRead, '+', charRead);
      // note: continue (skipping to the START of the
      // while loop) is applied after we use
      // readNumber() or readSymbol(), because we 
      // already modified the *charRead inside the function
      continue;
    }

    // read in symbols/numbers starting with '+', '-', '.'
    else if ((*charRead == '+') || (*charRead == '-')){
      char next = (char)fgetc(stdin);
      // expect a number if the next char is a digit
      if (checkDigit(next)){
        list = readNumber(list, next, *charRead, charRead);
        continue;
      }
      // expect a symbol if otherwise
      else{
        list = readPlusMinus(list, *charRead);
        *charRead = next;
        continue;
      }
    }

    // read in symbols
    // i.e. starts with letter/some punctuations
    else if (checkSymbolInitial(*charRead)){
      list = readSymbol(list, *charRead, charRead);
      continue;
    } 

    // comments are skipped
    else if (*charRead == ';'){
      skipComments();
    }  

    // ignore white spaces or next-line characters 
    // (that are not part of strings)
    else if ((*charRead == ' ') || (*charRead == '\n')){
    }  

    // if char is not of any of the types above
    // report a syntax error       
    else {
      printf("Syntax error (tokenize): character %c is not allowed in this tokenizer", *charRead);
      texit(1);      
    }
    // note: once EOF is reached, all future reads are EOF
    *charRead = (char)fgetc(stdin);
  }

  Value *revList = reverse(list);
  return revList;
}


// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list){
  Value *curVal = list;
  while ((curVal->type) != NULL_TYPE){
    switch ((car(curVal))->type){
      case OPEN_TYPE:
        printf("%s:open\n", (car(curVal))->s);
        break;
      case CLOSE_TYPE:
        printf("%s:close\n", (car(curVal))->s);
        break;
      case BOOL_TYPE:
        if ((car(curVal))->i){
          printf("#t:boolean\n");
        }
        else{
          printf("#f:boolean\n");
        }
        break;
      case STR_TYPE:
        printf("\"%s\":string\n", (car(curVal))->s);
        break;
      case INT_TYPE:
        printf("%i:integer\n", (car(curVal))->i);
        break;
      case DOUBLE_TYPE:
        printf("%f:double\n", (car(curVal))->d);  
        break; 
      case SYMBOL_TYPE:
        printf("%s:symbol\n", (car(curVal))->s); 
        break;
      case CONS_TYPE:
      case NULL_TYPE:
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
    curVal = cdr(curVal);
  }
  return;
}