# Scheme-Interpreter-In-C
## Introduction
This is a command-line interpreter written in C for the functional programming language Scheme. This is an individual project for the _Programming Languages: Design and Implementation_ course at Carleton College. 
 
## How to Run the Program
1. Write the Scheme code to be interpreted in a file with `.scm` extension, and place it in the main folder along with all the source code files.  
2. Compile all files by running `make` in the command line.
3. In the main folder, run the interpreter by typing `./interpreter < some_scheme_code.scm ` (you should replace `some_scheme_code.scm` with the actual Scheme file name). 

## Scripts Logic
The file that is actually run, `interpreter.c`, uses functions, structs, and variables from `value.h`, `linkedlist.h`, `talloc.h`, `tokenizer.h`, and `parser.h`. `value.h` is an implementation of a generic linked list item.

`linkedlist.c` is an implementation of a linked list in C.

`talloc.c` is a memory manager that tracks all heap memory allocations and can free all memory in one call.

`tokenizer.c` is an implementation of a tokenizer that reads a Scheme file and associates each element in the file with its type.

`parser.c` is an implementation of a parser that contructs a parse tree of the Scheme file.  

`interpreter.c` implements selective primitive functions as well as the evaluation of the parse tree. 

## Know Issues
This interpreter does not cover all possible Scheme syntax. 
- _Data types that are not yet supported:_ vectors, bytevector, hashtables
- _Primitive functions and special forms that are not yet suppported:_ `load`, `cond`, `list`, `append`, `equal?`, `display`, `when`, `unless`, `'`(must use `quote` instead of the symbol abbreviation)
