M\*LIB: Generic type-safe Container Library for C language
==========================================================

1. [Overview](#overview)
2. [Components](#components)
3. [Build & Installation](#build--installation)
4. [How to use](#how-to-use)
5. [Performance](#performance)
6. [OPLIST](#oplist)
7. [Memory Allocation](#memory-allocation)
8. [Emplace construction](#emplace)
9. [Errors & compilers](#errors--compilers)
10. [External Reference](#external-reference)
11. [API Documentation](#api-documentation)
    1. [Generic methods](#generic-methods)
    2. [List](#m-list)
    3. [Array](#m-array)
    4. [Deque](#m-deque)
    5. [Dictionary](#m-dict)
    6. [Tuple](#m-tuple)
    7. [Variant](#m-variant)
    8. [Red/Black Tree](#m-rbtree)
    9. [B+ Tree](#m-bptree)
    10. [Generic Tree](#m-tree)
    11. [Priority queue](#m-prioqueue)
    12. [Fixed buffer queue](#m-buffer)
    13. [Atomic Shared Register](#m-snapshot)
    14. [Shared pointers](#m-shared)
    15. [Intrusive Shared Pointers](#m-i-shared)
    16. [Intrusive list](#m-i-list)
    17. [Concurrent adapter](#m-concurrent)
    18. [Bitset](#m-bitset)
    19. [String](#m-string)
    20. [Core preprocessing](#m-core)
    21. [Thread](#m-thread)
    22. [Worker threads](#m-worker)
    23. [Atomic](#m-atomic)
    24. [Generic algorithms](#m-algo)
    25. [Function objects](#m-funcobj)
    26. [Exception handling](#m-try)
    27. [Memory pool](#m-mempool)
    28. [JSON Serialization](#m-serial-json)
    29. [Binary Serialization](#m-serial-bin)
12. [Global User Customization](#global-user-customization)
13. [License](#license)


## Overview

M\*LIB (M star lib) is a C library enabling to define and to use **generic and
type safe container** in C, aka handling generic
[containers](https://en.wikipedia.org/wiki/Container_%28abstract_data_type%29) in in pure C language.
The encapsulated objects can have their own constructor, destructor, operators
or can be basic C type like the C type 'int': both are fully supported.
This makes it possible to construct fully
recursive container objects (container-of[...]-container-of-type-T)
while keeping compile time type checking.

This is an equivalent of the [C++](https://en.wikipedia.org/wiki/C%2B%2B)
[Standard Library](https://en.wikipedia.org/wiki/C%2B%2B_Standard_Library),
providing vector, deque, forward_list, set, map, multiset, multimap,
unordered_set, unordered_map, stack, queue, shared_ptr, string, variant, option
to standard ISO C99 / C11.
There is not a strict mapping as both the STL and M\*LIB have their exclusive containers:
See [here](https://github.com/P-p-H-d/mlib/wiki/STL-to-M*LIB-mapping) for details.
M\*LIB provides also additional concurrent containers to design properly
multi-threaded programs: shared register, communication queue, ...

M\*LIB is portable to any systems that support [ISO C99](https://en.wikipedia.org/wiki/C99).
Some optional features need at least [ISO C11](https://en.wikipedia.org/wiki/C11_(C_standard_revision)).

M\*LIB is **only** composed of a set of headers.
There is no C file, and as such, the installation is quite simple:
you just have to put the header in the search path of your compiler,
and it will work.
There is no dependency (except some other headers of M\*LIB and the LIBC).

One of M\*LIB design key is to ensure safety. This is done by multiple means:

* in debug mode, defensive programming is extensively used:
  the contracts of the function are checked, ensuring
  that the data are not corrupted. For example, strict
  [Buffer overflow](https://en.wikipedia.org/wiki/Buffer_overflow) are checked in this mode
  through [bound checking](https://en.wikipedia.org/wiki/Bounds_checking)
  or the intrinsic properties of a Red-Black tree (for example) are verified.
  Buffer overflow checks can still be kept in release mode if needed.
* as few cast as possible are used within the library (casts are the evil of safety).
  Still the library can be used with the greatest level of warnings by a C compiler without
  any aliasing warning.
* the genericity is not done directly by macro (which usually prevent type safety), but indirectly by making them
  define inline functions with the proper prototypes: this enables
  the user calls to have proper error and warning checks. 
* extensive testing: the library is tested on the main targets using Continuous Integration with a coverage of the test suite of more than 99%.
  The test suite itself is run through the multiple sanitizers defined by GCC/CLANG (Address, undefined, leak, thread).
  The test suite also includes a comparison of equivalent behaviors of M\*LIB with the C++ STL using random testing or fuzzer testing.
* static analysis: multiple static analyzer (like scan-build or GCC fanalyzer or CodeQL) are run on the generated code, and the results analyzed.

Other key designs are:

* do not rewrite the C library and just wrap around it (for example don't rewrite sort but stable sort),
* do not make users pay the cost of what they don't need.

Due to the unfortunate [weak](https://en.wikipedia.org/wiki/Strong_and_weak_typing#Pointers) nature of the C language for pointers,
[type safe](https://en.wikipedia.org/wiki/Type_safety) means that at least a warning
is generated by the compiler in case of wrong type passed as
container arguments to the functions.

M\*LIB is still quite-efficient:
there is no overhead in using this library rather than using
direct C low-level access as the compiler is able to **fully** optimize
the library usage
and the library is carefully designed.
In [fact](https://github.com/P-p-H-d/mlib/wiki/performance), M\*LIB
is one of the fastest generic C/C++ library you can find.

M\*LIB uses internally the 'malloc', 'realloc' and 'free' functions to handle
the memory pool. This behavior can be overridden at different level.
Its default policy is to abort the program if there is a memory error.
However, this behavior can also be customized globally.
M\*LIB supports also the exception error model by providing its own implementation of the try / catch mechanism.
This mechanism is compatible with [RAII programming](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization):
when an exception is thrown, the destructors of the constructed objects are called (See [m-try](#m-try) for more details).

M\*LIB may use a lot of assertions in its implementation to ensure safety: 
it is highly recommended to properly define NDEBUG for released programs. 

M\*LIB provides automatically several [serialization](https://en.wikipedia.org/wiki/Serialization) methods for each containers.
You can read or write your full and complex data structure into [JSON](https://en.wikipedia.org/wiki/JSON) format in a few lines.

M\*LIB is distributed under BSD-2 simplified license.

It is strongly advised not to read the source to know how to use the library
as the code is quite complex and uses a lot of tricks
but rather read the examples.

In this documentation, 'shall' will be used to indicate a user constraint that is
mandatory to follow under penalty of undefined behavior.
'should' will be used to indicate a recommendation to the user.

All pointers expected by the functions of the library shall expect non-null argument except if indicated.


## Components

The following headers define containers that don't require the user structure to be modified:

* [m-array.h](#m-array): header for creating dynamic array of generic type,
* [m-list.h](#m-list): header for creating singly-linked list of generic type,
* [m-deque.h](#m-deque): header for creating dynamic double-ended queue of generic type,
* [m-dict.h](#m-dict): header for creating unordered associative array (through hashmap) or unordered set of generic type,
* [m-rbtree.h](#m-rbtree): header for creating ordered set (through Red/Black binary sorted tree) of generic type,
* [m-bptree.h](#m-bptree): header for creating ordered map/set/multimap/multiset (through sorted B+TREE) of generic type,
* [m-tree.h](#m-tree): header for creating arbitrary tree of generic type,
* [m-tuple.h](#m-tuple): header for creating arbitrary tuple of generic types,
* [m-variant.h](#m-variant): header for creating arbitrary variant of generic type,
* [m-prioqueue.h](#m-prioqueue): header for creating dynamic priority queue of generic type.

The available containers of M\*LIB for thread synchronization are in the following headers:

* [m-buffer.h](#m-buffer): header for creating fixed-size queue (or stack) of generic type (multiple producer / multiple consumer),
* [m-snapshot](#m-snapshot): header for creating 'atomic buffer' (through triple buffer) for sharing synchronously big data (thread safe),
* [m-shared.h](#m-shared): header for creating shared pointer of generic type,
* [m-concurrent.h](#m-concurrent): header for transforming a container into a concurrent container (thread safe),
* m-c-mempool.h: WIP header for creating fast concurrent memory allocation.

The following containers are intrusive (You need to modify your structure to add fields needed by the container) and are defined in:

* [m-i-list.h](#m-i-list): header for creating doubly-linked intrusive list of generic type,
* [m-i-shared.h](#m-i-shared): header for creating intrusive shared pointer of generic type (Thread Safe).

Other headers offering other functionality are:

* [m-string.h](#m-string): header for creating dynamic string of characters (UTF-8 support),
* [m-bitset.h](#m-bitset): header for creating dynamic bitset (or "packed array of bool"),
* [m-algo.h](#m-algo): header for providing various generic algorithms to the previous containers,
* [m-funcobj.h](#m-funcobj): header for creating function object (used by algorithm generation),
* [m-try.h](#m-try): header for handling errors by throwing exceptions,
* [m-mempool.h](#m-mempool): header for creating specialized & fast memory allocator,
* [m-worker.h](#m-worker): header for providing an easy pool of workers on separated threads to handle work orders (used for parallel tasks),
* [m-serial-json.h](#m-serial-json): header for importing / exporting the containers in [JSON format](https://en.wikipedia.org/wiki/JSON),
* [m-serial-bin.h](#m-serial-bin): header for importing / exporting the containers in an adhoc fast binary format,
* m-genint.h: internal header for generating unique integers in a concurrent context,
* [m-core.h](#m-core): header for meta-programming with the C preprocessor (used by all other headers).

Finally headers for compatibility with non C11 compilers:

* [m-atomic.h](#m-atomic): header for ensuring compatibility between C's stdatomic.h and C++'s atomic header (provide also its own implementation if nothing is available),
* [m-thread.h](#m-thread): header for providing a very thin layer across multiple implementation of mutex/threads (C11/PTHREAD/WIN32).

Each containers define their iterators (if it is meaningful).

All containers try to expose the same common interface:
if the method name is the same, then it does the same thing
and is used in the same way.
In some rare case, the method is adapted to the container needs.

Each header can be used separately from others: dependency between headers have been kept to the minimum.

![Dependence between headers](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/depend.png)


## Build & Installation

M\*LIB is **only** composed of a set of headers, as such there is no build for the library.
The library doesn't depend on any other library than the LIBC.

To run the test suite, run:

       make check

To generate the documentation, run:

       make doc

To install the headers, run:

       make install PREFIX=/my/directory/where/to/install [DESTDIR=...]

Other targets exist. Mainly for development purpose.


## How to use

To use these data structures, you first include the desired header,
instantiate the definition of the structure and its associated methods
by using a macro \_DEF for the needed container.
Then you use the defined types and functions. Let's see a first simple example
that creates a list of unsigned int:

```C
    #include <stdio.h>
    #include "m-list.h"
    
    LIST_DEF(list_uint, unsigned int)      /* Define struct list_uint_t and its methods */
    
    int main(void) {
       list_uint_t list ;             /* list_uint_t has been define above */
       list_uint_init(list);          /* All type needs to be initialized */
       list_uint_push_back(list, 42); /* Push 42 in the list */
       list_uint_push_back(list, 17); /* Push 17 in the list */
       list_uint_it_t it;             /* Define an iterator to scan each one */
       for(list_uint_it(it, list)     /* Start iterator on first element */
           ; !list_uint_end_p(it)     /* Until the end is not reached */
           ; list_uint_next(it)) {    /* Set the iterator to the next element*/
              printf("%d\n",          /* Get a reference to the underlying */
                *list_uint_cref(it)); /* data and print it */
       }
       list_uint_clear(list);         /* Clear all the list (destroying the object list)*/
    }
```

[ Do not forget to add -std=c99 (or c11) to your compile command to request a C99 compatible build ]

This looks like a typical C program except the line with 'LIST\_DEF'
that doesn't have any semi-colon at the end. And in fact, except
this line, everything is typical C program and even macro free!
The only macro is in fact LIST\_DEF: this macro expands to the
good type for the list of the defined type and to all the necessary
functions needed to handle such type. It is heavily context dependent
and can generate different code depending on it.
You can use it as many times as needed to defined as many lists as you want.
The first argument of the macro is the name to use, e.g. the prefix that
is added to all generated functions and types.
The second argument of the macro is the type to embed within the container.
It can be any C type.
The third argument of the macro is optional and is the oplist to use.
See below for more information.


You could replace LIST\_DEF by ARRAY\_DEF to change
the kind of container (an array instead of a linked list)
without changing the code below: the generated interface
of a list or of an array is very similar.
Yet the performance remains the same as hand-written code
for both the list variant and the array variant.

This is equivalent to this C++ program using the STL:

```C
    #include <iostream>
    #include <list>
    
    typedef std::list<unsigned int> list_uint_t;
    typedef std::list<unsigned int>::iterator list_uint_it_t;
    
    int main(void) {
       list_uint_t list ;             /* list_uint_t has been define above */
       list.push_back(42);            /* Push 42 in the list */
       list.push_back(17);            /* Push 17 in the list */
       for(list_uint_it_t it = list.begin()  /* Iterator is first element*/
           ; it != list.end()         /* Until the end is not reached */
           ; ++it) {                  /* Set the iterator to the next element*/
           std::cout << *it << '\n';  /* Print the underlying data */
       }
    }
```

As you can see, this is rather equivalent with the following remarks:

* M\*LIB requires an explicit definition of the instance of the list,
* M\*LIB code is more verbose in the method name,
* M\*LIB needs explicit construction and destruction (as plain old C requests),
* M\*LIB doesn't return a value to the underlying data but a pointer to this value:
  - this was done for performance (it avoids copying all the data within the stack)
  - and for generality reasons (some structure may not allow copying data).

Note: M\*LIB defines its own container as an array of a structure of size 1.
This has the following advantages:

* you effectively reserve the data whenever you declare a variable,
* you pass automatically the variable per reference for a function call,
* you can not copy the variable by an affectation (you have to use the API instead).

M\*LIB offers also the possibility to condense further your code, so that it is more high level:
by using the M\_EACH & M\_LET macros (if you are not afraid of using syntactic macros):

```C
    #include <stdio.h>
    #include "m-list.h"
    
    LIST_DEF(list_uint, unsigned int)   /* Define struct list_uint_t and its methods */
    
    int main(void) {
       M_LET(list, LIST_OPLIST(uint)) { /* Define & init list as list_uint_t */
         list_uint_push_back(list, 42); /* Push 42 in the list */
         list_uint_push_back(list, 17); /* Push 17 in the list */
         for M_EACH(item, list, LIST_OPLIST(uint)) {
           printf("%d\n", *item);       /* Print the item */
         }
       }                                /* Clear of list will be done now */
    }
```

Here is another example with a complete type (with proper initialization & clear function) by using the [GMP](https://gmplib.org/) library:

```C
    #include <stdio.h>
    #include <gmp.h>
    #include "m-array.h"

    ARRAY_DEF(array_mpz, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)) )

    int main(void) {
       array_mpz_t array ;             /* array_mpz_t has been define above */
       array_mpz_init(array);          /* All type needs to be initialized */
       mpz_t z;                        /* Define a mpz_t type */
       mpz_init(z);                    /* Initialize the z variable */
       mpz_set_ui (z, 42);
       array_mpz_push_back(array, z);  /* Push 42 in the array */
       mpz_set_ui (z, 17);
       array_mpz_push_back(array, z); /* Push 17 in the array */
       array_it_mpz_t it;              /* Define an iterator to scan each one */
       for(array_mpz_it(it, array)     /* Start iterator on first element */
           ; !array_mpz_end_p(it)      /* Until the end is not reached */
           ; array_mpz_next(it)) {     /* Set the iterator to the next element*/
              gmp_printf("%Zd\n",      /* Get a reference to the underlying */
                *array_mpz_cref(it));  /* data and print it */
       }
       mpz_clear(z);                   /* Clear the z variable */
       array_mpz_clear(array);         /* Clear all the array */
    }
```

As the mpz\_t type needs proper initialization, copy and destroy functions
we need to tell to the container how to handle such a type.
This is done by giving it the oplist associated to the type.
An oplist is an associative array where the operators are associated to methods.

In the example, we tell to the container to use
the mpz\_init function for the INIT operator of the type (aka constructor),
the mpz\_clear function for the CLEAR operator of the type (aka destructor),
the mpz\_set function for the SET operator of the type (aka copy),
the mpz\_init\_set function for the INIT\_SET operator of the type (aka copy constructor).
See oplist chapter for more detailed information.

We can also write the same example shorter:

```C
    #include <stdio.h>
    #include <gmp.h>
    #include "m-array.h"
    
    // Register the oplist of a mpz_t.
    #define M_OPL_mpz_t() (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear))
    // Define an instance of an array of mpz_t (both type and function)
    ARRAY_DEF(array_mpz, mpz_t)
    // Register the oplist of the created instance of array of mpz_t
    #define M_OPL_array_mpz_t() ARRAY_OPLIST(array_mpz, M_OPL_mpz_t())
    
    int main(void) {
      // Let's define 'array' as an 'array_mpz_t' & initialize it.
      M_LET(array, array_mpz_t)
        // Let's define 'z1' and 'z2' to be 'mpz_t' & initialize it
        M_LET (z1, z2, mpz_t) {
         mpz_set_ui (z1, 42);
         array_mpz_push_back(array, z1);  /* Push 42 in the array */
         mpz_set_ui (z2, 17);
         array_mpz_push_back(array, z2); /* Push 17 in the array */
         // Let's iterate over all items of the container
         for M_EACH(item, array, array_mpz_t) {
              gmp_printf("%Zd\n", *item);
         }
      } // All variables are cleared with the proper method beyond this point.
      return 0;
    }
```

Or even shorter when you're comfortable enough with the library:

```C
        #include <stdio.h>
        #include <gmp.h>
        #include "m-array.h"
        
        // Register the oplist of a mpz_t. It is a classic oplist.
        #define M_OPL_mpz_t() M_OPEXTEND(M_CLASSIC_OPLIST(mpz), INIT_WITH(mpz_init_set_ui), EMPLACE_TYPE(unsigned int) )
        // Define an instance of an array of mpz_t (both type and function)
        ARRAY_DEF(array_mpz, mpz_t)
        // Register the oplist of the created instance of array of mpz_t
        #define M_OPL_array_mpz_t() ARRAY_OPLIST(array_mpz, M_OPL_mpz_t())
        
        int main(void) {
            // Let's define 'array' as an 'array_mpz_t' with mpz_t(17) and mpz_t(42)
            M_LET((array,(17),(42)), array_mpz_t) {
             // Let's iterate over all items of the container
             for M_EACH(item, array, array_mpz_t) {
                  gmp_printf("%Zd\n", *item);
             }
          } // All variables are cleared with the proper method beyond this point.
          return 0;
        }
```


There are two ways a container can known what is the oplist of a type:

* either the oplist is passed explicitly for each definition of container and for the M\_LET & M\_EACH macros,
* or the oplist is registered globally by defining a new macro starting with the prefix M\_OPL\_ and finishing with the name of type (don't forget the parenthesis and the suffix \_t if needed). The macros performing the definition of container and the M\_LET & M\_EACH will test if such macro is defined. If it is defined, it will be used. Otherwise default methods are used.

Here we can see that we register the mpz\_t type into the container with
the minimum information of its interface needed, and another one to initialize a mpz\_t from an unsigned integer.

We can also see in this example so the container ARRAY provides also
a macro to define the oplist of the array itself. This is true for
all containers and this enables to define proper recursive container like in this example
which reads from a text file a definition of sections:

```C
        #include <stdio.h>
        #include "m-array.h"
        #include "m-tuple.h"
        #include "m-dict.h"
        #include "m-string.h"
        
        TUPLE_DEF2(symbol, (offset, long), (value, long))
        #define M_OPL_symbol_t() TUPLE_OPLIST(symbol, M_BASIC_OPLIST, M_BASIC_OPLIST)
        
        ARRAY_DEF(array_symbol, symbol_t)
        #define M_OPL_array_symbol_t() ARRAY_OPLIST(array_symbol, M_OPL_symbol_t())
        
        DICT_DEF2(sections, string_t, array_symbol_t)
        #define M_OPL_sections_t() DICT_OPLIST(sections, STRING_OPLIST, M_OPL_array_symbol_t())
        
        int main(int argc, const char *argv[])
        {
          if (argc < 2) abort();
          FILE *f = fopen(argv[1], "rt");
          if (!f) abort();
          M_LET(sc, sections_t) {
            sections_in_str(sc, f);
            array_symbol_t *a = sections_get(sc, STRING_CTE(".text"));
            if (a == NULL) {
              printf("There is no .text section.");
            } else {
              printf("Section .text is :");
              array_symbol_out_str(stdout, *a);
              printf("\n");
            }
          }
          return 0;
        }
```

This example reads the data from a file
and outputs the .text section if it finds it on the terminal.

Other examples are available in the example folder.

Internal fields of the structure are subject to change even for small revision
of the library.

The final goal of the library is to be able to write code like this in pure C while keeping type safety and compile time name resolution:

```C
        let(list, list_uint_t) {
          push(list, 42);
          push(list, 17);
          for each (item, list) {
            print(item, "\n");
          }
        }
```

## Performance

M\*LIB performance is compared to the one of GNU C++ STL (v10.2) in the following graphs.
Each graph is presented first in linear scale and then in logarithmic scale to better realize the differences.
M\*LIB is on par with the STL or even faster.

All used bench codes are available in the bench directory.
The results for several different libraries are also available [in a separate page](https://github.com/P-p-H-d/mlib/wiki/performance).

#### Singly List

![Singly List performance](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-list.png)
![Singly List performance - Log Scale](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-list-log.png)

#### Array

![Array performance](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-array.png)
![Array performance - Log Scale](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-array-log.png)

#### Unordered Map

![Unordered Map performance](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-umap.png)
![Unordered Map performance - Log Scale](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-umap-log.png)

#### Ordered Set

![Ordered Set performance](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-oset.png)
![Ordered Set performance - Log Scale](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/bench-oset-log.png)



## OPLIST

### Definition

An OPLIST is a fundamental notion of M\*LIB that hasn't be seen in any other library.
In order to know how to operate on a type, M\*LIB needs additional information
as the compiler doesn't know how to handle properly any type (contrary to C++).
This is done by giving an operator list (or oplist in short) to any macro that
needs to handle the type. As such, an oplist has only meaning within a macro.
Fundamentally, this is the exposed interface of a type:
that is to say the association of the operators defined by the library to the
effective methods provided by the type, including their call interface.
This association is done only with the C preprocessor.

Therefore, an oplist is an associative array of operators to methods
in the following format:

```C
     (OPERATOR1(method1), OPERATOR2(method2), ...)
```

It starts with a parenthesis and ends with a parenthesis.
Each association is separated by a comma.
Each association is composed of a predefined operator (as defined below)
a method (in parenthesis), and an optional API interface (see below).

In the given example,
the function 'method1' is used to handle 'OPERATOR1'.
The function 'method2' is used to handle 'OPERATOR2'. etc.

In case the same operator appears multiple times in the list,
the first apparition of the operator has priority,
and its associated method will be used.
This enables overloading of operators in oplist in case you want to inherit oplists.

The associated method in the oplist is a preprocessor expression
that shall not contain a comma as first level.

An oplist has no real form from the C language point of view. It is just an abstraction
that disappears after the macro expansion step of the preprocessing.
If an oplist remains unprocessed after the C preprocessing, a compiler error will be generated.


### Usage

When you define an instance of a new container for a given type, you give the type oplist
alongside the type for building the container.
Some functions of the container may not be available in function of the provided interface of the oplist
(for optional operators).
Of if some mandatory operators are missing, a compiler error is generated.

The generated container will also provide its own oplist, which will depends on all
the oplists used to generate it. This oplist can also be used to generate new containers.

You can therefore use the oplist of the container to chain this new interface
with another container, creating container-of-container.
![oplist and definition](https://raw.githubusercontent.com/P-p-H-d/mlib/master/doc/oplist.png)


### Operators

The following operators are usually expected for an object:

* INIT(obj): initialize the object 'obj' into a valid state (constructor).
* INIT\_SET(obj, org): initialize the object 'obj' into the same state as the object 'org' (copy constructor).
* SET(obj, org): set the initialized object 'obj' into the same state as the initialized object org (copy operator).
* CLEAR(obj): destroy the initialized object 'obj', releasing any attached memory (destructor). This method shall never fail.

INIT, INIT\_SET & SET methods should only fail due to ***memory errors***.

Not all operators are needed for an oplist to handle a container.
If some operator is missing, the associated default method of the operator is used if it exists.
To use C integer or float types, the default constructors are perfectly fine.

Other documented operators are:

* NAME() --> prefix: Return the base name (prefix) used to construct the container.
* TYPE() --> type: Return the base type associated to this oplist.
* SUBTYPE() --> type: Return the type of the element stored in the container (used to iterate over the container).
* OPLIST() --> oplist: Return the oplist of the type of the elements stored in the container.
* KEY\_TYPE() --> key\_t: Return the key type for associative containers.
* VALUE\_TYPE() --> value\_t: Return the value type for associative containers.
* KEY\_OPLIST() --> oplist: Return the oplist of the key type for associative containers.
* VALUE\_OPLIST() --> oplist: Return the oplist of the value type for associative containers.
* NEW (type) -> type pointer: allocate a new object (with suitable alignment and size) and return a pointer to it. The returned object is **not initialized** (a constructor operator shall be called afterward). The default method is M\_MEMORY\_ALLOC (that allocates from the heap). It returns NULL in case of failure.
* DEL (&obj): free the allocated uninitialized object 'obj'. The object is not cleared before being free (A destructor operator shall be called before). The object shall have been allocated by the associated NEW method. The default method is M\_MEMORY\_DEL (that frees to the heap).
* REALLOC(type, type pointer, number) --> type pointer: realloc the given array referenced by type pointer (either a NULL pointer or a pointer returned by the associated REALLOC method itself) to an array of the number of objects of this type and return a pointer to this new array. Previously objects pointed by the pointer are kept up to the minimum of the new size and old one. New objects are not initialized (a constructor operator shall be called afterward). Freed objects are not cleared (A destructor operator shall be called before). The default is M\_MEMORY\_REALLOC (that allocates from the heap). It returns NULL in case of failure in which case the original array is not modified.
* FREE (&obj) : free the allocated uninitialized array object 'obj'. The objects are not cleared before being free (CLEAR operator has to be called before).  The object shall have been allocated by the associated REALLOC method. The default is M\_MEMORY\_FREE (that frees to the heap).
* INC\_ALLOC(size\_t s) -> size\_t: Define the growing policy of an array (or equivalent structure). It returns a new allocation size based on the old allocation size ('s'). Default policy is to get the maximum between '2*s' and 16. NOTE: It doesn't check for overflow: if the returned value is lower than the old one, the user shall raise an overflow error.
* INIT\_MOVE(objd, objc): Initialize 'objd' to the same state than 'objc' by stealing as much resources as possible from 'objc', and then clear 'objc' (constructor of objd + destructor of objc). It is semantically equivalent to calling INIT\_SET(objd,objc) then CLEAR(objc) but is usually way faster.  Contrary to the C++ choice of using "conservative move" semantic (you still need to call the destructor of a moved object in C++) M\*LIB implements a "destructive move" semantic (this enables better optimization). By default, all objects are assumed to be **trivially movable** (i.e. using memcpy to move an object is safe). Most C objects (even complex structure) are trivially movable and it is a very nice property to have (enabling better optimization). A notable exception are intrusive objects. If an object is not trivially movable, it shall provide an INIT\_MOVE method or disable the INIT\_MOVE method entirely (NOTE: Some containers may assume that the objects are trivially movable). An INIT\_MOVE operator shall not fail. Moved objects shall use the same memory allocator.
* MOVE(objd, objc): Set 'objd' to the same state than 'objc' by stealing as resources as possible from 'objc' and then clear 'objc' (destructor of 'objc'). It is equivalent to calling SET(objd,objc) then CLEAR(objc) or CLEAR(objd) and then INIT\_MOVE(objd, objc). See INIT\_MOVE for details and constraints. TBC if this operator is really needed as calling CLEAR then INIT\_MOVE is what do all known implementation, and is efficient.
* INIT\_WITH(obj, ...): Initialize the object 'obj' with the given variable set of arguments (constructor). The arguments can be of different types. It is up to the method of the object to decide how to initialize the object based on this initialization array. This operator is used by the M\_LET macro to initialize objects with their given values and this operator defines what the M\_LET macro supports. In C11, you can use API\_1(M\_INIT\_WITH\_THROUGH\_EMPLACE\_TYPE) as method to automatically use the different emplace functions defined in EMPLACE\_TYPE through a _Generic switch case. The EMPLACE\_TYPE shall use the LIST format. See emplace chapter.
* SWAP(objd, objc): Swap the states of the object 'objc' and the object 'objd'.  The objects shall use the same allocator.
* RESET(obj): Reset the object to its initialized state (Emptying the object if it is a container object).
* EMPTY\_P(obj) --> bool: Test if the container object is empty (true) or not.
* GET\_SIZE (container) --> size\_t: Return the number of elements in the container object.
* HASH (obj) --> size\_t: return a hash of the object (not a secure hash but one that is usable for a hash table). Default is performing a hash of the memory representation of the object. This default implementation is invalid if the object holds pointer to other objects.
* EQUAL(obj1, obj2) --> bool: Compare the two objects for equality. Return true if both objects are equal, false otherwise. Default is using the C comparison operator. 'obj1' may be an OOR object (Out of Representation) for the Open Addressing dictionary (see OOR\_* operators): in such cases, it shall return false.
* CMP(obj1, obj2) --> int: Provide a complete order the objects. return a negative integer if obj1 < obj2, 0 if obj1 = obj2, a positive integer otherwise. Default is C comparison operator. NOTE: The equivalence between EQUAL(a,b) and CMP(a,b)==0 is not required, but is usually welcome.
* ADD(obj1, obj2, obj3) : Set obj1 to the sum of obj2 and obj3. Default is '+' C operator.
* SUB(obj1, obj2, obj3) : Set obj1 to the difference of obj2 and obj3. Default is '-' C operator.
* MUL(obj1, obj2, obj3) : Set obj1 to the product of obj2 and obj3. Default is '*' C operator.
* DIV(obj1, obj2, obj3) : Set obj1 to the division of obj2 and obj3. Default is '/' C operator.
* GET\_KEY (container, key) --> &value: Return a pointer to the value object within the container associated to the key 'key' or return NULL if no object is associated to this key. The pointer to the value object remains valid until any modification of the container. 
* SET\_KEY (container, key, value): Associate the key object 'key' to the value object 'value' in the given container.
* SAFE\_GET\_KEY (container, key) --> &value: return a pointer to the value object within the container associated to the key 'key' if it exists, or create a new entry in the container and associate it to the key 'key' with the default initialization before returning its pointer. The pointer to the object remains valid until any modification of the container. The returned pointer is therefore never NULL.
* ERASE\_KEY (container, key) --> bool: Erase the object associated to the key 'key' within the container. Return true if successful, false if the key is not found (nothing is done).
* PUSH(obj, subobj) : Push 'subobj' (of type SUBTYPE() ) into the container 'obj'. How and where it is pushed is container dependent.
* POP(&subobj, obj) : Pop an object from the container 'obj' and set it in the initialized object '*subobj' (of type SUBTYPE()) if subobj is not NULL. Which object is popped is container dependent. The container shall have at least one object.
* PUSH\_MOVE(obj, &subobj) : Push and move the object '*subobj' (of type SUBTYPE()) into the container 'obj' (subobj destructor). How it is pushed is container dependent. '*subobj' is cleared afterward and shall not be used anymore. See INIT\_MOVE for more details and constraints.
* POP\_MOVE(&subobj, obj) : Pop an object from the container 'obj' and **init & move** it in the uninitialized object '*subobj' (*subobj constructor). Which object is popped is container dependent. '*subobj' shall be uninitialized. The container shall have at least one object. See INIT\_MOVE for more details and constraints.
* IT\_TYPE() --> type: Return the type of the iterator object of this container.
* IT\_FIRST(it\_obj, obj): Set the iterator it\_obj to the first sub-element of the container 'obj'. What is the first element is container dependent (it may be front or back, or something else). However, iterating from FIRST to LAST (included) or END (excluded) through IT\_NEXT ensures going through all elements of the container. If there is no sub-element in the container, it references an end of the container.
* IT\_LAST(it\_obj, obj): Set the iterator it\_obj to the last sub-element of the container 'obj'.  What is the last element is container dependent (it may be front or back, or something else). However, iterating from LAST to FIRST (included) or END (excluded) through IT\_PREVIOUS ensures going through all elements of the container. If there is no sub-element in the container, it references an end of the container.
* IT\_END(it\_obj, obj): Set the iterator it\_obj to an end of the container 'obj'. Once an iterator has reached an end, it can't use PREVIOUS or NEXT operators. If an iterator has reached an END, it means that there is no object referenced by the iterator (kind of NULL pointer). There can be multiple representation of the end of a container, but all of then share the same properties.
* IT\_SET(it\_obj, it\_obj2): Set the iterator it\_obj to reference the same sub-element as it\_obj2.
* IT\_END\_P(it\_obj)--> bool: Return true if the iterator it\_obj references an end of the container, false otherwise.
* IT\_LAST\_P(it\_obj)--> bool: Return true if the iterator it\_obj references the last element of the container (just before reaching an end) or has reached an end of the container, false otherwise.
* IT\_EQUAL\_P(it\_obj, it\_obj2) --> bool: Return true if both iterators reference the same element, false otherwise.
* IT\_NEXT(it\_obj): Move the iterator to the next sub-element or an end of the container if there is no more sub-element. The direction of IT\_NEXT is container dependent. it\_obj shall not be an end of the container.
* IT\_PREVIOUS(it\_obj): Move the iterator to the previous sub-element or an end of the container if there is no more sub-element. The direction of PREVIOUS is container dependent, but it is the reverse of the IT\_NEXT operator. it\_obj shall not be an end of the container.
* IT\_CREF(it\_obj) --> &subobj: Return a constant pointer to the object referenced by the iterator (of type const SUBTYPE()). This pointer remains valid until any modifying operation on the container, or until another reference is taken from this container through an iterator (some containers may reduce theses constraints, for example a list). The iterator shall not be an end of the container.
* IT\_REF(it\_obj) --> &subobj: Same as IT\_CREF, but return a modifiable pointer to the object referenced by the iterator.
* IT\_INSERT(obj, it\_obj, subobj): Insert 'subobj' after 'it\_obj' in the container 'obj' and update it\_obj to point to the inserted object (as per IT\_NEXT semantics). All other iterators of the same container become invalidated. If 'it\_obj' is an end of the container, it inserts the object as the first one.
* IT\_REMOVE(obj, it\_obj): Remove it\_obj from the container 'obj' (clearing the associated object) and update it\_obj to point to the next object (as per IT\_NEXT semantics). As it modifies the container, all other iterators of the same container become invalidated. it\_obj shall not be an end of the container.
* SPLICE\_BACK(objd, objs, it\_obj): Move the object of the container 'objs' referenced by the iterator 'it\_obj' to the container 'objd'. Where it is moved is container dependent (it is recommended however to be like the PUSH method). Afterward 'it\_obj' references the next item in 'containerSrc' (as per IT\_NEXT semantics). 'it\_obj' shall not be an end of the container. Both objects shall use the same allocator.
* SPLICE\_AT(objd, id\_objd, objs, it\_objs): Move the object referenced by the iterator 'it\_objs' from the container 'objs' just after the object referenced by the iterator 'it\_objd' in the container 'objd'. If 'it\_objd' references an end of the container, it is inserted as the first item of the container (See operator 'IT\_INSERT'). Afterward 'it\_objs' references the next item in the container 'objs', and 'it\_objd' references the moved item in the container 'objd'. 'it\_objs' shall not be an end of the container.  Both objects shall use the same allocator.
* OUT\_STR(FILE* f, obj): Output 'obj' as a custom formatted string into the FILE stream 'f'. Format is container dependent, but is human readable.
* IN\_STR(obj, FILE* f) --> bool: Set 'obj' to the value associated to the formatted string representation of the object in the FILE stream 'f'. Return true in case of success (in that case the stream 'f' has been advanced to the end of the parsing of the object), false otherwise (in that case, the stream 'f' is in an undetermined position but is likely where the parsing fails). It ensures that an object which is output in a FILE through OUT\_STR, and an object which is read from this FILE through IN\_STR are considered as equal.
* GET\_STR(string\_t str, obj, bool append): Set 'str' to a formatted string representation of the object 'obj'. Append to the string if 'append' is true, set it otherwise. This operator requires the module m-string.
* PARSE\_STR(obj, const char *str, const char **endp) --> bool: Set 'obj' to the value associated to the formatted string representation of the object in the char stream 'str'. Return true in case of success (in that case if endp is not NULL, it points to the end of the parsing of the object), false otherwise (in that case, if endp is not NULL, it points to an undetermined position but likely to be where the parsing fails). It ensures that an object which is written in a string through GET\_STR, and an object which is read from this string through PARSE\_STR are considered as equal.
* OUT\_SERIAL(m\_serial\_write\_t *serial, obj) --> m\_serial\_return\_code\_t : Output 'obj' into the configurable serialization stream 'serial' (See #[m-serial-json.h](#m-serial-json) for details and example) as per the serial object semantics. Return M\_SERIAL\_OK\_DONE in case of success, or M\_SERIAL\_FAIL otherwise .
* IN\_SERIAL(obj, m\_serial\_read\_t *serial) --> m\_serial\_return\_code\_t: Set 'obj' to its representation from the configurable serialization stream 'serial' (See #[m-serial-json.h](#m-serial-json) for details and example) as per the serial object semantics. M\_SERIAL\_OK\_DONE in case of success (in that case the stream 'serial' has been advanced up to the complete parsing of the object), or M\_SERIAL\_FAIL otherwise (in that case, the stream 'serial' is in an undetermined position but usually around the next characters after the first failure).
* OOR\_SET(obj, int\_value): Some containers want to store some information within the uninitialized objects (for example Open Addressing Hash Table). This method stores the integer value 'int\_value' into an uninitialized object 'obj'. It shall be able to differentiate between uninitialized object and initialized object (How is type dependent). The way to store this information is fully object dependent. In general, you use out-of-range value for detecting such values. The object remains uninitialized but sets to of out-of-range value (OOR). int\_value can be 0 or 1.
* OOR\_EQUAL(obj, int\_value): This method compares the object 'obj' (initialized or uninitialized) to the out-of-range value (OOR) representation associated to 'int\_value' and returns true if both objects are equal, false otherwise. See OOR\_SET.
* REVERSE(obj) : Reverse the order of the items in the container 'obj'.
* SEPARATOR() --> character: Return the character used to separate items for I/O methods (default is ',') (for internal use only).
* EXT\_ALGO(name, container oplist, object oplist): Define additional algorithms functions specialized for the containers (for internal use only).
* PROPERTIES() --> ( properties): Return internal properties of a container in a recursive oplist format. Use M\_GET\_PROPERTY to get the property.
* EMPLACE\_TYPE( ... ) : Specify the types usable for "emplacing" the object. See chapter 'emplace'.

The operator names listed above shall not be defined as macro.
More operators are expected.

NOTE: An iterator doesn't have a constructor nor destructor methods.
It should not allocate any memory. A reference to an object through
an iterator is only valid until another reference is taken from the same
container (potentially through another iterator),
the iterator is moved. If the container is modified, all iterators
of this container become invalid and shall not be used anymore
except if the modifying operator provided itself an updated iterator.
Some containers may lessen these constraints.

### Properties

Properties can be stored in a sub-oplist format in the PROPERTIES operator.

The following properties are defined:
* LET\_AS\_INIT\_WITH(1): Defined if the macro M\_LET shall always initialize the object with INIT\_WITH regardless of the given input. The value of the property is 1 (enabled) or 0 (disabled/default).
* NOCLEAR(1): Defined if the object CLEAR operator can be omitted (like for basic types or POD data). The value of the property is 1 (enabled) or 0 (disabled/default).

More properties are expected.


### Example:

Let's take the interface of the MPZ library:
```C
     void mpz_init(mpz_t z); // Constructor of the object z
     void mpz_init_set(mpz_t z, const mpz_t s); // Copy Constructor of the object z
     void mpz_set(mpz_t z, const mpz_t s); // Copy operator of the object z
     void mpz_clear(mpz_t z); // Destructor of the object z
```
A basic oplist will be:

```C
        (INIT(mpz_init),SET(mpz_set),INIT_SET(mpz_init_set),CLEAR(mpz_clear),TYPE(mpz_t))
```

Much more complete oplist can be built for this type however, enabling much more powerful generation:
See in the [example](https://github.com/P-p-H-d/mlib/blob/master/example/ex11-multi02.c)


### Global namespace

Oplist can be registered globally by defining, for the type 'type', a macro named
M\_OPL\_ ## type () that expands to the oplist of the type.
Only type without any space in their name can be registered. A typedef of the type
can be used instead, but this typedef shall be used everywhere.

Example:

```C
        #define M_OPL_mpz_t() (INIT(mpz_init),SET(mpz_set),INIT_SET(mpz_init_set),CLEAR(mpz_clear),TYPE(mpz_t))
```

This can simplify a lot OPLIST usage and it is recommended.

Then each times a macro expects an oplist, you can give instead its type.
This make the code much easier to read and understand.

There is one exception however: the macros that are used to build oplist
(like ARRAY\_OPLIST) don't perform this simplification and the oplist of
the basic type shall be given instead
(This is due to limitation in the C preprocessing).


### API Interface Adaptation

Within an OPLIST, you can also specify the needed low-level transformation to perform for calling your method.
This is called API Interface Adaptation: it enables to transform the API requirements of the selected operator
(which is very basic in general) to the API provided by the given method.
Assuming that the method to call is called 'method' and the first argument of the operator is 'output',
which interface is OPERATOR(output, ...)
then the following predefined adaptation are available:

* API\_0: method(output, ...)  /\* No adaptation \*/
* API\_1: method(oplist, output, ...) /\* No adaptation but gives the oplist to the method \*/
* API\_2: method(&output, ...) /\* Pass by address the first argument (like with M\_IPTR) \*/
* API\_3: method(oplist, &output, ...) /\* Pass by address the first argument (like with M\_IPTR) and give the oplist of the type \*/
* API\_4 : output = method(...) /\* Pass by return value the first argument \*/
* API\_5:  output = method(oplist, ...) /\* Pass by return value the first argument and give the oplist of the type \*/
* API\_6 : method(&output, &...) /\* Pass by address the two first arguments \*/
* API\_7:  method(oplist, &output, &...) /\* Pass by address the two first argument and give the oplist of the type \*/

The API Adaptation to use shall be embedded in the OPLIST definition.
For example:

```C
        (INIT(API_0(mpz_init)), SET(API_0(mpz_set)), INIT_SET(API_0(mpz_init_set)), CLEAR(API_0(mpz_clear)))
```

The default adaptation is API\_0 (i.e. no adaptation between operator interface and method interface).
If an adaptation gives an oplist to the method, the method shall be implemented as macro.


Let's take the interface of a pseudo library:
```C
     typedef struct { ... } obj_t;
     obj_t *obj_init(void);                // Constructor of the object z
     obj_t *obj_init_str(const char *str); // Constructor of the object z
     obj_t *obj_clone(const obj_t *s);     // Copy Constructor of the object z
     void obj_clear(obj_t *z);             // Destructor of the object z
```
The library returns a pointer to the object, so we need API_4 for these methods.
There is no method for the SET operator available. However, we can use the macro M_SET_THROUGH_INIT_SET
to emulate a SET semantics by using a combination of CLEAR+INIT_SET. This enables to support
the type for array containers in particular. Or we can avoid this definition if we don't need it.
A basic oplist will be:

```C
     (INIT(API_4(obj_init)),SET(API_1(M_SET_THROUGH_INIT_SET)),INIT_SET(API_4(obj_clone)),CLEAR(obj_clear),TYPE(obj_t *))
```


### Generic API Interface Adaptation

You can also describe the exact transformation to perform for calling the method:
this is called Generic API Interface Adaptation (or GAIA).
With this, you can add constant values to parameter of the method,
reorder the parameters as you wish, pass then by pointers, or even change the return value.

The API adaptation is also described in the operator mapping with the method name by using the API keyword.
Usage in oplist:

```C
    , OPERATOR( API( method, returncode, args...) ) ,
```
Within the API keyword,
* method is the pure method name (as like any other oplist)
* returncode is the transformation to perform of the return code.
* args are the list of the arguments of the function. It can be:

'returncode' can be
* NONE (no transformation),
* VOID (cast to void),
* NEG (inverse the result),
* EQ(val)/NEQ(val)/LT(val)/GT(val)/LE(val)/GE(val) (compare the return code to the given value)
* ARG[1-9] (set the associated argument number of the operator to the return code)

An argument can be:
* a constant,
* a variable name (probably global),
* ID( constant or variable), if the constant or variable is not a valid token,
* ARG[1-9] (the associated argument number of the operator),
* ARGPTR[1-9] (the pointer of the associated argument number of the operator),
* OPLIST (the oplist)

Therefore it supports at most 9 arguments.

Example:

     , EQUAL( API(mpz_cmp, EQ(0), ARG1, ARG2) ) , 

This will transform a return value of 0 by the mpz\_cmp method into a boolean for the EQUAL operator.

Another Example:

     , OUT_STR( API(mpz_out_str, VOID, ARG1, 10, ARG2) ) , 

This will serialize the mpz\_t value in base 10 using the mpz\_out\_str method, and discarding the return value.


### Disable an operator

An operator OP can be defined, omitted or disabled:

* ( OP(f) ): the function f is the method of the operator OP
* ( OP(API\_N(f)) ): the function f is the method of the operator OP with the API transformation number N,
* ( ): the operator OP is omitted, and the default global operation for OP is used (if it exists).
* ( OP(0) ): the operator OP is disabled, and it can never be used.

This can be useful to disable an operator in an inherited oplist.


### Which OPLIST to use?

My type is:

* a C Boolean: M\_BOOL\_OPLIST (M\_BASIC\_OPLIST also works partially),
* a C integer or a C float: M\_BASIC\_OPLIST (it can also be omitted),
* a C enumerate: M\_ENUM\_OPLIST,
* a pointer to something (the container do nothing on the pointed object): M\_PTR\_OPLIST,
* a plain structure that can be init/copy/compare with memset/memcpy/memcmp: M\_POD\_OPLIST,
* a plain structure that is passed by reference using [1] and can be init,copy,compare with memset,memcpy,memcmp: M\_A1\_OPLIST,
* a type that offers name\_init, name\_init\_set, name\_set, name\_clear methods: M\_CLASSIC\_OPLIST,
* a const string (const char *) that is neither freed nor moved: M\_CSTR\_OPLIST,
* a M\*LIB string\_t: STRING\_OPLIST,
* a M\*LIB container: the OPLIST of the used container,
* other things: you need to provide a custom OPLIST to your type.

Note: The precise exported methods of the OPLIST depend of the version
of the used C language. Typically, in C11 mode, the M\_BASIC\_OPLIST
exports all needed methods to handle generic input/output of int/floats
(using \_Generic keyword) whereas it is not possible in C99 mode.

This explains why JSON import/export is only available in C11 mode
(See below chapter).

Basic usage of oplist is available in the [example](https://github.com/P-p-H-d/mlib/blob/master/example/ex-array00.c)


### Oplist inheritance

Oplist can inherit from another one.
This is useful when you want to customize some specific operators while keeping other ones by default.
For example, internally M\_BOOL\_OPLIST inherits from M\_BASIC\_OPLIST.

A typical example is if you want to provide the OOR\_SET and OOR\_EQUAL operators to a type
so that it can be used in an OA dict.
To do it, you use the M\_OPEXTEND macro. It takes as first argument the oplist you want to inherit with,
and then you provide the additional associations between operators to methods you want to add
or override in the inherited oplist. For example:

```C
    int my_int_oor_set(char c) { return INT_MIN + c; }
    bool my_int_oor_equal(int i1, int i2) { return i1 == i2; }

    #define MY_INT_OPLIST                                                 \
        M_OPEXTEND(M_BASIC_OPLIST, OOR_SET(API_4(my_int_oor_set)), OOR_EQUAL(my_int_oor_equal))
```

You can even inherit from another oplist to disable some operators in your new oplist.
For example:

```C
    #define MY_INT_OPLIST                                                 \
        M_OPEXTEND(M_BASIC_OPLIST, HASH(0), CMP(0), EQUAL(0))
```

MY\_INT\_OPLIST is a new oplist that handles integers but has disabled the operators HASH, CMP and EQUAL.
The main interest is to disable the generation of optional methods of a container (since they are only
expanded if the oplist provides some methods).

Usage of inheritance and oplist is available in the [int example](https://github.com/P-p-H-d/mlib/blob/master/example/ex-dict05.c)
and the [cstr example](https://github.com/P-p-H-d/mlib/blob/master/example/ex-dict06.c)


### Advanced example

Let's take a look at the interface of the FILE interface:
```C
     FILE *fopen(const char *filename, const char *mode);
     fclose(FILE *f);
```
There is no INIT operator (an argument is mandatory), no INIT\_SET operator.
It is only possible to open a file from a filename.
'FILE *' contains some space, so an alias is needed.
There is an optional mode argument, which is a constant string, and isn't a valid preprocessing token.
An oplist can therefore be:

```C
     typedef FILE *m_file_t;
     #define M_OPL_m_file_t() (INIT_WITH(API(fopen, ARG1, ARG2, ID("wt"))),SET(0),INIT_SET(0),CLEAR(fclose),TYPE(m_file_t))
```

Since there is no INIT\_SET operator available, pretty much no container can work.
However you'll be able to use a writing text FILE using a M\_LET :

```C
    M_LET( (f, ("tmp.txt")), m_file_t) {
      fprintf(f, "This is a tmp file.");
    }
```

This is pretty useless, except if you enable exceptions...
In which case, this enables you to close the file even if an exception is thrown.


## Memory Allocation

### Customization

Memory Allocation functions can be globally set by overriding the following macros before using the definition \_DEF macros:

* M\_MEMORY\_ALLOC (type) --> ptr: return a pointer to a new object of type 'type'.
* M\_MEMORY\_DEL (ptr): free the single object pointed by 'ptr'.
* M\_MEMORY\_REALLOC (type, ptr, number) --> ptr: return a pointer to an array of 'number' objects of type 'type', reusing the old array pointed by 'ptr'. 'ptr' can be NULL, in which case the array will be created.
* M\_MEMORY\_FREE (ptr): free the array of objects pointed by 'ptr'.

ALLOC & DEL operators are supposed to allocate fixed size single element object (no array).
Theses objects are not expected to grow.
REALLOC & FREE operators deal with allocated memory for growing objects.
Do not mix pointers between both: a pointer allocated by ALLOC (resp. REALLOC) is supposed
to be only freed by DEL (resp. FREE). There are separated 'free' operators to enable
specialization in the allocator (a good allocator can take this hint into account).

M\_MEMORY\_ALLOC and  M\_MEMORY\_REALLOC are supposed to return NULL in case of memory allocation failure.
The defaults are 'malloc', 'free', 'realloc' and 'free'.

You can also override the methods NEW, DEL, REALLOC & DEL in the oplist given to a container
so that only the container will use these memory allocation functions instead of the global ones.


### Out-of-memory error

When a memory exhaustion is reached, the global macro "M\_MEMORY\_FULL" is called
and the function returns immediately after.
The object remains in a valid (if previously valid) and unchanged state in this case.

By default, the macro prints an error message and aborts the program:
handling non-trivial memory errors can be hard,
testing them is even harder but still mandatory to avoid security holes.
So the default behavior is rather conservative.

Indeed, a good design should handle a process entire failure (using for examples multiple
processes for doing the job) so that even if a process stops, it should be recovered.
See [here](http://joeduffyblog.com/2016/02/07/the-error-model/) for more
information about why abandonment is good software practice.

It can however be overloaded to handle other policy for error handling like:

* throwing an error (See header [m-try](#m-try) for defining exceptions with M\*LIB),
* set a global error and handle it when the function returns [planned, not possible yet],
* other policy.

This function takes the size in bytes of the memory that has been tried to be allocated.

If needed, this macro shall be defined ***prior*** to instantiate the structure.



## Emplace construction

For M\*LIB, 'emplace' means pushing a new object in the container,
while not giving it a copy of the new object, but the parameters needed
to construct this object.
This is a shortcut to the pattern of creating the object with the arguments,
pushing it in the container, and deleting the created object
(even if using PUSH\_MOVE could simplify the design).

The containers defining an emplace like method generate the emplace functions
based on the provided EMPLACE\_TYPE of the oplist. If EMPLACE\_TYPE doesn't exist or is disabled, no emplace function is generated. Otherwise EMPLACE\_TYPE identifies
which types can be used to construct the object and which methods to use to construct then:

* EMPLACE\_TYPE( typeA ) means that the object can be constructed from 'typeA' using the method of the INIT_WITH operator. An emplace function without suffix will be generated.
* EMPLACE\_TYPE( (typeA, typeB, ...) ) means that the object can be constructed from the lists of types 'typeA, typeB, ...' using the method of the INIT_WITH operator. An emplace function without suffix will be generated.
* EMPLACE\_TYPE( LIST( (suffix, function, typeA, typeB, ...),  (suffix, function, typeA, typeB, ...), ...) means that the object can be constructed from all the provided lists of types 'typeA, typeB, ...' using the provided method 'function'. The 'function' is the method to call to construct the object from the list of types. It supports API transformation if needed. As many emplace function will be generated as there are constructor function. Each generated function will be generated by suffixing it with the provided 'suffix' (if suffix is empty, no suffix will be added).

For example, for an ARRAY definition named vec, if there is such a definition of EMPLACE\_TYPE(const char *), it will generate a function vec\_emplace\_back(const char *). This function will take a const char * parameter, construct the object from it (for example a string\_t) then push the result back on the array.

Another example, for an ARRAY definition named vec, if there is such a definition of EMPLACE\_TYPE( LIST( (_ui, mpz\_init\_set\_ui, unsigned int), (_si, mpz\_init\_set\_si, int) ) ), it will generate two functions vec\_emplace\_back\_ui(unsigned int) and vec\_emplace\_back\_si(int). Theses functions will take the (unsigned) int parameter, construct the object from it then push the result back on the array.

If the container is an associative array, the name will be constructed as follow:
        name\_emplace[\_key\_keysuffix][\_val\_valsuffix]
where keysuffix (resp. valsuffix) is the emplace suffix of the key (resp. value) oplist.

If you take once again the example of the FILE, a more complete oplist can be:

```C
     typedef FILE *m_file_t;
     #define M_OPL_m_file_t() (INIT_WITH(API_1(M_INIT_WITH_THROUGH_EMPLACE_TYPE)),SET(0),INIT_SET(0),CLEAR(fclose),TYPE(m_file_t), \
             EMPLACE_TYPE(LIST((_str, API(fopen, ARG1, ARG2, ID("wb")), char *))))
```

The INIT\_WITH operator will use the provided init methods in the EMPLACE\_TYPE.
EMPLACE\_TYPE defines a \_str suffix method with a GAIA for fopen,
and accepts a char * as argument.
The GAIA specifies that the output (ARG1) is set as return value,
ARG2 is given as the first argument, and a third constant argument is used.


## ERRORS & COMPILERS

M\*LIB implements internally some controls to reduce the list of errors/warnings generated by a compiler
when it detects some violation in the use of oplist by use of static assertion.
It can also transform some type warnings into proper errors.
In C99 mode, it will produce illegal code with the name of the error as attribute.
In C11 mode, it will use static assert and produce a detailed error message.

The list of errors it can generate:

* M\_LIB\_NOT\_AN\_OPLIST: something has been given (directly or indirectly) and it doesn't reduce as a proper oplist. You need to give an oplist for this definition.
* M\_LIB\_ERROR(ARGUMENT\_OF\_\*\_OPLIST\_IS\_NOT\_AN\_OPLIST, name, oplist): sub error of the previous error one, identifying the root cause. The error is in the oplist construction of the given macro. You need to give an oplist for this oplist construction.
* M\_LIB\_MISSING\_METHOD: a required operator doesn't define any method in the given oplist. You need to complete the oplist with the missing method.
* M\_LIB\_TYPE\_MISMATCH: the given oplist and the type do not match each other. You need to give the right oplist for this type.
* M\_LIB\_NOT\_A\_BASIC\_TYPE: The oplist M\_BASIC\_OPLIST (directly or indirectly) has been used with the given type, but the given type is not a basic C type (int /float). You need to give the right oplist for this type.

You should focus mainly on the first reported error/warning
even if the link between what the compiler report and what the error is
is not immediate. The error is always in one of the **oplist definition**.

Examples of typical errors:

* lack of inclusion of an header,
* overriding locally operator names by macros (like NEW, DEL, INIT, OPLIST, ...),
* lack of ( ) or double level of ( ) around the oplist,
* an unknown variable (example using BASIC\_OPLIST instead of M\_BASIC\_OPLIST),
* the name given to the oplist is not the same as the one used to define the methods,
* use of a type instead of an oplist in the OPLIST definition,
* a missing sub oplist in the OPLIST definition.

A good way to avoid theses errors is to register the oplist globally as soon
as you define the container.

In case of difficulties, debugging can be done by generating the preprocessed file
(by usually calling the compiler with the '-E' option instead of '-c')
and check what's wrong in the preprocessed file:

```shell
          cc -std=c99 -E test-file.c |grep -v '^#' > test-file.i
          perl -i -e 's/;/;\n/g' test-file.i
          cc -std=c99 -c -Wall test-file.i
```

If there is a warning reported by the compiler in the generated code,
then there is definitely an **error** you should fix (except if it reports
shadowed variables), in particular cast evolving pointers.

You should also turn off the macro expansion of the errors reported by
your compiler. There are often completely useless and misleading:

* For GCC, uses -ftrack-macro-expansion=0
* For CLANG, uses -fmacro-backtrace-limit=1

Due to the unfortunate [weak](https://en.wikipedia.org/wiki/Strong_and_weak_typing#Pointers) nature of the C language for pointers,
you should turn incompatible pointer type warning into an error in your compiler.
For GCC / CLANG, uses -Werror=incompatible-pointer-types

For MS Visual C++ compiler , you need the following options:

      /Zc:__cplusplus /Zc:preprocessor /D__STDC_WANT_LIB_EXT1__ /EHsc


## External Reference

Many other implementation of generic container libraries in C exist.
For example, a non exhaustive list is:

* [BKTHOMPS/CONTAINERS](https://github.com/bkthomps/Containers)
* [BSD tree.h](http://fxr.watson.org/fxr/source/sys/tree.h)
* [CC](https://github.com/JacksonAllan/CC)
* [CDSA](https://github.com/MichaelJWelsh/cdsa)
* [CELLO](http://libcello.org/)
* [C-Macro-Collections](https://github.com/LeoVen/C-Macro-Collections)
* [COLLECTIONS-C](https://github.com/srdja/Collections-C)
* [CONCURRENCY KIT](https://github.com/concurrencykit/ck)
* CTL [by glouw](https://github.com/glouw/ctl) or [by rurban](https://github.com/rurban/ctl)
* [GDB internal library](https://sourceware.org/git/gitweb.cgi?p=binutils-gdb.git;a=blob;f=gdb/common/vec.h;h=41e41b5b22c9f5ec14711aac35ce4ae6bceab1e7;hb=HEAD)
* [GLIB](https://wiki.gnome.org/Projects/GLib)
* [KLIB](https://github.com/attractivechaos/klib)
* [LIBCHASTE](https://github.com/mgrosvenor/libchaste)
* [LIBCOLLECTION](https://bitbucket.org/manvscode/libcollections)
* [LIBDICT](https://github.com/fmela/libdict)
* [LIBDYNAMIC](https://github.com/fredrikwidlund/libdynamic)
* [LIBLFDS](https://www.liblfds.org/)
* [LIBSRT:  Safe Real-Time library for the C programming language](https://github.com/faragon/libsrt)
* [NEDTRIES](https://github.com/ned14/nedtries)
* [POTTERY](https://github.com/ludocode/pottery)
* [QLIBC](http://wolkykim.github.io/qlibc/)
* [SC](https://github.com/tezc/sc)
* [SGLIB](http://sglib.sourceforge.net/)
* [Smart pointer for GNUC](https://github.com/Snaipe/libcsptr)
* [STB stretchy_buffer](https://github.com/nothings/stb)
* [STC - Smart Template Container for C](https://github.com/tylov/STC)
* [TommyDS](https://github.com/amadvance/tommyds)
* [UTHASH](http://troydhanson.github.io/uthash/index.html)

Each can be classified into one of the following concept:

* Each object is handled through a pointer to void (with potential registered callbacks to handle the contained objects for the specialized methods). From a user point of view, this makes the code harder to use (as you don't have any help from the compiler) and type unsafe with a lot of cast (so no formal proof of the code is possible). This also generally generate slower code (even if using link time optimization, this penalty can be reduced). Properly used, it can yet be the most space efficient for the code, but can consume a lot more for the data due to the obligation of using pointers. This is however the easiest to design & code.
* Macros are used to access structures in a generic way (using known fields of a structure - typically size, number, etc.). From a user point of view, this can create subtitle bug in the use of the library (as everything is done through macro expansion in the user defined code) or hard to understand warnings. This can generates fully efficient code. From a library developer point of view, this can be quite limiting in what you can offer.
* Macros detect the type of the argument passed as parameter using _Generics, and calls the associated methods of the switch. The difficulty is how to add pure user types in this generic switch.
* A known structure is put in an intrusive way in the type of all the objects you wan to handle. From a user point of view, he needs to modify its structure and has to perform all the allocation & deallocation code itself (which is good or bad depending on the context). This can generate efficient code (both in speed and size). From a library developer point of view, this is easy to design & code. You need internally a cast to go from a pointer to the known structure to the pointed object (a reverse of offsetof) that is generally type unsafe (except if mixed with the macro generating concept). This is quite limitation in what you can do: you can't move your objects so any container that has to move some objects is out-of-question (which means that you cannot use the most efficient container).
* Header files are included multiple times with different contexts (some different values given to defined macros) in order to generate different code for each type. From a user point of view, this creates a new step before using the container: an instantiating stage that has to be done once per type and per compilation unit (The user is responsible to create only one instance of the container, which can be troublesome if the library doesn't handle proper prefix for its naming convention). The debug of the library is generally easy and can generate fully specialized & efficient code. Incorrectly used, this can generate a lot of code bloat. Properly used, this can even create smaller code than the void pointer variant. The interface used to configure the library can be quite tiresome in case of a lot of specialized methods to configure: it doesn't enable to chain the configuration from a container to another one easily. It also cannot have heavy customization of the code.
* Macros are used to generate context-dependent C code enabling to generate code for different type. This is pretty much like the headers solution but with added flexibility. From a user point of view, this creates a new step before using the container: an instantiating stage that has to be done once per type and per compilation unit (The user is responsible to create only one instance of the container, which can be troublesome if the library doesn't handle proper prefix for its naming convention). This can generate fully specialized & efficient code. Incorrectly used, this can generate a lot of code bloat. Properly used, this can even create smaller code than the void pointer variant. From a library developer point of view, the library is harder to design and to debug: everything being expanded in one line, you can't step in the library (there is however a solution to overcome this limitation by adding another stage to the compilation process). You can however see the generated code by looking at the preprocessed file. You can perform heavy context-dependent customization of the code (transforming the macro preprocessing step into its own language). Properly done, you can also chain the methods from a container to another one easily, enabling expansion of the library. Errors within the macro expansion are generally hard to decipher, but errors in code using containers are easy to read and natural.

M\*LIB category is mainly the last one.
Some macros are also defined to access structure in a generic way, but they are optional.
There are also intrusive containers.

M\*LIB main added value compared to other libraries is its oplist feature
enabling it to chain the containers and/or use complex types in containers:
list of array of dictionary of C++ objects are perfectly supported by M\*LIB.

For the macro-preprocessing part, other libraries and reference also exist. For example:

* [Boost preprocessor](http://www.boost.org/doc/libs/1_63_0/libs/preprocessor/doc/index.html)
* [C99 Lambda](https://github.com/Leushenko/C99-Lambda)
* [C Preprocessor Tricks, Tips and Idioms](https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms)
* [CPP MAGIC](http://jhnet.co.uk/articles/cpp_magic)
* [MAP MACRO](https://github.com/swansontec/map-macro)
* [metalang99](https://github.com/Hirrolot/metalang99)
* [OrderPP](https://github.com/rofl0r/order-pp)
* [P99](http://p99.gforge.inria.fr/p99-html/)
* [Zlang](https://github.com/pfultz2/ZLang)

You can also consult [awesome-c-preprocessor](https://github.com/Hirrolot/awesome-c-preprocessor) for a more comprehensive list of libraries.

For the string part, there is the following libraries for example:

* [POTTERY STRING](https://github.com/ludocode/pottery/tree/develop/util/pottery/string)
* [SDS](https://github.com/antirez/sds)
* [STC - C99 Standard Container library](https://github.com/tylov/C99Containers)
* [STR -yet another string library for C language](https://github.com/maxim2266/str)
* [The Better String Library](http://bstring.sourceforge.net/) with a page that lists a lot of other string libraries.
* [VSTR](http://www.and.org/vstr/) with a [page](http://www.and.org/vstr/comparison) that lists a lot of other string libraries.


## API Documentation

The M*LIB reference card is available [here](http://htmlpreview.github.io/?https://github.com/P-p-H-d/mlib/blob/master/doc/Container.html).


### Generic methods

The generated containers tries to generate and provide a consistent interface:
their methods would behave the same for all generated containers.
This chapter will explain the generic interface.
In case of difference, it will be explained in the specific container.

In the following description:

* name is the prefix used for the container generation,
* name\_t refers to the type of the container,
* name\_it\_t refers to the iterator type of the container,
* type\_t refers to the type of the object stored in the container,
* key\_type\_t refers to the type of the key object used to associate an element to,
* value\_type\_t refers to the type of the value object used to associate an element to.
* name\_itref\_t refers to a pair of key and value for associative arrays.

An object shall be initialized (aka constructor) before being used by other methods.
It shall be cleared (aka destructor) after being used and before program termination.
An iterator has not destructor but shall be set before being used.

A container takes as input the
* name: it is a mandatory argument that is the prefix used to generate all functions and types,
* type: it is a mandatory argument that the basic element of the container,
* oplist: it is an optional argument that defines the methods associated to the type. The provided oplist (if provided) shall be the one that is associated to the type, otherwise it won't generate compilable code. If there is no oplist parameter provided, a globally registered oplist associated to the type is used if possible, or the basic oplist for basic C types is used. This oplist will be used to handle internally the objects of the container.

The 'type' given to any templating macro can be either
an integer, a float, a boolean, an enum, a named structure, a named union, a pointer to such types,
or a typedef alias of any C type:
in fact, the only constraint is that the preprocessing concatenation between 'type' and 'variable' into
'type variable' shall be a valid C expression.
Therefore the 'type' cannot be a C array or a function pointer
and you shall use a typedef as an intermediary named type for such types.

This generic interface is specified as follow:

##### void name\_init(name\_t container)

Initialize the container 'container' (aka constructor) to an empty container.
Also called the default constructor of the container.

##### void name\_init\_set(name\_t container, const name\_t ref)

Initialize the container 'container' (aka constructor) and set it to a copy of 'ref'.
This method is only created only if the INIT\_SET & SET methods are provided.
Also called the copy constructor of the container.

##### void name\_set(name\_t container, const name\_t ref)

Set the container 'container' to the a copy of 'ref'.
This method is only created only if the INIT\_SET & SET methods are provided.

##### void name\_init\_move(name\_t container, name\_t ref)

Initialize the container 'container' (aka constructor)
by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used (aka destructor).

##### void name\_move(name\_t container, name\_t ref)

Set the container 'container' (aka constructor)
by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used (aka destructor).

##### void name\_clear(name\_t container)

Clear the container 'container (aka destructor),
calling the CLEAR method of all the objects of the container and freeing memory.
The object can't be used anymore, except to be reinitialized with a constructor.

##### void name\_reset(name\_t container)

Reset the container clearing and freeing all objects stored in it.
The container becomes empty but remains initialized.

##### type\_t *name\_back(const name\_t container)

Return a pointer to the data stored in the back of the container.
This pointer should not be stored in a global variable.

##### type\_t *name\_front(const name\_t container)

Return a pointer to the data stored in the front of the container.
This pointer should not be stored in a global variable.

##### void name\_push(name\_t container, const type\_t value)
##### void name\_push\_back(name\_t container, const type\_t value)
##### void name\_push\_front(name\_t container, const type\_t value)

Push a new element in the container 'container'
as a copy of the object 'value'.
This method is created only if the INIT\_SET operator is provided.

The \_back suffixed method will push it in the back of the container.
The \_front suffixed method will push it in the front of the container.

##### type\_t *name\_push\_raw(name\_t container)
##### type\_t *name\_push\_back\_raw(name\_t container)
##### type\_t *name\_push\_front\_raw(name\_t container)

Push a new element in the container 'container'
without initializing it and returns a pointer to the **non-initialized** data.
The first thing to do after calling this function shall be to initialize the data
using the proper constructor of the object of type 'type\_t'.
This enables using more specialized constructor than the generic copy one.
The user should use other \_push function if possible rather than this one
as it is low level and error prone.
This pointer should not be stored in a global variable.

The \_back suffixed method will push it in the back of the container.
The \_front suffixed method will push it in the front of the container.

##### type\_t *name\_push\_new(name\_t container)
##### type\_t *name\_push\_back\_new(name\_t container)
##### type\_t *name\_push\_front\_new(name\_t container)

Push a new element in the container 'container'
and initialize it with the default constructor associated to the type 'type\_t'.
Return a pointer to the initialized object.
This pointer should not be stored in a global variable.
This method is only created only if the INIT method is provided.

The \_back suffixed method will push it in the back of the container.
The \_front suffixed method will push it in the front of the container.

##### void name\_push\_move(name\_t container, type\_t *value)
##### void name\_push\_back\_move(name\_t container, type\_t *value)
##### void name\_push\_front\_move(name\_t container, type\_t *value)

Push a new element in the container 'container' 
as a move from the object '\*value':
it will therefore steal as much resources from '\*value' as possible. 
Afterward '\*value' is cleared and cannot longer be used (aka. destructor).

The \_back suffixed method will push it in the back of the container.
The \_front suffixed method will push it in the front of the container.

##### void name\_emplace\[suffix\](name\_t container, args...)
##### void name\_emplace\_back\[suffix\](name\_t container, args...)
##### void name\_emplace\_front\[suffix\](name\_t container, args...)

Push a new element in the container 'container'
by initializing it with the provided arguments.
The provided arguments shall therefore match one of the constructor provided
by the EMPLACE\_TYPE operator.
There is one generated method per suffix defined in the EMPLACE\_TYPE operator,
and the 'suffix' in the generated method name corresponds to the suffix defined in
in the EMPLACE\_TYPE operator (it can be empty).
This method is created only if the EMPLACE\_TYPE operator is provided.
See emplace chapter for more details.

The \_back suffixed method will push it in the back of the container.
The \_front suffixed method will push it in the front of the container.

##### void name\_pop(type\_t *data, name\_t container)
##### void name\_pop\_back(type\_t *data, name\_t container)
##### void name\_pop\_front(type\_t *data, name\_t container)

Pop a element from the the container 'container';
and set '*data' to this value if data is not the NULL pointer,
otherwise it only pops the data.
This method is created if the SET or INIT\_MOVE operator is provided.

The \_back suffixed method will pop it from the back of the container.
The \_front suffixed method will pop it from the front of the container.

##### void name\_pop\_move(type\_t *data, name\_t container)
##### void name\_pop\_move\_back(type\_t *data, name\_t container)
##### void name\_pop\_move\_front(type\_t *data, name\_t container)

Pop a element from the container 'container'
and initialize and set '*data' to this value (aka. constructor)
by stealing as much resources from the container as possible.
data shall not be a NULL pointer.

The \_back suffixed method will pop it from the back of the container.
The \_front suffixed method will pop it from the front of the container.

##### bool name\_empty\_p(const name\_t container)

Return true if the container is empty, false otherwise.

##### void name\_swap(name\_t container1, name\_t container2)

Swap the container 'container1' and 'container2'.

##### void name\_set\_at(name\_t container, size_t key, type\_t value)
##### void name\_set\_at(name\_t container, key\_type\_t key, value\_type\_t value) [for associative array]

Set the element associated to 'key' of the container 'container' to 'value'.

If the container is sequence-like (like array), the key is an integer.
The selected element is the 'key'-th element of the container,
The index 'key' shall be within the size of the container.
This method is created if the INIT\_SET operator is provided.

If the container is associative-array like,
the selected element is the 'value' object associated to the 'key' object in the container.
It is created if it doesn't exist, overwritten otherwise.

##### void name\_emplace\_key[keysuffix](name\_t container, keyargs..., value\_type\_t value) [for associative array]
##### void name\_emplace\_val[valsuffix](name\_t container, key\_type\_t key, valargs...) [for associative array]
##### void name\_emplace\_key[keysuffix]\_val[valsuffix](name\_t container, keyargs..., valargs...) [for associative array]

Set the element associated to 'key' of the container 'container' to 'value'.
'key' (resp. value) is constructed from the provided args 'keyargs' (resp. valargs) if not provided.

keyargs (resp. valargs) shall therefore match one of the constructor provided
by the EMPLACE\_TYPE operator of the key (resp. the value).

There is:

* one generated method per suffix defined in the EMPLACE\_TYPE operator of the key,
* one generated method per suffix defined in the EMPLACE\_TYPE operator of the value,
* one generated method per pair of suffix defined in the EMPLACE\_TYPE operators of the key and value,

The 'suffix' in the generated method name corresponds to the suffix defined in
in the EMPLACE\_TYPE operator (it can be empty).
This method is created only if one EMPLACE\_TYPE operator is provided.
See emplace chapter for more details.


##### type\_t *name\_get(const name\_t container, size\_t key) [for sequence-like]
##### const type\_t *name\_cget(const name\_t container, size_t key) [for sequence-like]
##### type\_t *name\_get(const name\_t container, const type\_t key) [for set-like]
##### const type\_t *name\_cget(const name\_t container, const type\_t key) [for set-like]
##### value\_type\_t *name\_get(const name\_t container, const key\_type\_t key) [for associative array]
##### const value\_type\_t *name\_cget(const name\_t container, const key\_type\_t key) [for associative array]

Return a modifiable (resp. constant) pointer to 
the element associated to 'key' in the container.

If the container is sequence-like, the key is an integer.
The selected element is the 'key'-th element of the container,
the front being at the index 0, the last at the index (size-1).
The index 'key' shall be within the size of the container.
Therefore it will never return NULL in this case.

If the container is associative array like,
the selected element is the 'value' object associated to the 'key' object in the container.
If the key is not found, it returns NULL.

If the container is set-like,
the selected element is the 'value' object which is equal to the 'key' object in the container.
If the key is not found, it returns NULL.

This pointer remains valid until the container is modified by another method.
This pointer should not be stored in a global variable.

##### type\_t *name\_get\_emplace\[suffix\](const name\_t container, args...) [for set-like]
##### value\_type\_t * name\_get\_emplace\[suffix\](name\_t container, args...) [for associative array]

Return a modifiable (resp. constant) pointer to
the element associated to the key constructed from 'args' in the container.
The selected element is the 'value' object associated to the 'key' object in the container
for an associative array, or the element itself for a set.
If the key is not found, it returns NULL.

This pointer remains valid until the container is modified by another method.
This pointer should not be stored in a global variable.

##### type\_t *name\_safe\_get(name\_t container, size\_t key) [for sequence]
##### type\_t *name\_safe\_get(name\_t container, const type\_t key) [for set]
##### value\_type\_t *name\_safe\_get(name\_t container, const key\_type\_t key) [for associative array]

Return a modifiable pointer to 
the element associated to 'key' in the container,
creating a new element if it doesn't exist (ensuring therefore a safe 'get' operation).

If the container is sequence-like, key\_type\_t is an alias for size\_t and key an integer,
the selected element is the 'key'-th element of the container.
If the element doesn't exist, the container size will be increased 
if needed (creating new elements with the INIT operator),
which might increase the container to much in some cases.

The returned pointer cannot be NULL.
This method is only created only if the INIT method is provided.
This pointer remains valid until the array is modified by another method.
This pointer should not be stored in a global variable.

##### bool name\_erase(name\_t container, const size\_t key)
##### bool name\_erase(name\_t container, const type\_t key) [for set]
##### bool name\_erase(name\_t container, const key\_type\_t key) [for associative array]

Remove the element referenced by the key 'key' in the container 'container'.
Do nothing if 'key' is no present in the container.
Return true if the key was present and erased, false otherwise.

##### size\_t name\_size(const name\_t container)

Return the number elements of the container (its size).
Return 0 if there no element.

##### size\_t name\_capacity(const name\_t container)

Return the capacity of the container, i.e. the maximum number of elements
supported by the container before a reserve operation is needed to accommodate
new elements.

##### void name\_resize(name\_t container, size\_t size)

Resize the container 'container' to the size 'size',
increasing or decreasing the size of the container
by initializing new elements or clearing existing ones.
This method is only created only if the INIT method is provided.

##### void name\_reserve(name\_t container, size\_t capacity)

Extend or reduce the capacity of the 'container' to a rounded value based on 'capacity'.
If the given capacity is below the current size of the container, 
the capacity is set to a rounded value based on the size of the container.
Therefore a capacity of 0 can be used to perform a shrink-to-fit operation on the container,
i.e. reducing the container usage to the minimum possible allocation.

##### void name\_it(name\_it\_t it, const name\_t container)

Set the iterator 'it' to the first element of the container 'container'.
There is no destructor associated to this initialization.

##### void name\_it\_set(name\_it\_t it, const name\_it\_t ref)

Set the iterator 'it' to the iterator 'ref'.
There is no destructor associated to this initialization.

##### void name\_it\_end(name\_it\_t it, const name\_t container)

Set the iterator 'it' to a non valid element of the container.
There is no destructor associated to this initialization.

##### bool name\_end\_p(const name\_it\_t it)

Return true if the iterator doesn't reference a valid element anymore.

##### bool name\_last\_p(const name\_it\_t it)

Return true if the iterator references the last element of the container
or if the iterator doesn't reference a valid element anymore.

##### bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)

Return true if the iterator 'it1' references the same element than the iterator 'it2'.

##### void name\_next(name\_it\_t it)

Move the iterator to the next element of the container.

##### void name\_previous(name\_it\_t it)

Move the iterator 'it' to the previous element of the container.

##### type\_t *name\_ref(name\_it\_t it)
##### const type\_t *name\_cref(const name\_it\_t it)
##### name\_itref\_t *name\_ref(name\_it\_t it)  [for associative array]
##### const name\_itref\_t *name\_cref(name\_it\_t it)  [for associative array]

Return a modifiable (resp. constant) pointer to the element pointed by the iterator.
For associative-array like container, this element is the pair composed of
the key ('key' field) and the associated value ('value' field);
otherwise this element is simply the basic type of the container (type\_t).

This pointer should not be stored in a global variable.
This pointer remains valid until the container is modified by another method.

##### void name\_insert(name\_t container, const name\_it\_t it, const type\_t x)

Insert the object 'x' after the position referenced by the iterator 'it' in the container 'container'
or if the iterator 'it' doesn't reference anymore to a valid element of the container,
it is added as the first element of the container.
'it' shall be an iterator of the container 'container'.

##### void name\_remove(name\_t container, name\_it\_t it)

Remove the element referenced by the iterator 'it' from the container 'container'.
'it' shall be an iterator of the container 'container'.
Afterwards, 'it' references the next element of the container if it exists,
or not a valid element otherwise.

##### bool name\_equal\_p(const name\_t container1, const name\_t container2)

Return true if both containers 'container1' and 'container2' are considered equal.
This method is only created only if the EQUAL method is provided.

##### size\_t name\_hash(const name\_t container)

Return a fast hash value of the container 'container',
suitable to be used by a dictionary.
This method is only created only if the HASH method is provided.

##### void name\_get\_str(string\_t str, const name\_t container, bool append)

Set 'str' to the formatted string representation of the container 'container' if 'append' is false
or append 'str' with this representation if 'append' is true.
This method is only created only if the GET\_STR method is provided.

##### bool name\_parse\_str(name\_t container, const char str[], const char **endp)

Parse the formatted string 'str',
that is assumed to be a string representation of a container,
and set 'container' to this representation.
It returns true if success, false otherwise.
If endp is not NULL, it sets '*endp' to the pointer of the first character not
decoded by the function (or where the parsing fails).
This method is only created only if the PARSE\_STR & INIT methods are provided.

It is ensured that the container gets from parsing a formatted string representation
gets from name\_get\_str and the original container are equal.

##### void name\_out\_str(FILE *file, const name\_t container)

Generate a formatted string representation of the container 'container'
and outputs it into the file 'file'.
The file 'file' shall be opened in write text mode and without error.
This method is only created only if the OUT\_STR methods is provided.

##### bool name\_in\_str(name\_t container, FILE *file)

Read from the file 'file' a formatted string representation of a container
and set 'container' to this representation.
It returns true if success, false otherwise.
This method is only created only if the IN\_STR & INIT methods are provided.

It is ensured that the container gets from parsing a formatted string representation
gets from name\_out\_str and the original container are equal.

##### m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)

Output the container 'container' into the serializing object 'serial'.
How and where it is output depends on the serializing object.
It returns M\_SERIAL\_OK\_DONE in case of success,
or M\_SERIAL\_FAILURE in case of failure.
In case of failure, the serializing object is in an unspecified but valid state.
This method is only created only if the OUT\_SERIAL methods is provided.

##### m\_serial\_return\_code\_t name\_in\_serial(name\_t container, m\_serial\_read\_t serial)

Read from the serializing object 'serial' a representation of a container
and set 'container' to this representation.
It returns M\_SERIAL\_OK\_DONE in case of success,
or M\_SERIAL\_FAILURE in case of failure.
In case of failure, the serializing object is in an unspecified but valid state.
This method is only created only if the IN\_SERIAL & INIT methods are provided.

It is ensured that the container gets from parsing a representation
gets from name\_out\_serial and the original container are equal (using the same type of serialization object).


### M-LIST

This header is for creating [singly linked list](https://en.wikipedia.org/wiki/Linked_list).

A linked list is a linear collection of elements,
in which each element points to the next, all of then representing a sequence.

A fundamental property of a list is that the objects created within the list
will remain at their initialized address, and won't moved due to
operations done on the list (except if it is removed).
Therefore a returned pointer to an element of the container
remains valid until this element is destroyed in the container.

#### LIST\_DEF(name, type [, oplist])
#### LIST\_DEF\_AS(name, name\_t, name\_it\_t, type [, oplist])

LIST\_DEF defines the singly linked list named 'name\_t'
that contains objects of type 'type' and their associated methods as "static inline" functions.
'name' shall be a C identifier that will be used to identify the list.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

For this container, the back is always the first element,
and the front is the last element: the list grows from the back.
Therefore, the iteration of this container using iterator objects will
go from the back element to the front element (contrary to an array).

Even if it provides random access functions, theses access are slow
and should be avoided: it iterates linearly over all the elements of the container
until it reaches the requested element. The size method has the same drawback.

The push / pop methods of the container always operate on the back of the container.

LIST\_DEF\_AS is the same as LIST\_DEF
except the name of the types 'name\_t', 'name\_it\_t' are provided by the user,
and not computed from the 'name' prefix.

Example:

```C
	#include <stdio.h>
	#include <gmp.h>
	#include "m-list.h"
	
	#define MPZ_OUT_STR(stream, x) mpz_out_str(stream, 0, x)
	LIST_DEF(list_mpz, mpz_t,                                           \
	          (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), OUT_STR(MPZ_OUT_STR)))
	
	int main(void) {
	  list_mpz_t a;
	  list_mpz_init(a);
	  mpz_t x;
	  mpz_init_set_ui(x, 16);
	  list_mpz_push_back (a, x);
	  mpz_set_ui(x, 45);             
	  list_mpz_push_back (a, x);
	  mpz_clear(x);
	  printf ("LIST is: ");
	  list_mpz_out_str(stdout, a);
	  printf ("\n");
	  printf ("First element is: ");
	  mpz_out_str(stdout, 0, *list_mpz_back(a));
	  printf ("\n");
	  list_mpz_clear(a);
	}
```

If the given oplist contain the method MEMPOOL, then LIST\_DEF macro will create
a dedicated mempool that is named with the given value of the method MEMPOOL.
This mempool (see mempool chapter) is optimized for this kind of list:

* it creates a mempool named by the concatenation of "name" and "\_mempool",
* it creates a variable named by the value of the method MEMPOOL with the linkage
defined by the value of the method MEMPOOL\_LINKAGE (can be extern, static or none).
This variable is shared by all lists of the same type.
* it links the memory allocation of the list to use this mempool with this variable.

mempool create heavily efficient list. However it is only worth the
effort in some heavy performance context.
Using mempool should be therefore avoided except in performance critical code.
The created mempool has to be explicitly initialized before using any
methods of the created list by calling  mempool\_list\_name\_init(variable)
and cleared by calling mempool\_list\_name\_clear(variable).

Example:

```C
        LIST_DEF(list_uint, unsigned int, (MEMPOOL(list_mpool), MEMPOOL_LINKAGE(static)))

        static void test_list (size_t n)
        {
          list_uint_mempool_init(list_mpool);
          M_LET(a1, LIST_OPLIST(uint)) {
              for(size_t i = 0; i < n; i++)
                  list_uint_push_back(a1, rand_get() );
          }
          list_uint_mempool_clear(list_mpool);
        }
```

#### LIST\_OPLIST(name [, oplist])

Return the oplist of the list defined by calling LIST\_DEF
& LIST\_DUAL\_PUSH\_DEF with name & oplist. 
If there is no given oplist, the basic oplist for basic C types is used.
There is no globally registered oplist support.

#### LIST\_INIT\_VALUE()

Define an initial value that is suitable to initialize global variable(s)
of type 'list' as created by LIST\_DEF or LIST\_DEF\_AS.
It enables to create a list as a global variable and to initialize it.

The list shall still be cleared manually to avoid leaking memory.

Example:

```C
        LIST_DEF(list_int, int)
        list_int_t my_list = LIST_INIT_VALUE();
```


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the list of 'type' objects.

#### name\_it\_t

Type of an iterator over this list.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t list)
* void name\_init\_set(name\_t list, const name\_t ref)
* void name\_set(name\_t list, const name\_t ref)
* void name\_init\_move(name\_t list, name\_t ref)
* void name\_move(name\_t list, name\_t ref)
* void name\_clear(name\_t list)
* void name\_reset(name\_t list)
* type *name\_back(const name\_t list)
* void name\_push\_back(name\_t list, const type value)
* type *name\_push\_raw(name\_t list)
* type *name\_push\_new(name\_t list)
* void name\_push\_move(name\_t list, type *value)
* void name\_emplace\_back\[suffix\](name\_t list, args...)
* void name\_pop\_back(type *data, name\_t list)
* void name\_pop\_move(type *data, name\_t list)
* bool name\_empty\_p(const name\_t list)
* void name\_swap(name\_t list1, name\_t list2)
* void name\_it(name\_it\_t it, const name\_t list)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_end(name\_it\_t it, const name\_t list)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_cref(const name\_it\_t it)
* type *name\_get(const name\_t list, size\_t i)
* const type *name\_cget(const name\_t list, size\_t i)
* size\_t name\_size(const name\_t list)
* void name\_insert(name\_t list, const name\_it\_t it, const type x)
* void name\_remove(name\_t list, name\_it\_t it)
* void name\_get\_str(string\_t str, const name\_t list, bool append)
* bool name\_parse\_str(name\_t list, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t list)
* bool name\_in\_str(name\_t list, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t list1, const name\_t list2)
* size\_t name\_hash(const name\_t list)

#### Specialized methods

The following specialized methods are  automatically created by the previous definition macro:

##### void name\_splice\_back(name\_t list1, name\_t list2, name\_it\_t it)

Move the element referenced by the iterator 'it'
from the list 'list2' to the back position of the list 'list1'.
'it' shall be an iterator of 'list2'.
Afterwards, 'it' references the next element of the list if it exists,
or not a valid element otherwise.

##### void name\_splice\_at(name\_t list1, name\_it\_t it1, name\_t list2, name\_it\_t it2)

Move the element referenced by the iterator 'it2' from the list 'list2'
to the position just after 'it1' in the list 'list1'.
(If 'it1' is not a valid position, it inserts it at the back just like name\_insert).
'it1' shall be an iterator of 'list1'.
'it2' shall be an iterator of 'list2'.
Afterwards, 'it2' references the next element of the list if it exists,
or not a valid element otherwise,
and 'it1' references the inserted element in 'list1'.

##### void name\_splice(name\_t list1, name\_t list2)

Move all the element of the list 'list2' into the list 'list1",
moving the last element of 'list2' after the first element of 'list1'.
Afterwards, 'list2' remains initialized but is emptied.

##### void name\_reverse(name\_t list)

Reverse the order of the list.


#### LIST\_DUAL\_PUSH\_DEF(name, type[, oplist])
#### LIST\_DUAL\_PUSH\_DEF\_AS(name, name\_t, name\_it\_t, type [, oplist])

LIST\_DUAL\_PUSH\_DEF defines the singly linked list named 'name\_t'
that contains the objects of type 'type' and their associated methods as "static inline" functions.

The only difference with the list defined by LIST\_DEF is
the support of the method for PUSH\_FRONT in addition to the one for PUSH\_BACK 
(so the DUAL PUSH name).
However, there is still only POP method (associated to POP\_BACK).
The list is a bit bigger to be able to handle such method to work, but not the nodes.

This list is therefore able to represent:
* either a stack (PUSH\_BACK + POP\_BACK)
* or a queue (PUSH\_FRONT + POP\_BACK).

LIST\_DUAL\_PUSH\_DEF\_AS is the same as LIST\_DUAL\_PUSH\_DEF
except the name of the types name\_t, name\_it\_t are provided by the user.

See LIST\_DEF for more details and constraints.

Example:

```C
	#include <stdio.h>
	#include <gmp.h>
	#include "m-list.h"
	
	#define MPZ_OUT_STR(stream, x) mpz_out_str(stream, 0, x)
	LIST_DUAL_PUSH_DEF(list_mpz, mpz_t,                                           \
	          (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), OUT_STR(MPZ_OUT_STR)))
	
	int main(void) {
	  list_mpz_t a;
	  list_mpz_init(a);
	  mpz_t x;
	  mpz_init_set_ui(x, 16);
	  list_mpz_push_back (a, x);
	  mpz_set_ui(x, 45);             
	  list_mpz_push_back (a, x);
	  mpz_clear(x);
	  printf ("LIST is: ");
	  list_mpz_out_str(stdout, a);
	  printf ("\n");
	  printf ("First element is: ");
	  mpz_out_str(stdout, 0, *list_mpz_back(a));
	  printf ("\n");
	  list_mpz_clear(a);
	}
```
The methods follow closely the methods defined by LIST\_DEF.

#### LIST\_DUAL\_PUSH\_INIT\_VALUE()

Define an initial value that is suitable to initialize global variable(s)
of type 'list' as created by LIST\_DUAL\_PUSH\_DEF or LIST\_DUAL\_PUSH\_DEF\_AS.
It enables to create a list as a global variable and to initialize it.

The list should still be cleared manually to avoid leaking memory.

Example:

```C
        LIST_DUAL_PUSH_DEF(list_int, int)
        list_int_t my_list = LIST_DUAL_PUSH_INIT_VALUE();
```


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the list of 'type'.

#### name\_it\_t

Type of an iterator over this list.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t list)
* void name\_init\_set(name\_t list, const name\_t ref)
* void name\_set(name\_t list, const name\_t ref)
* void name\_init\_move(name\_t list, name\_t ref)
* void name\_move(name\_t list, name\_t ref)
* void name\_clear(name\_t list)
* void name\_reset(name\_t list)
* type *name\_back(const name\_t list)
* void name\_push\_back(name\_t list, type value)
* type *name\_push\_back\_raw(name\_t list)
* type *name\_push\_back\_new(name\_t list)
* void name\_push\_back\_move(name\_t list, type *value)
* void name\_emplace\_back\[suffix\](name\_t list, args...)
* const type *name\_front(const name\_t list)
* void name\_push\_front(name\_t list, type value)
* type *name\_push\_front\_raw(name\_t list)
* type *name\_push\_front\_new(name\_t list)
* void name\_push\_front\_move(name\_t list, type *value)
* void name\_emplace\_front\[suffix\](name\_t list, args...)
* void name\_pop\_back(type *data, name\_t list)
* void name\_pop\_move(type *data, name\_t list)
* bool name\_empty\_p(const name\_t list)
* void name\_swap(name\_t list1, name\_t list2)
* void name\_it(name\_it\_t it, name\_t list)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_end(name\_it\_t it, const name\_t list)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_cref(const name\_it\_t it)
* size\_t name\_size(const name\_t list)
* void name\_insert(name\_t list, const name\_it\_t it, const type x)
* void name\_remove(name\_t list, name\_it\_t it)
* void name\_get\_str(string\_t str, const name\_t list, bool append)
* bool name\_parse\_str(name\_t list, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t list)
* bool name\_in\_str(name\_t list, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t list1, const name\_t list2)
* size\_t name\_hash(const name\_t list)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_splice\_back(name\_t list1, name\_t list2, name\_it\_t it)

Move the element pointed by 'it'
from the list 'list2' to the back position of the list 'list1'.
'it' shall be an iterator of 'list2'.
Afterwards, 'it' points to the next element of 'list2'.

##### void name\_splice(name\_t list1, name\_t list2)

Move all the element of the list 'list2' into the list 'list1",
moving the last element of 'list2' after the first element of 'list1'.
Afterwards, 'list2' is emptied.

##### void name\_reverse(name\_t list)

Reverse the order of the list.



### M-ARRAY

An [array](https://en.wikipedia.org/wiki/Array_data_structure) is a growable collection of element that are individually indexable.

#### ARRAY\_DEF(name, type [, oplist])
#### ARRAY\_DEF\_AS(name, name\_t, name\_it\_t, type [, oplist])

ARRAY\_DEF defines the array 'name\_t' that contains the objects of type 'type'
in a sequence
and its associated methods as "static inline" functions.
Compared to C arrays, the created methods handle automatically the size (aka growable array).
'name' shall be a C identifier that will be used to identify the container.
It will be used to create all the types (including the iterator)
and functions to handle the container.

The provided oplist shall have at least the following operators (CLEAR).
It should also provide the following operators (INIT, INIT\_SET, SET),
so that at least some methods are defined for the array!

The push / pop methods of the container always operate on the back of the container,
acting like a stack-like container.

ARRAY\_DEF\_AS is the same as ARRAY\_DEF except the name of the types name\_t, name\_it\_t
are provided by the user.

Example:

```C
	#include <stdio.h>
	#include <gmp.h>
	#include "m-array.h"
	
	#define MPZ_OUT_STR(stream, x) mpz_out_str(stream, 0, x)
	ARRAY_DEF(array_mpz, mpz_t,                                           \
	          (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), OUT_STR(MPZ_OUT_STR)))
		
	int main(void) {
	  array_mpz_t a;
	  array_mpz_init(a);
	  mpz_t x;
	  mpz_init_set_ui(x, 16);
	  array_mpz_push_back (a, x);
	  mpz_set_ui(x, 45);             
	  array_mpz_push_back (a, x);
	  mpz_clear(x);
	  printf ("ARRAY is: ");
	  array_mpz_out_str(stdout, a);
	  printf ("\n");
	  printf ("First element is: ");
	  mpz_out_str(stdout, 0, *array_mpz_get(a, 0));
	  printf ("\n");
	  array_mpz_clear(a);
	}
```

#### ARRAY\_OPLIST(name [, oplist])

Return the oplist of the array defined by calling ARRAY\_DEF with name & oplist. 
If there is no given oplist, the basic oplist for basic C types is used.

#### ARRAY\_INIT\_VALUE()

Define an initial value that is suitable to initialize global variable(s)
of type 'array' as created by ARRAY\_DEF or ARRAY\_DEF\_AS.
It enables to create an array as a global variable and to initialize it.

The array should still be cleared manually to avoid leaking memory.

Example:

```C
        ARRAY_DEF(array_int_t, int)
        array_int_t my_array = ARRAY_INIT_VALUE();
```


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the array of 'type'.

#### name\_it\_t

Type of an iterator over this array.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t array)
* void name\_init\_set(name\_t array, const name\_t ref)
* void name\_set(name\_t array, const name\_t ref)
* void name\_init\_move(name\_t array, name\_t ref)
* void name\_move(name\_t array, name\_t ref)
* void name\_clear(name\_t array)
* void name\_reset(name\_t array)
* type *name\_push\_raw(name\_t array)
* void name\_push\_back(name\_t array, const type value)
* type *name\_push\_new(name\_t array)
* void name\_push\_move(name\_t array, type *val)
* void name\_emplace\_back\[suffix\](name\_t array, args...)
* void name\_pop\_back(type *data, name\_t array)
* void name\_pop\_move(type *data, name\_t array)
* type *name\_front(const name\_t array)
* type *name\_back(const name\_t array)
* void name\_set\_at(name\_t array, size\_t i, type value)
* type *name\_get(name\_t array, size\_t i)
* const type *name\_cget(const name\_t it, size\_t i)
* type *name\_safe\_get(name\_t array, size\_t i)
* bool name\_empty\_p(const name\_t array)
* size\_t name\_size(const name\_t array)
* size\_t name\_capacity(const name\_t array)
* void name\_resize(name\_t array, size\_t size)
* void name\_reserve(name\_t array, size\_t capacity)
* void name\_swap(name\_t array1, name\_t array2)
* void name\_it(name\_it\_t it, name\_t array)
* void name\_it\_last(name\_it\_t it, name\_t array)
* void name\_it\_end(name\_it\_t it, name\_t array)
* void name\_it\_set(name\_it\_t it1, name\_it\_t it2)
* bool name\_end\_p(name\_it\_t it)
* bool name\_last\_p(name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* void name\_previous(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_cref(const name\_it\_t it)
* void name\_remove(name\_t array, name\_it\_t it)
* void name\_insert(name\_t array, name\_it\_t it, const type x)
* void name\_get\_str(string\_t str, const name\_t array, bool append)
* bool name\_parse\_str(name\_t array, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t array)
* bool name\_in\_str(name\_t array, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t array1, const name\_t array2)
* size\_t name\_hash(const name\_t array)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_push\_at(name\_t array, size\_t key, const type x)

Push a new element into the position 'key' of the array 'array' 
with the value 'value'.
All elements after the position 'key' (included) will be moved in the array towards the back,
and the array will have one more element.
'key' shall be a valid position of the array: from 0 to the size of array (included).
This method is created only if the INIT\_SET operator is provided.

##### void name\_pop\_until(name\_t array, array\_it\_t position)

Pop all elements of the array 'array' from 'position' to the back of the array,
while clearing them.
This method is created only if the INIT operator is provided.

##### void name\_pop\_at(type *dest, name\_t array, size\_t key)

Set *dest to the value the element 'key' if dest is not NULL,
then remove the element 'key' from the array (decreasing the array size).
'key' shall be within the size of the array.
This method is created only if the SET or INIT\_MOVE operator is provided.

##### void name\_remove\_v(name\_t array, size\_t i, size\_t j)

Remove the element 'i' (included) to the element 'j' (excluded)
from the array.
'i' and 'j' shall be within the size of the array, and i < j.

##### void name\_insert\_v(name\_t array, size\_t i, size\_t j)

Insert from the element 'i' (included) to the element 'j' (excluded)
new empty elements to the array.
'i' and 'j' shall be within the size of the array, and i < j.
This method is only defined if the type of the element defines an INIT method.

##### void name\_swap\_at(name\_t array, size\_t i, size\_t j)

Swap the elements 'i' and 'j' of the array 'array'.
'i' and 'j' shall reference valid elements of the array.
This method is created if the INIT\_SET or INIT\_MOVE operator is provided.

##### void name\_special\_sort(name\_t array)

Sort the array 'array'.
This method is defined if the type of the element defines CMP method.
This method uses the qsort function of the C library.

##### void name\_special\_stable\_sort(name\_t array)

Sort the array 'array' using a stable sort.
This method is defined if the type of the element defines CMP and SWAP and SET methods.
This method provides an ad-hoc implementation of the stable sort.
In practice, it is faster than the \_sort method for small types and fast
comparisons.

##### void name\_splice(name\_t array1, name\_t array2)

Move all the elements of the array 'array2' to the end of the array 'array1'.
Afterwards, 'array2' is empty.



### M-DEQUE

This header is for creating [double-ended queue](https://en.wikipedia.org/wiki/Double-ended_queue) (or deque). 
A deque is an abstract data type that generalizes a queue, 
for that elements can be added to or removed from either the front (head) or back (tail)

#### DEQUE\_DEF(name, type [, oplist])
#### DEQUE\_DEF\_AS(name, name\_t, name\_it\_t, type [, oplist])

DEQUE\_DEF defines the deque 'name\_t' that contains the objects of type 'type' and its associated methods as "static inline" functions.
'name' shall be a C identifier that will be used to identify the deque.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT, INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

The algorithm complexity to access random elements is in O(ln(n)).
Removing an element may unbalance the deque, which breaks the promise
of algorithm complexity for the _get method.

DEQUE\_DEF\_AS is the same as DEQUE\_DEF
except the name of the types name\_t, name\_it\_t are provided.

Example:

```C
	#include <stdio.h>
	#include <gmp.h>
	#include "m-deque.h"
	
	DEQUE_DEF(deque_mpz, mpz_t,                                           \
	          (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))
	
	int main(void) {
	  deque_mpz_t a;
	  deque_mpz_init(a);
	  mpz_t x;
	  mpz_init_set_ui(x, 16);
	  deque_mpz_push_back (a, x);
	  mpz_set_ui(x, 45);             
	  deque_mpz_push_front (a, x);
	  deque_mpz_pop_back(&x, a);
	  mpz_set_ui(x, 5);
	  deque_mpz_push_back (a, x);
	  mpz_clear(x);
	
	  printf ("First element is: ");
	  mpz_out_str(stdout, 0, *deque_mpz_front(a));
	  printf ("\n");
	  printf ("Last element is: ");
	  mpz_out_str(stdout, 0, *deque_mpz_back(a));
	  printf ("\n");
	  deque_mpz_clear(a);
	}
```


#### DEQUE\_OPLIST(name [, oplist])

Return the oplist of the deque defined by calling DEQUE\_DEF with name & oplist. 


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the deque of 'type'.

#### name\_it\_t

Type of an iterator over this deque.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t deque)
* void name\_init\_set(name\_t deque, const name\_t ref)
* void name\_set(name\_t deque, const name\_t ref)
* void name\_init\_move(name\_t deque, name\_t ref)
* void name\_move(name\_t deque, name\_t ref)
* void name\_clear(name\_t deque)
* void name\_reset(name\_t deque)
* type *name\_back(const name\_t deque)
* void name\_push\_back(name\_t deque, type value)
* type *name\_push\_back\_raw(name\_t deque)
* type *name\_push\_back\_new(name\_t deque)
* void name\_emplace\_back\[suffix\](name\_t list, args...)
* void name\_pop\_back(type *data, name\_t deque)
* type *name\_front(const name\_t deque)
* void name\_push\_front(name\_t deque, type value)
* type *name\_push\_front\_raw(name\_t deque)
* type *name\_push\_front\_new(name\_t deque)
* void name\_emplace\_front\[suffix\](name\_t list, args...)
* void name\_pop\_front(type *data, name\_t deque)
* bool name\_empty\_p(const name\_t deque)
* void name\_swap(name\_t deque1, name\_t deque2)
* void name\_it(name\_it\_t it, name\_t deque)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* void name\_previous(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_cref(const name\_it\_t it)
* void name\_remove(name\_t deque, name\_it\_t it)
* type *name\_get(const name\_t deque, size\_t i)
* const type *name\_cget(const name\_t deque, size\_t i)
* size\_t name\_size(const name\_t deque)
* void name\_get\_str(string\_t str, const name\_t deque, bool append)
* bool name\_parse\_str(name\_t deque, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t deque)
* bool name\_in\_str(name\_t deque, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t deque1, const name\_t deque2)
* size\_t name\_hash(const name\_t deque)
* void name\_swap\_at(name\_t deque, size\_t i, size\_t j)



### M-DICT

A [dictionary](https://en.wikipedia.org/wiki/Associative_array) (or associative array, map, symbol table)
is an abstract data type composed of a collection of (key, value) pairs,
such that each possible key appears at most once in the collection,
and is associated to only one value.
It is possible to search for a key in the dictionary and get back its value.

Several dictionaries are proposed. The "best" to use depends on the data type
and in particular:

* the size of the data,
* the inner cost of copying the object,
* the inner cost of computing the hash of the object,
* the inner cost of comparing the objects,
* the load factor.

For small, fast types (integer, or floats, or pair of such types),
DICT\_OA\_DEF2 may be the best to use (but slightly more complex to instantiate).
For medium type, DICT\_DEF2 with mempool activated may be better.
For even larger object, DICT\_STOREHASH\_DEF2 may be better.
But for most uses, DICT\_DEF2 should be good enough.

#### DICT\_DEF2(name, key\_type[, key\_oplist], value\_type[, value\_oplist])
#### DICT\_DEF2\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, key\_type[, key\_oplist], value\_type[, value\_oplist])

DICT\_DEF2 defines the dictionary 'name\_t' and its associated methods as "static inline" functions as an associative array of 'key\_type' to 'value\_type'.

'name' shall be a C identifier that will be used to identify the dictionary.
It will be used to create all the types (including the iterator and the iterated object type)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

Both oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.
The key\_oplist shall also define the additional operators (HASH and EQUAL).

Current implementation uses chained Hash-Table and as such, elements in the dictionary are **unordered**. 
However, elements are not moved on insertion / delete of other elements:
even if the iterator may become invalid, the referenced element remains unmoved.

The \_set\_at method overwrites the already existing value if 'key' is already present in the dictionary (contrary to C++).

The iterated object type 'name##\_itref\_t' is a pair of key\_type and value\_type.

What is exactly the "first" element for the iteration is not specified.
It is only ensured that all elements of the dictionary are explored
by going from "first" to "end".

DICT\_DEF2\_AS is the same as DICT\_DEF2
except the name of the types name\_t, name\_it\_t, name\_itref\_t are provided.

Example:

```C
	#include <stdio.h>
	#include "m-string.h"
	#include "m-dict.h"
	
	DICT_DEF2(dict_string, string_t, unsigned)
	
	int main(void) {
	  dict_string_t a;
	  dict_string_init(a);
	  string_t x;
	  string_init_set_str(x, "This is an example");
	  dict_string_set_at (a, x, 1);
	  string_set_str(x, "This is an another example");
	  dict_string_set_at (a, x, 2);
	
	  string_set_str(x, "This is an example");
	  unsigned *val = dict_string_get(a, x);
	  printf ("Value of %s is %u\n", string_get_cstr(x), *val);
	  string_clear(x);
	
	  printf ("Dictionary is: ");
	  dict_string_out_str(stdout, a);
	  printf ("\n");
	  dict_string_clear(a);
	}
```


#### DICT\_STOREHASH\_DEF2(name, key\_type[, key\_oplist], value\_type[, value\_oplist])
#### DICT\_STOREHASH\_DEF2\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, key\_type[, key\_oplist], value\_type[, value\_oplist])

DICT\_STOREHASH\_DEF2 defines the dictionary 'name\_t' and its associated methods as "static inline" functions
just like DICT\_DEF2.

The only difference is that it stores (caches) the hash of each key alongside the key in the dictionary.
This enables the container to avoid recomputing it in some occasions resulting in faster
dictionary if the hash is costly to compute (which is usually the case for large object), or slower otherwise.

DICT\_STOREHASH\_DEF2\_AS is the same as DICT\_STOREHASH\_DEF2
except the name of the types name\_t, name\_it\_t, name\_itref\_t are provided.


#### DICT\_OA\_DEF2(name, key\_type[, key\_oplist], value\_type[, value\_oplist])
#### DICT\_OA\_DEF2\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, key\_type[, key\_oplist], value\_type[, value\_oplist])

DICT\_OA\_DEF2 defines the dictionary 'name\_t' and its associated methods
as "static inline" functions much like DICT\_DEF2.
The difference is that it uses an Open Addressing Hash-Table as container
(breaking the property of not-moving object on dictionary modification).

The key\_oplist shall also define the additional operators:
**OOR\_EQUAL** and **OOR\_SET**

The Out-Of-Range operators (OOR\_EQUAL and OOR\_SET) are used to store uninitialized keys
in the dictionary and be able to detect it. This enables avoiding a separate bit-field
to know the state of the entry in the dictionary (which increases memory usage and is
cache unfriendly).

The elements may move when inserting / deleting other elements (and not just the iterators).

This implementation is in general faster for small types of keys
(like integer or float) but may be slower for larger types.

DICT\_OA\_DEF2\_AS is the same as DICT\_OA\_DEF2
except the name of the types name\_t, name\_it\_t, name\_itref\_t are provided.

Example:

```C
	#include <stdio.h>
	#include "m-string.h"
	#include "m-dict.h"

	// Define an Out Of Range equal function
	static inline bool oor_equal_p(unsigned k, unsigned char n) {
	  return k == (unsigned)(-n-1);
	}
	// Define an Out Of Range function
	static inline void oor_set(unsigned *k, unsigned char n) {
	  *k = (unsigned)(-n-1);
	}
	
	DICT_OA_DEF2(dict_unsigned, unsigned, M_OPEXTEND(M_BASIC_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(API_2(oor_set))), long long, M_BASIC_OPLIST)
	
	unsigned main(void) {
	  dict_unsigned_t a;
	  dict_unsigned_init(a);
	  dict_unsigned_set_at (a, 13566, 14890943049);
	  dict_unsigned_set_at (a, 656, -2);
	
	  long long *val = dict_unsigned_get(a, 458);
	  printf ("Value of %d is %p\n", 458, val); // Not found value
	  val = dict_unsigned_get(a, 656);
	  printf ("Value of %d is %lld\n", 656, *val);
	
	  dict_unsigned_clear(a);
	}
```

#### DICT\_OPLIST(name[, key\_oplist, value\_oplist])

Return the oplist of the dictionary defined by calling any DICT\_*\_DEF2 with name & key\_oplist & value\_oplist. 


#### DICT\_SET\_DEF(name, key\_type[, key\_oplist])
#### DICT\_SET\_DEF\_AS(name,  name\_t, name\_it\_t, key\_type[, key\_oplist])

DICT\_SET\_DEF defines the dictionary set 'name\_t' and its associated methods as "static inline" functions.
A dictionary set is a specialized version of a dictionary with no value (only keys).

'name' shall be a C identifier that will be used to identify the dictionary.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT\_SET, SET, CLEAR, HASH and EQUAL),
otherwise it won't generate compilable code.

The _push method will overwrite the already existing value if 'key' is already present in the dictionary (contrary to C++).

What is exactly the "first" element for the iteration is not specified.
It is only ensured that all elements of the dictionary are explored
by going from "first" to "end".

DICT\_SET\_DEF\_AS is the same as DICT\_SET\_DEF
except the name of the types name\_t, name\_it\_t are provided.

Example:

```C
	#include <stdio.h>
	#include "m-string.h"
	#include "m-dict.h"
	
	DICT_SET_DEF(set_string, double)
	
	unsigned main(void) {
	  set_string_t a;
	  set_string_init(a);
	  set_string_push (a, 13566);
	  set_string_push (a, 656);
	
	  double *val = set_string_get(a, 458);
	  printf ("Value of %d is %p\n", 458, val); // Not found value
	  val = set_string_get(a, 656);
	  printf ("Value of %d is %f\n", 656, *val);
	
	  printf("Set is ");
	  set_string_out_str(stdout, a);
	  printf("\n");
	  set_string_clear(a);
	}
```


#### DICT\_OASET\_DEF(name, key\_type[, key\_oplist])
#### DICT\_OASET\_DEF\_AS(name,  name\_t, name\_it\_t, key\_type[, key\_oplist])

DICT\_OASET\_DEF defines the dictionary set 'name\_t' and its associated methods as "static inline" functions just like DICT\_SET\_DEF.
The difference is that it uses an Open Addressing Hash-Table as container.

The key\_oplist shall also define the additional operators:
**OOR\_EQUAL** and **OOR\_SET**

The elements may move when inserting / deleting other elements (and not just the iterators).

This implementation is in general faster for small types of keys
(like integer) but slower for larger types.

DICT\_OASET\_DEF\_AS is the same as DICT\_OASET\_DEF
except the name of the types name\_t, name\_it\_t are provided.


#### DICT\_SET\_OPLIST(name[, key\_oplist])

Return the oplist of the set defined by calling DICT\_SET\_DEF (or DICT\_OASET\_DEF) with name & key\_oplist.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the dictionary which
* either associate 'key\_type' to 'value\_type',
* or store unique element 'key\_type'.

##### name\_it\_t

Type of an iterator over this dictionary.

##### name\_itref\_t [only for associative array]

Type of one item referenced in the dictionary for associative array.
It is a structure composed of the key (field 'key') and the value (field 'value').
This type is created only for associative arrays (\_DEF2 suffix) and not for sets.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t dict)
* void name\_clear(name\_t dict)
* void name\_init\_set(name\_t dict, const name\_t ref)
* void name\_set(name\_t dict, const name\_t ref)
* void name\_init\_move(name\_t dict, name\_t ref)
* void name\_move(name\_t dict, name\_t ref)
* void name\_reset(name\_t dict)
* size\_t name\_size(const name\_t dict)
* bool name\empty\_p(const name\_t dict)
* value\_type \*name\_get(const name\_t dict, const key\_type key)
* value\_type\_t * name\_get\_emplace\[suffix\](name\_t container, args...)
* value\_type *name\_safe\_get(name\_t dict, const key\_type key)
* void name\_set\_at(name\_t dict, const key\_type key, const value\_type value)   [for associative array]
* void name\_push(name\_t dict, const key\_type key)       [for dictionary set]
* bool name\_erase(name\_t dict, const key\_type key)
* void name\_it(name\_it\_t it, name\_t dict)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* void name\_next(name\_it\_t it)
* name\_itref\_t *name\_ref(name\_it\_t it)  [for associative array]
* key\_type *name\_ref(name\_it\_t it)       [for dictionary set]
* const name\_itref\_t *name\_cref(name\_it\_t it)  [for associative array]
* const key\_type *name\_cref(name\_it\_t it)       [for dictionary set]
* void name\_get\_str(string\_t str, const name\_t dict, bool append)
* bool name\_parse\_str(name\_t dict, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t dict)
* bool name\_in\_str(name\_t dict, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t dict1, const name\_t dict2)
* void name\_emplace[suffix](name\_t container, keyargs...) [for dictionary set]
* void name\_emplace\_key[keysuffix]\_val[valsuffix](name\_t container, keyargs..., valargs...) [for associative array]

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_splice(name\_t dict1, name\_t dict2)

Move all items from 'dict2' into 'dict1'.
If there is the same key between 'dict2' into 'dict1',
then their values are added (as per the ADD method of the value type).
Afterward 'dict2' is reset (i.e. empty).
This method is only defined if the value type defines an ADD method.



### M-TUPLE

A [tuple](https://en.wikipedia.org/wiki/Tuple) is a finite ordered list of elements of different types. 

#### TUPLE\_DEF2(name, (element1, type1[, oplist1]) [, ...])
#### TUPLE\_DEF2\_AS(name,  name\_t, (element1, type1[, oplist1]) [, ...])

TUPLE\_DEF2 defines the tuple 'name\_t' and its associated methods as "static inline" functions.
Each parameter of the macro is expected to be an element of the tuple.
Each element is defined by three parameters within parenthesis:
* the element name (the field name of the structure) 
* the element type (the associated type)
* and the optional element oplist associated to this type (see generic interface for the behavior if it is absent)
'name' and 'element' shall be C identifiers that will be used to identify the container and the fields.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

This is more or less a C structure. The main added value compared to using a C struct
is that it generates also all the basic methods to handle it which is quite handy.

The oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

In general, an optional method of the tuple will only be created 
if all oplists define the needed optional methods for the underlying type.

The \_hash (resp. \_equal\_p and \_cmp) method is an exception.
This method is created only if at least one oplist of the tuple defines the HASH (resp. EQUAL) method.
You can disable the use of a specific field for the hash computation of the tuple
by disabling the HASH operator of such field ( with HASH(0) in its oplist ),
in which case it is coherent to also disable the EQUAL operator too.
Resp., you can disable the use of a field for the equality of the tuple
by disabling the EQUAL operator of such field ( with EQUAL(0) in its oplist )

The comparison of two tuples uses lexicographic order of the fields defining the CMP method.
It is created only if at least one Oplist of the tuple define CMP method.
You can disable the use of a field for the comparison of the tuple
by disabling the CMP operator of such field ( with CMP(0) in its oplist )

TUPLE\_DEF2\_AS is the same as TUPLE\_DEF2
except the name of the type name\_t is provided.

Example:

```C
	#include <stdio.h>
	#include <gmp.h>
	#include "m-string.h"
	#include "m-tuple.h"
	
	#define MPZ_OUT_STR(stream, x) mpz_out_str(stream, 0, x)
	TUPLE_DEF2(pair,
	           (key_field, string_t, STRING_OPLIST),
	           (value_field, mpz_t, M_OPEXTEND(M_CLASSIC_OPLIST(mpz), OUT_STR(MPZ_OUT_STR))))
	
	int main(void) {
	  pair_t p1;
	  pair_init (p1);
	  string_set_str(p1->key_field, "HELLO");
	  mpz_set_str(p1->value_field, "1742", 10);
	  printf("The pair is ");
	  pair_out_str(stdout, p1);
	  printf("\n");
	  pair_clear(p1);
	}
```

#### TUPLE\_OPLIST(name, oplist1[, ...] )

Return the oplist of the tuple defined by calling TUPLE\_DEF2 with the given name & the Oplist.

#### Created types

The following type is automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the defined tuple.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t tuple)
* void name\_init\_set(name\_t tuple, const name\_t ref)
* void name\_set(name\_t tuple, const name\_t ref)
* void name\_init\_move(name\_t tuple, name\_t ref)
* void name\_move(name\_t tuple, name\_t ref)
* void name\_clear(name\_t tuple)
* void name\_reset(name\_t tuple)
* void name\_get\_str(string\_t str, const name\_t tuple, bool append)
* bool name\_parse\_str(name\_t tuple, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t tuple)
* bool name\_in\_str(name\_t tuple, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* size\_t name\_hash(const name\_t tuple)
* int name\_equal\_p(const name\_t tuple1, const name\_t tuple2)
* int name\_cmp(const name\_t tuple1, const name\_t tuple2)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init\_emplace(name\_t tuple, const type1 element1[, ...])

Initialize the tuple 'tuple' (aka constructor) and set it to the value of the constructed tuple ('element1'[, ...]).

##### void name\_emplace(name\_t tuple, const type1 element1[, ...])

Set the tuple 'tuple' to the value of the tuple constructed with ('element1'[,...]).

##### const type1 *name\_cget\_at\_element1(const name\_t tuple)

Return a constant pointer to the element 'element1' of the tuple.
There is as many methods as there are elements.

##### type1 *name\_get\_at\_element1(const name\_t tuple)

Return a pointer to the element 'element1' of the tuple.
There is as many methods as there are elements.

##### void name\_set\_element1(name\_t tuple, type1 element1)

Set the element of the tuple to 'element1'
There is as many methods as there are elements.

##### int name\_cmp\_order(const name\_t tuple1, const name\_t tuple2, const int order[])

Compare 'tuple1' to 'tuple2' using the given order.
'order' is a null terminated array of int that defines the order of comparison:
an order of {1,4,2,0} indicates to compare first the first field,
if it is equal, to compare the fourth and so on. The third field is not
compared. A negative value indicates to revert the comparison.
This method is created only if all Oplist of the tuple define CMP method.

##### int name\_cmp\_element1(const name\_t tuple1, const name\_t tuple2)

Compare 'tuple1' to 'tuple2' using only the element element1 as reference.
This method is created only if the oplist of element1 defines the CMP method.



### M-VARIANT

A [variant](https://en.wikipedia.org/wiki/Variant_type) is a finite exclusive list of elements of different types:
the variant can only be equal to one element at a time.

#### VARIANT\_DEF2(name, (element1, type1[, oplist1]) [, ...])
#### VARIANT\_DEF2\_AS(name,  name\_t, (element1, type1[, oplist1]) [, ...])

VARIANT\_DEF2 defines the variant 'name\_t' and its associated methods as "static inline" functions.
Each parameter of the macro is expected to be an element of the variant.
Each element is defined by three parameters within parenthesis:
* the mandatory element name,
* the mandatory element type
* and the optional element oplist.

If an 'oplist' is given, it shall be the one matching the associated type.
'name' and 'element<n>' shall be C identifiers that will be used to identify the variant.
'name' will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

This is like a C union. The main added value compared to using a union
is that it generates all the basic methods to handle it and it dynamically
identifies which element is stored within.
It is also able to store an 'EMPTY' state for the variant, contrary to an union
(this is the state when default constructing it).

The oplists shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.
In general, an optional method of the variant will only be created
if all oplists define the needed optional methods for the underlying type.

VARIANT\_DEF2\_AS is the same as VARIANT\_DEF2 except the name of the type name\_t
is provided.

name\_parse\_str & name\_in\_str depend also on the INIT operator.

Example:

```C
	#include <stdio.h>
	#include "m-string.h"
	#include "m-variant.h"
	
	VARIANT_DEF2(item,
	             (name, string_t),
	             (age, long))
	
	int main(void) {
	  item_t p1;
	  item_init (p1);
	  item_set_name(p1, STRING_CTE("HELLO"));
	  printf("The variant is ");
	  item_out_str(stdout, p1);
	  printf("\n");
	
	  item_set_age(p1, 43);
	  printf("The variant is now ");
	  item_out_str(stdout, p1);
	  printf("\n");
	  
	  item_clear(p1);
	}
```


#### VARIANT\_OPLIST(name, oplist1[, ...] )

Return the oplist of the variant defined by calling VARIANT\_DEF2 with the given name & the Oplists used to generate it.

#### Created types

The following type is automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the defined variant.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t variant)
* void name\_init\_set(name\_t variant, const name\_t ref)
* void name\_set(name\_t variant, const name\_t ref)
* void name\_init\_move(name\_t variant, name\_t ref)
* void name\_move(name\_t variant, name\_t ref)
* void name\_clear(name\_t variant)
* void name\_reset(name\_t variant)
* bool name\_empty\_p(const name\_t variant)
* size\_t name\_hash(const name\_t variant)
* bool name\_equal\_p(const name\_t variant1, const name\_t variant2)
* void name\_swap(name\_t variant1, name\_t variant2)
* void name\_get\_str(string\_t str, name\_t variant, bool append)
* bool name\_parse\_str(name\_t variant, const char str[], const char **endp)
* void name\_out\_str(FILE *file, name\_t variant)
* bool name\_in\_str(name\_t variant, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init\_elementN(name\_t variant)

Initialize the variant 'variant' to the type of 'element1' [constructor]
using the default constructor of this element.
This method is defined if all methods define an INIT method.

##### void name\_init\_set\_elementN(name\_t variant, const typeN elementN)

Initialize the variant 'variant' and set it to the type and value of 'elementN' [constructor]

##### void name\_move\_elementN(name\_t variant, typeN ref)

Set the variant 'variant' by stealing as many resources from 'ref' as possible.
Afterwards 'ref' is cleared (destructor)
This method is created only if all Oplist of the variant define MOVE method.

##### void name\_set\_elementN(name\_t variant, const typeN elementN)

Set the variant 'variant' to the type and value of 'elementN'.

##### const typeN * name\_cget\_at\_elementN(name\_t variant)

Return a pointer to the 'variant' viewed as of type 'typeN'.
If the variant isn't an object of such type, it returns NULL.

##### typeN * name\_get\_at\_elementN(name\_t variant)

Return a pointer to the 'variant' viewed as of type 'typeN'.
If the variant isn't an object of such type, it returns NULL.

##### bool name\_elementN\_p(const name\_t variant)

Return true if the variant is of the type of 'elementN'.



### M-RBTREE

A binary tree is a tree data structure in which each node has at most two children,
which are referred to as the left child and the right child.
A node without any child is called a leaf. It can be seen as an ordered set.

A R-B Tree is a tree where all elements are also totally ordered, and the worst-case of any operation is in logarithm of the number of elements in the tree.
The current implementation is [RED-BLACK TREE](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
which provides performance guarantee for both insertion and lockup operations.
It has not to be confused with a [B-TREE](https://en.wikipedia.org/wiki/B-tree).

#### RBTREE\_DEF(name, type[, oplist])
#### RBTREE\_DEF\_AS(name,  name\_t, name\_it\_t, type[, oplist])

RBTREE\_DEF defines the binary ordered tree 'name\_t' and its associated methods as "static inline" functions.
'name' shall be a C identifier that will be used to identify the R-B Tree.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The CMP operator is used to perform the total ordering of the elements.

The oplist shall have at least the following operators (INIT, INIT\_SET, SET, CLEAR & CMP),
otherwise it won't generate compilable code.

Some methods may return a modifiable pointer to the found element
(for example, _get). In this case, the user shall not modify the
key order of the element, as there is no reordering of the tree
in this case.

A push method on the tree will put the given 'key' in its right place in the tree
by keeping the tree ordered.
It overwrites the already existing value if the key is already present in the dictionary (contrary to C++).

RBTREE\_DEF\_AS is the same as RBTREE\_DEF2 except the name of the types name\_t, name\_it\_t
are provided by the user.

Example:

        RBTREE_DEF(rbtree_uint, unsigned int)
        void f(unsigned int num) {
                rbtree_uint_t tree;
                rbtree_uint_init(tree);
                for(unsigned int i = 0; i < num; i++)
                        rbtree_uint_push(tree, i);
                rbtree_uint_clear(tree);                              
        }


#### RBTREE\_OPLIST(name [, oplist])

Return the oplist of the Red-Black tree defined by calling RBTREE\_DEF with name & oplist.
If there is no given oplist, the basic oplist for basic C types is used.

#### Created types

The following types are automatically defined by the previous definition macro with 'name' if not provided by the user:

##### name\_t

Type of the Red Black Tree.

##### name\_it\_t

Type of an iterator over this Red Black Tree.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t rbtree)
* void name\_clear(name\_t rbtree)
* void name\_init\_set(name\_t rbtree, const name\_t ref)
* void name\_set(name\_t rbtree, const name\_t ref)
* void name\_init\_move(name\_t rbtree, name\_t ref)
* void name\_move(name\_t rbtree, name\_t ref)
* void name\_reset(name\_t rbtree)
* size\_t name\_size(const name\_t rbtree)
* void name\_push(name\_t rbtree, const type data)
* void name\_emplace\[suffix\](name\_t rbtree, args...)
* type * name\_get(const name\_t rbtree, const type data)
* const type * name\_cget(const name\_t rbtree, const type data)
* void name\_swap(name\_t rbtree1, name\_t rbtree2)
* bool name\_empty\_p(const name\_t rbtree)
* void name\_it(name\_it\_t it, name\_t rbtree)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_last(name\_it\_t it, name\_t rbtree)
* void name\_it\_end(name\_it\_t it, name\_t rbtree)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* void name\_it\_remove(name\_t rbtree, name\_it\_t it)
* void name\_next(name\_it\_t it)
* void name\_previous(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_ref(name\_it\_t it)
* void name\_get\_str(string\_t str, const name\_t rbtree, bool append)
* bool name\_parse\_str(name\_t tree, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t rbtree)
* bool name\_in\_str(name\_t rbtree, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t rbtree1, const name\_t rbtree2)
* size\_t name\_hash(const name\_t rbtree)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_pop(type *dest, name\_t rbtree, const type data)

Pop 'data' from the Red Black Tree 'rbtree'
and save the popped value into 'dest' if the pointer is not null
while keeping the tree balanced.
Do nothing if 'data' is no present in the Red Black Tree.

##### type * name\_min(const name\_t rbtree)
##### const type * name\_cmin(const name\_t rbtree)

Return a pointer to the minimum element of the tree
or NULL if there is no element.

##### type * name\_max(const name\_t rbtree)
##### const type * name\_cmax(const name\_t rbtree)

Return a pointer to the maximum element of the tree
or NULL if there is no element.

##### void name\_it\_from(name\_it\_t it, const name\_t rbtree, const type data)

Set the iterator 'it' to
the lowest element of the tree 'rbtree' greater or equal than 'data'
or an iterator to no element is there is none.

##### bool name\_it\_until\_p(const name\_it\_t it, const type data)

Return true if 'it' references an element that is greater or equal than 'data'
or if it references no longer a valid element, false otherwise.

##### bool name\_it\_while\_p(const name\_it\_t it, const type data)

Return true if 'it' references an element that is lower or equal than 'data'.
Otherwise (or if it references no longer a valid element) it returns false.



### M-BPTREE

A [B+TREE](https://en.wikipedia.org/wiki/B%2B_tree) is a variant of
[BTREE](https://en.wikipedia.org/wiki/B-tree) that itself is
a generalization of [Binary Tree](https://en.wikipedia.org/wiki/Binary_tree).

A B+TREE is an N-ary tree with a variable but often large number of children per node.
It is mostly used for handling slow media by file system and database.

It provides a fully sorted container enabling fast access to individual item
or range of items, and as such is concurrent to Red-Black Tree.
On modern architecture, a B+TREE is typically faster than a red-black tree due to being
more cache friendly (The RAM itself can be considered as a slow media nowadays!)

When defining a B+TREE it is necessary to give the type of the item within, but also
the maximum number of child per node (N). The best maximum number of child per node
depends on the type itself (its size, its compare cost) and the cache of the
processor. 

#### BPTREE\_DEF2(name, N, key\_type, key\_oplist, value\_type, value\_oplist)
#### BPTREE\_DEF2\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, N, key\_type, key\_oplist, value\_type, value\_oplist)

Define the B+TREE tree of rank N 'name\_t' and its associated methods as
"static inline" functions. This B+TREE will be created as an associative
array of the key\_type to the value\_type.

The CMP operator is used to perform the total ordering of the key elements.

N is the number of items per node and shall be greater or equal than 2.

It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplist shall have at least the operators (INIT, INIT\_SET, SET, CLEAR and CMP).

BPTREE\_DEF2\_AS is the same as BPTREE\_DEF2 except the name of
the container type name\_t, 
the iterator type name\_it\_t,
and the iterated object type name\_itref\_t
are provided by the user.

Example:

        BPTREE_DEF2(tree_uint, 4, unsigned int, (), float, ())
        void f(unsigned int num) {
                tree_uint_t tree;
                tree_uint_init(tree);
                for(unsigned int i = 0; i < num; i++)
                        tree_uint_set_at(tree, i, (float) i);
                tree_uint_clear(tree);
        }


#### BPTREE\_OPLIST2(name, key\_oplist, value\_oplist)

Return the oplist of the BPTREE defined by calling BPTREE\_DEF2 with name,
key\_oplist and value\_oplist.


#### BPTREE\_DEF(name, N, key\_type[, key\_oplist])
#### BPTREE\_DEF\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, N, key\_type, key\_oplist)

Define the B+TREE tree of rank N 'name\_t' and its associated methods as
"static inline" functions. This B+TREE will be created as an ordered set
of key\_type.

The CMP operator is used to perform the total ordering of the key elements.

N is the number of items per node and shall be greater or equal than 2.

It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplist shall have at least the operators (INIT, INIT\_SET, SET, CLEAR and CMP).

BPTREE\_DEF\_AS is the same as BPTREE\_DEF except the name of
the container type name\_t, the iterator type name\_it\_t and the iterated object type name\_itref\_t
are provided by the user.

Example:

        BPTREE_DEF(tree_uint, unsigned int)
        void f(unsigned int num) {
                tree_uint_t tree;
                tree_uint_init(tree);
                for(unsigned int i = 0; i < num; i++)
                        tree_uint_push(tree, i);
                tree_uint_clear(tree);
        }


#### BPTREE\_OPLIST(name[, key\_oplist])

Return the oplist of the BPTREE defined by calling BPTREE\_DEF with name, key\_oplist.
If there is no given oplist, the basic oplist for basic C types is used.


#### BPTREE\_MULTI\_DEF2(name, N, key\_type, key\_oplist, value\_type, value\_oplist)
#### BPTREE\_MULTI\_DEF2\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, N, key\_type, key\_oplist, value\_type, value\_oplist)

Define the B+TREE tree of rank N 'name\_t' and its associated methods as
"static inline" functions. This B+TREE will be created as an associative
array of the 'key\_type' to the 'value\_type' and allows multiple instance of
the same key in the tree (aka it is a multi-map: re-adding the same key in
the tree will add a new instance of the key in the tree rather than update
the value associated to the key).

See BPTREE\_DEF2 for additional details and example.

BPTREE\_MULTI\_DEF2\_AS is the same as BPTREE\_MULTI\_DEF2 except the name of the types
name\_t, name\_it\_t, name\_itref\_t are provided by the user.


#### BPTREE\_MULTI\_DEF(name, N, key\_type[, key\_oplist])
#### BPTREE\_MULTI\_DEF\_AS(name,  name\_t, name\_it\_t, name\_itref\_t, N, key\_type, key\_oplist)

Define the B+TREE tree of rank N 'name\_t' and its associated methods as
"static inline" functions. This B+TREE will be created as an ordered set
of key\_type and allows multiple instance of
the same key in the tree (aka it is a multi set: re-adding the same key in
the tree will add a new instance of the key in the tree rather than update
the key value).

See BPTREE\_DEF for additional details and example.

BPTREE\_MULTI\_DEF\_AS is the same as BPTREE\_MULTI\_DEF except the name of the types
name\_t, name\_it\_t, name\_itref\_t are provided by the user.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the B+Tree of 'type'.

##### name\_it\_t

Type of an iterator over this B+Tree.

##### name\_itref\_t

Type of one item referenced in the B+Tree. It is either:

* a structure composed of a pointer to the key (field key\_ptr) and a pointer to the value (field value\_ptr) if the B+Tree is an associative array,
* or the basic type of the container if the B+Tree is a set.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t tree)
* void name\_clear(name\_t tree)
* void name\_init\_set(name\_t tree, const name\_t ref)
* void name\_set(name\_t tree, const name\_t ref)
* void name\_init\_move(name\_t tree, name\_t ref)
* void name\_move(name\_t tree, name\_t ref)
* void name\_reset(name\_t tree)
* size\_t name\_size(const name\_t tree)
* void name\_push(name\_t tree, const key\_type data) [for set definition only]
* void name\_set\_at(name\_t tree, const key\_type data, const value\_type val) [for associative array definition only]
* bool name\_erase(name\_t tree, const key\_type data)
* value\_type * name\_get(const name\_t tree, const key\_type data)
* const value\_type * name\_cget(const name\_t tree, const key\_type data)
* value\_type * name\_safe\_get(name\_t tree, const key\_type data)
* void name\_swap(name\_t tree1, name\_t tree2)
* bool name\_empty\_p(const name\_t tree)
* void name\_it(name\_it\_t it, name\_t tree)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_end(name\_it\_t it, name\_t tree)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it1)
* void name\_next(name\_it\_t it)
* name\_itref\_t *name\_ref(name\_it\_t it)
* const name\_itref\_t *name\_cref(name\_it\_t it)
* void name\_get\_str(string\_t str, const name\_t tree, bool append)
* bool name\_parse\_str(name\_t tree, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t tree)
* bool name\_in\_str(name\_t tree, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(const name\_t tree1, const name\_t tree2)
* size\_t name\_hash(const name\_t tree)
* void name\_emplace[suffix](name\_t container, keyargs...) [for dictionary set]
* void name\_emplace\_key[keysuffix]\_val[valsuffix](name\_t container, keyargs..., valargs...) [for associative array]

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_pop(value\_type *dest, name\_t tree, const key\_type data)

Pop 'data' from the B+Tree 'tree'
and save the popped value into 'dest' if the pointer is not null
while keeping the tree balanced.
Do nothing if 'data' is no present in the B+Tree.

##### value\_type * name\_min(const name\_t tree)
##### const value\_type * name\_cmin(const name\_t tree)

Return a pointer to the minimum element of the tree
or NULL if there is no element in the B+Tree.

##### value\_type * name\_max(const name\_t tree)
##### const value\_type * name\_cmax(const name\_t tree)

Return a pointer to the maximum element of the tree
or NULL if there is no element in the B+Tree.

##### void name\_it\_from(name\_it\_t it, const name\_t tree, const type data)

Set the iterator 'it' to the greatest element of 'tree'
lower of equal than 'data' or the first element is there is none.

##### bool name\_it\_until\_p(const name\_it\_t it, const type data)

Return true if 'it' references an element that is greater or equal than 'data'.

##### bool name\_it\_while\_p(const name\_it\_t it, const type data)

Return true if 'it' references an element that is lower or equal than 'data'.



### M-TREE

A [tree](https://en.wikipedia.org/wiki/Tree_(data_structure)) is an abstract data type 
to represent the hierarchic nature of a structure with a set of connected nodes.
Each node in the tree can be connected to many children,
but must be connected to exactly one parent,
except for the root node, which has no parent.

#### TREE\_DEF(name, type [, oplist])
#### TREE\_DEF\_AS(name,  name\_t, name\_it\_t, type [, oplist])

Define the tree 'name\_t' and its associated methods as "static inline" functions.
The tree will be composed of object of type 'type', connected each other.

'name' shall be a C identifier that will be used to identify the tree.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

Any insert (resp. remove) in the tree shall specific the insertion point
(resp. deleting point). To construct a tree, you start by creating the
root node (using the \_set\_root method) and then insert new nodes from there.
Each insertion of node in the tree will return an iterator of the inserted
node. This can be used to construct quickly a full tree.

The oplist shall have at least the following operators (INIT\_SET, & CLEAR),
otherwise it won't generate compilable code.

The tree handles its own pool of nodes for the nodes.
It is called the capacity of the tree. 
This pool of nodes will increase when needed by default.
However, in case of capacity increased, all the nodes of the tree may move in memory to accommodate the new need.
You may also request to reserve more capacity to avoid moving the items, and disable this auto-expand feature (in which a MEMORY\_FAILURE is raised).

There are several way to iterate over this container:

* Scan all nodes: first the parent then the children (pre-order walk).
* Scan all nodes: first the children then the parent (post-order walk).
* Scan the nodes of a sub-tree: first the parent then the children (pre-order walk of a sub-tree).
* Scan the nodes of a sub-tree: first the children then the parent (post-order walk of a sub-tree).

On insertion, all iterators remain valid.
Except if it says otherwise, all functions accepting iterators expect a valid iterator (i.e. it references an existing node).

TRREE\_DEF\_AS is the same as TREE\_DEF except the name of the types name\_t, name\_it\_t are provided.

#### TREE\_OPLIST(name, [, oplist])

Define the oplist of the generic tree defined with 'name' and potentially 'oplist'.
If there is no given oplist, the basic oplist for basic C types is used.

#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the generic tree of 'type'.

##### name\_it\_t

Type of an iterator over this generic tree.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t tree)
* void name\_init\_set(name\_t tree, const name\_t ref)
* void name\_init\_move(name\_t tree, name\_t ref)
* void name\_set(name\_t tree, const name\_t ref)
* void name\_move(name\_t tree, name\_t ref)
* void name\_clear(name\_t tree)
* void name\_reset(name\_t tree)
* size\_t name\_size(const name\_t tree)
* size\_t name_capacity(const name\_t tree)
* void name\_reserve(name\_t tree, size\_t capacity)
* bool name\_empty\_p(const name\_t tree)
* void name\_swap(name\_t tree1, name\_t tree2)
* bool name\_equal\_p(const name\_t tree1, const name\_t tree2)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* const type *name\_cref(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* bool name\_remove(it\_t it)
* void name\_get\_str(string\_t str, const name\_t tree, bool append)
* bool name\_parse\_str(name\_t tree, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t tree)
* bool name\_in\_str(name\_t tree, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_lock(name\_t tree, bool lock)

Disable the auto-resize feature of the tree (if lock is true), or enable it otherwise.
By default, the feature is enabled.
Locking a tree shall be done after reserving the maximum
number of nodes that can be added by your tree,
so that the returned pointers to the internal types
won't move on inserting a new node.

##### name\_it\_t name\_set\_root(name\_t tree, const type value)

Set the tree to a single root node and set this node to 'value'.

##### name\_it\_t name\_emplace\_root\[suffix\](name\_t tree, args...)

Set the tree to a single root node and set this node to the value
initialized with the given args.
The provided arguments shall therefore match one of the constructor provided
by the EMPLACE\_TYPE operator.
See emplace chapter for more details.

##### it\_t name\_insert\_up\_raw(it\_t ref)
##### it\_t name\_insert\_left\_raw(it\_t ref)
##### it\_t name\_insert\_right\_raw(it\_t ref)
##### it\_t name\_insert\_down\_raw(it\_t ref)
##### it\_t name\_insert\_child\_raw(it\_t ref)

Insert a node up (resp. left, right, down and down) the given referenced iterator without initializing it.
It returns an iterator to the inserted node with **non-initialized** data.
The first thing to do after calling this function shall be to initialize the data
using the proper constructor of the object of type 'type' (the pointer can be get through name\_ref)
This enables using more specialized constructor than the generic copy one.
The user should use other the non \_raw function if possible rather than this one
as it is low level and error prone.

name\_insert\_down\_raw will move all children of the referenced node as children of the inserted children,
whereas name\_insert\_down\_raw will insert the node as the new first child of the referenced node.

##### it\_t name\_insert\_up(it\_t ref, const type value)
##### it\_t name\_insert\_left(it\_t ref, const type value)
##### it\_t name\_insert\_right(it\_t ref, const type value)
##### it\_t name\_insert\_down(it\_t ref, const type value)
##### it\_t name\_insert\_child(it\_t ref, const type value)

Insert a node up (resp. left, right, down and down) the given referenced iterator and initialize it with a copy of 'value'.
It returns an iterator to the inserted node.

name\_insert\_down will move all children of the referenced node as children of the inserted children,
whereas name\_insert\_down will insert the node as the new first child of the referenced node.

##### it\_t name\_move\_up(it\_t ref, type *value)
##### it\_t name\_move\_left(it\_t ref, type *value)
##### it\_t name\_move\_right(it\_t ref, type *value)
##### it\_t name\_move\_down(it\_t ref, type *value)
##### it\_t name\_move\_child(it\_t ref, type *value)

Insert a node up (resp. left, right, down and down) the given referenced iterator and initialize it with 'value' by stealing as much resource from 'value' as possible (and destroy '\*value')
It returns an iterator to the inserted node.

name\_move\_down will move all children of the referenced node as children of the inserted children,
whereas name\_move\_down will insert the node as the new first child of the referenced node.

##### it\_t name\_emplace\_up\[suffix\](it\_t ref, args...)
##### it\_t name\_emplace\_left\[suffix\](it\_t ref, args...)
##### it\_t name\_emplace\_right\[suffix\](it\_t ref, args...)
##### it\_t name\_emplace\_down\[suffix\](it\_t ref, args...)
##### it\_t name\_emplace\_child\[suffix\](it\_t ref, args...)

Insert a node up (resp. left, right, down and down) the given referenced iterator
and initialize this node to the value initialized with the given args.
The provided arguments shall therefore match one of the constructor provided
by the EMPLACE\_TYPE operator.
See emplace chapter for more details.
It returns an iterator to the inserted node.

name\_emplace\_down will move all children of the referenced node as children of the inserted children,
whereas name\_emplace\_down will insert the node as the new first child of the referenced node.

##### type *name\_up\_ref(name\_it\_t it)
##### type *name\_down\_ref(name\_it\_t it)
##### type *name\_left\_ref(name\_it\_t it)
##### type *name\_right\_ref(name\_it\_t it)

Return a pointer to the type of the node which is 
* up the given iterator,
* down the given iterator (i.e. the first child of the node)
* left the given iterator,
* right the given iterator,
respectively.
It returns NULL if there is no such node.

##### bool  name\_it\_up(it\_t *it)
##### bool  name\_it\_down(it\_t *it)
##### bool  name\_it\_left(it\_t *it)
##### bool  name\_it\_right(it\_t *it)

Update the iterator to point to the node which is up (resp. down, left and right) the given iterator.
Return true in case of success, false otherwise (as such node doesn't exist, the iterator remains unchanged).

##### bool  name\_root\_p(const it\_t it)

Return true if 'it' references the root node, false otherwise.

##### bool  name\_node\_p(const it\_t it)

Return true if 'it' references a non-leaf node, false otherwise.

##### bool  name\_leaf\_p(const it\_t it)

Return true if 'it' references a leaf node, false otherwise.

##### int32_t name\_degree(const it\_t it)

Return the degree of the referenced node (aka the number of children).
A leaf node has a degree of 0.
This function is linear in the number of children of the node.

#####  int32_t name\_depth(const it\_t it)

Return the depth of the referenced node (aka the number of nodes until reaching the root node).
The root node has a depth of 0.
This function is linear in the depth of the node.

##### type *name\_unlink(it\_t it)

Unlink the referenced node from the tree,
so that the node is removed from the tree.
All children of the removed node become children of the parent node.
If the removed node is the root node, than the root node shall have only one child.

Return a reference to the internal type and give back ownership of the type:
you shall destroy the type (using CLEAR or MOVE methods) before calling any other tree functions
(as the memory area used by the node may be removed on inserting a new node)

You should use the remove service instead as it has the same semantics but it is safer as it perform the clear of the data.

##### void name\_prune(name\_it\_t it)

Remove the referenced node including all its children.
See name\_remove for more details.

##### name\_it\_t name\_it\_end(name\_t tree)

Return an iterator that references no valid node.

##### void name\_it\_set(name\_it\_t *it, const name\_it\_t ref)

Set the iterator '*it' to 'ref'.
NOTE: you can use the '=' affectation operator of the C language to copy tree iterators too.

##### name\_it\_t name\_it(name\_t tree)

Return an iterator of the tree that will iterator on the tree in global pre-order walk
(the root).

##### void name\_next(name\_it\_t *it)

Update the iterator of the tree so that it references the next node in a global pre-order walk,
or set it to invalid if the walk is finished.

##### name\_it\_t name\_it\_post(name\_t tree)

Return an iterator of the tree that will iterator on the tree in global post-order walk

##### void name\_next\_post(name\_it\_t *it)

Update the iterator of the tree so that it references the next node in a global post-order walk,
or set it to invalid if the walk is finished.

##### name\_it\_t name\_it\_subpre(name\_t tree, const name\_it\_t ref)

Return an iterator of the tree that will iterator on the tree in pre-order walk of the child nodes of the referenced one.

##### void name\_next\_subpre(name\_it\_t it, const name\_it\_t ref)

Update the iterator of the tree so that it references the next node in a local pre-order walk of the child nodes of the referenced one,
or set it to invalid if the walk is finished (the sub tree is fully scanned).

The referenced iterator shall be the same as the one used to create the updated iterator (with name\_it\_subpre).

##### name\_it\_t name\_it\_subpost(name\_t tree, const name\_it\_t ref)

Return an iterator of the tree that will iterator on the tree in post-order walk of the child nodes of the referenced one.

##### void name\_next\_subpost(name\_it\_t it, const name\_it\_t ref)

Update the iterator of the tree so that it references the next node in a local post-order walk of the child nodes of the referenced one,
or set it to invalid if the walk is finished (the sub tree is fully scanned).

The referenced iterator shall be the same as the one used to create the updated iterator (with name\_it\_subpost).

##### void name\_lca(name\_it\_t it1, name\_it\_t it2)

Compute the Lowest Common Ancestor of the two iterators.
Both iterators shall belong to the same tree.

##### void name\_swap\_at(name\_it\_t it1, name\_it\_t it2, bool swapChild)

Swap the node referenced by it1 and the node referenced by it2 in the tree.
If swapChild is true, the children nodes perform the swap with their parent.
Otherwise, only the referenced nodes are swapped.

##### void name\_sort\_child(name\_it\_t it1)

Sort the child of the node referenced by it1 in the order of the type.
This method is constructed if the basic type exports the CMP type.



### M-PRIOQUEUE

A [priority queue](https://en.wikipedia.org/wiki/Priority_queue) is a queue 
where each element has a "priority" associated with it:
an element with high priority is served before an element with low priority. 
It is currently implemented as a [heap](https://en.wikipedia.org/wiki/Heap_(data_structure)).


#### PRIOQUEUE\_DEF(name, type [, oplist])
#### PRIOQUEUE\_DEF\_AS(name,  name\_t, name\_it\_t, type [, oplist])

Define the priority queue 'name\_t' and its associated methods
as "static inline" functions.
The queue will be composed of object of type 'type'.

'name' shall be a C identifier that will be used to identify the queue.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The CMP operator is used to sort the queue so that the highest priority is the minimum.
The EQUAL operator is used to identify an item on update or remove operations.
It is uncorrelated with the CMP operator from the point of view of this container.
(i.e. EQUAL() == TRUE is not equivalent to CMP() == 0 for this container)

This queue will push the object at the right place in the queue in function
of their priority. A pop from this queue will always return the minimum of all objects
within the queue (contrary to C++ which returns the maximum), and the front method
will reference this object.

The oplist shall have at least the following operators (INIT, INIT\_SET, SET, CLEAR, CMP),
otherwise it won't generate compilable code.

The equal\_p, update & erase methods have a complexity of O(n) due to the linear
search of the data and are only created if the EQUAL method is defined.

Iteration over this container won't iterate from minimum to maximum but in an implementation
define way that ensures that all items are accessed.

PRIOQUEUE\_DEF\_AS is the same as PRIOQUEUE\_DEF except the name of the types name\_t, name\_it\_t are provided.

#### PRIOQUEUE\_OPLIST(name, [, oplist])

Define the oplist of the prioqueue defined with 'name' and potentially 'oplist'.
If there is no given oplist, the basic oplist for basic C types is used.

#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the priority queue of 'type'.

##### name\_it\_t

Type of an iterator over this priority queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t queue)
* void name\_clear(name\_t queue)
* void name\_init\_set(name\_t queue, const name\_t ref)
* void name\_set(name\_t queue, const name\_t ref)
* void name\_init\_move(name\_t queue, name\_t ref)
* void name\_move(name\_t queue, name\_t ref)
* void name\_reset(name\_t queue)
* size\_t name\_size(const name\_t queue)
* bool name\_empty\_p(const name\_t queue)
* void name\_swap(name\_t queue1, name\_t queue2)
* void name\_push(name\_t queue, const type x)
* const type *name\_front(name\_t queue)
* void name\_pop(type *dest, name\_t queue)
* bool name\_equal\_p(const name\_t queue1, const name\_t queue2)
* bool name\_erase(name\_t queue, const type\_t val)
* void name\_it(name\_it\_t it, name\_t queue)
* void name\_it\_last(name\_it\_t it, name\_t queue)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_end(name\_it\_t it, name\_t queue)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* void name\_previous(name\_it\_t it)
* const type *name\_cref(name\_it\_t it)
* void name\_get\_str(string\_t str, const name\_t queue, bool append)
* bool name\_parse\_str(name\_t queue, const char str[], const char **endp)
* void name\_out\_str(FILE *file, const name\_t queue)
* bool name\_in\_str(name\_t queue, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* void name\_emplace\[suffix\](name\_t queue, args...)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_update(name\_t queue, const type\_t old\_val, const type\_t new\_val)

Change the priority of the data of the priority equals to 'old\_val' (in EQUAL sense)
to 'new\_val' (increase or decrease priority).
This method has a complexity of O(n) (due to to linear search of the data).
This method is defined only if the EQUAL method is defined.



### M-BUFFER

This header implements different kind of fixed circular buffer.

A [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) 
(or ring buffer or circular queue) is a data structure using a single, bounded buffer
as if its head was connected to its tail.

#### BUFFER\_DEF(name, type, size, policy[, oplist])
#### BUFFER\_DEF\_AS(name,  name\_t, type, size, policy[, oplist])

Define the buffer 'name\_t' and its associated methods as "static inline" functions.
A buffer is a fixed circular queue implementing a queue (or stack) interface.
It can be used to transfer message from multiple producer threads to multiple consumer threads.
This is done internally using a mutex and conditional waits
(if it is built with the BUFFER\_THREAD\_SAFE option -- default)

'name' shall be a C identifier that will be used to identify the buffer.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The size parameter defined the fixed size of the queue.
It can be 0. In this case, the fixed size is defined at initialization time only
and the needed objects to handle the buffer are allocated at initialization time too.
Otherwise the needed objects are embedded within the structure, preventing
any other allocations.

The size of the buffer shall be lower or equal than the maximum of the type 'int'.

Multiple additional policy can be applied to the buffer by performing a logical or of the following properties:

* BUFFER\_QUEUE : define a FIFO queue (default),
* BUFFER\_STACK : define a stack (exclusive with BUFFER\_QUEUE),

* BUFFER\_THREAD\_SAFE : define thread safe functions (default),
* BUFFER\_THREAD\_UNSAFE : define thread unsafe functions (exclusive with BUFFER\_THREAD\_SAFE),

* BUFFER\_PUSH\_INIT\_POP\_MOVE : change the behavior of PUSH to push a new initialized object, and POP as moving this new object into the new emplacement (this is mostly used for performance reasons or to handle properly a shared\_ptr semantic). In practice, it works as if POP performs the initialization of the object. 
* BUFFER\_PUSH\_OVERWRITE : PUSH overwrites the last entry if the queue is full instead of blocking,
* BUFFER\_DEFERRED\_POP : do not consider the object to be fully popped from the buffer by calling the pop method until the call to pop\_deferred ; this enables to handle object that are in-progress of being consumed by the thread.

This container is designed to be used for synchronization inter-threads of data
(and the buffer variable should be a global shared one). A function tagged "thread safe"
is thread safe only if the container has been generated with the THREAD_SAFE option.

The oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

The push & pop methods operate like push\_blocking & pop\_blocking when the blocking parameter
is true.

BUFFER\_DEF\_AS is the same as BUFFER\_DEF except the name of the type name\_t is provided.

Example:

        BUFFER_DEF(buffer_uint, unsigned int, 10, BUFFER_QUEUE|BUFFER_BLOCKING)

        buffer_uint_t g_buff;

        void f(unsigned int i) {
                buffer_uint_init(g_buff, 10);
                buffer_uint_push(g_buff, i);
                buffer_uint_pop(&i, g_buff);
                buffer_uint_clear(g_buff);
        }


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the buffer.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_clear(buffer\_t buffer) [Not thread safe]
* void name\_reset(buffer\_t buffer) [Thread safe]
* bool name\_empty\_p(const buffer\_t buffer) [Thread safe]
* size\_t name\_size(const buffer\_t buffer) [Thread safe]
* size\_t name\_capacity(const buffer\_t buffer) [Thread safe]
* bool name\_push(buffer\_t buffer, const type data)
* bool name\_pop(type *data, buffer\_t buffer)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init(buffer\_t buffer, size\_t size)

Initialize the buffer 'buffer' for 'size' elements.
The 'size' argument shall be the same as the one used to create the buffer
or the one used to create the buffer was '0'.
The size of the buffer shall be lower or equal than UINT\_MAX.
This function is not thread safe.

##### bool name\_full\_p(const buffer\_t buffer)

Return true if the buffer is full, false otherwise.
This function is thread safe if the buffer was built thread safe. 

##### size\_t name\_overwrite(const buffer\_t buffer)

If the buffer is built with the BUFFER\_PUSH\_OVERWRITE option,
this function returns the number of elements that have been overwritten
and thus discarded.
If the buffer was not built with the BUFFER\_PUSH\_OVERWRITE option,
it returns 0.

##### bool name\_push\_blocking(buffer\_t buffer, const type data, bool blocking)

Push the object 'data' in the buffer 'buffer',
waiting for an empty room if 'blocking' is true (performing a blocking wait)
Returns true if the data was pushed, false otherwise.
Always return true if the buffer is blocking.
This function is thread safe if the buffer was built thread safe. 

##### bool name\_pop\_blocking(type *data, buffer\_t buffer, bool blocking)

Pop from the buffer 'buffer' into the object '*data',
waiting for a data if 'blocking' is true.

If the buffer is built with the BUFFER\_PUSH\_INIT\_POP\_MOVE option,
the object pointed by 'data' shall be ***uninitialized***
as the pop function will perform a quick initialization of the object
(using an INIT\_MOVE operator)
, otherwise it shall be an initialized object (the pop function will 
perform a SET operator).

If the buffer is built with the BUFFER\_DEFERRED\_POP option,
the object is still considered being present in the queue until
a call to name\_pop\_release.

Returns true if a data was popped, false otherwise.
Always return true if the buffer is blocking.
This function is thread safe if the buffer was built thread safe. 

##### bool name\_pop\_release(buffer\_t buffer)

If the buffer is built with the BUFFER\_DEFERRED\_POP option,
the object being popped is considered fully release (freeing a
space in the queue).
Otherwise it does nothing.


#### QUEUE\_MPMC\_DEF(name, type, policy[, oplist])
#### QUEUE\_MPMC\_DEF\_AS(name, name\_t, type, policy[, oplist])

Define the MPMC queue 'name\_t' and its associated methods as "static inline" functions.
A MPMC queue is a fixed circular queue implementing a queue (or stack) interface.
It can be used to transfer message from Multiple Producer threads to Multiple Consumer threads.
This queue is not strictly lock free but [has](https://stackoverflow.com/questions/45907210/lock-free-progress-guarantees)
a lot of the properties of such algorithms.

The size is specified only at run-time and shall be a power of 2.

'name' shall be a C identifier that will be used to identify the queue.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

An additional policy can be applied to the buffer by performing a logical or of the following properties:

* BUFFER\_QUEUE : define a FIFO queue (default),
* BUFFER\_PUSH\_INIT\_POP\_MOVE : change the behavior of PUSH to push a new initialized object, and POP as moving this new object into the new emplacement (this is mostly used for performance reasons or to handle properly a shared\_ptr semantic). In practice, it works as if POP performs the initialization of the object. 

This container is designed to be used for easy synchronization inter-threads
in a context of very fast communication (the variable should be a global shared one).
There should not have much more threads using this queue than they are available hardware cores 
due to the only partial protection on Context-switch Immunity of this structure
(This can happen only if you abuse **massively** the number of threads vs the number of cores).

The oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

The size method is thread safe but may return a size greater than
the size of the queue in some race condition.

QUEUE\_MPMC\_DEF\_AS is the same as QUEUE\_MPMC\_DEF except the name of the type name\_t
is provided.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the circular queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_clear(buffer\_t buffer)
* bool name\_empty\_p(const buffer\_t buffer) [Thread safe]
* size\_t name\_size(const buffer\_t buffer) [Thread safe]
* size\_t name\_capacity(const buffer\_t buffer) [Thread safe]

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init(buffer\_t buffer, size\_t size)

Initialize the buffer 'buffer' with 'size' elements.
The 'size' argument shall be a power of two greater than 0, and less than UINT\_MAX.
This function is not thread safe.

##### bool name\_full\_p(const buffer\_t buffer)

Return true if the buffer is full, false otherwise.
This function is thread safe.

##### bool name\_push(buffer\_t buffer, const type data)

Push the object 'data' in the buffer 'buffer' if possible.
Returns true if the data was pushed, false otherwise
(buffer full or unlikely data race).
This function is thread safe. 

##### bool name\_pop(type *data, buffer\_t buffer)

Pop from the buffer 'buffer' into the object '*data' if possible.

If the buffer is built with the BUFFER\_PUSH\_INIT\_POP\_MOVE option,
the object pointed by 'data' shall be ***uninitialized***
as the pop function will perform a quick initialization of the object
(using an INIT\_MOVE operator)
, otherwise it shall be an initialized object (the pop function will 
perform a SET operator).

Returns true if a data was popped, false otherwise (buffer empty or unlikely data race).
This function is thread safe. 



#### QUEUE\_SPSC\_DEF(name, type, policy[, oplist])
#### QUEUE\_SPSC\_DEF\_AS(name, name\_t, type, policy[, oplist])

Define the SPSC queue 'name\_t' and its associated methods as "static inline" functions.
A SPSC queue is a fixed circular queue implementing a queue (or stack) interface.
It can be used to transfer message from a Single Producer thread to a Single Consumer thread.
This is done internally using lock-free objects.
It is more specialized than QUEUE\_MPMC\_DEF and as such, is faster.

The size is specified only at run-time and shall be a power of 2.

'name' shall be a C identifier that will be used to identify the queue.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

An additional policy can be applied to the buffer by performing a logical or of the following properties:

* BUFFER\_QUEUE : define a FIFO queue (default),
* BUFFER\_PUSH\_INIT\_POP\_MOVE : change the behavior of PUSH to push a new initialized object, and POP as moving this new object into the new emplacement (this is mostly used for performance reasons or to handle properly a shared\_ptr semantic). In practice, it works as if POP performs the initialization of the object. 

This container is designed to be used for easy synchronization inter-threads
in a context of very fast communication (the variable should be a global shared one).

The oplist shall have at least the following operators (INIT, INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

QUEUE\_SPSC\_DEF\_AS is the same as QUEUE\_MPMC\_DEF except the name of the type name\_t
is provided.

#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the circular queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

##### void name\_clear(buffer\_t buffer)
##### bool name\_empty\_p(const buffer\_t buffer) [Thread safe]
##### size\_t name\_size(const buffer\_t buffer) [Thread safe]
##### size\_t name\_capacity(const buffer\_t buffer) [Thread safe]

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init(buffer\_t buffer, size\_t size)

Initialize the buffer 'buffer' with 'size' elements.
The 'size' argument shall be a power of two greater than 0, and less than UINT\_MAX.
This function is not thread safe.

##### bool name\_full\_p(const buffer\_t buffer)

Return true if the buffer is full, false otherwise.
This function is thread safe.

##### bool name\_push(buffer\_t buffer, const type data)

Push the object 'data' in the buffer 'buffer' if possible.
Returns true if the data was pushed, false otherwise (buffer full).
This function is thread safe. 

##### bool name\_push\_move(buffer\_t buffer, type *data)

Push & move the object '*data' in the buffer 'buffer' if possible.
Returns true if the data was pushed, false otherwise (buffer full).
Afterwards '*data' is cleared (destructor) if true is returned.
This function is thread safe. 

##### bool name\_push\_force(buffer\_t buffer, const type data)

Push the object 'data' in the buffer 'buffer'
discarding the oldest data if needed.
This function is thread safe. 

##### bool name\_push\_bulk(buffer\_t buffer, unsigned n, const type data[])

Push as much objects from the array 'data' in the buffer 'buffer' as possible,
starting from the object at index 0 to the object at index 'n-1'.
Returns the number of objects effectively pushed (it depends on the free size of the queue)
This function is thread safe. 

##### bool name\_pop(type *data, buffer\_t buffer)

Pop from the buffer 'buffer' into the object '*data' if possible.

If the buffer is built with the BUFFER\_PUSH\_INIT\_POP\_MOVE option,
the object pointed by 'data' shall be ***uninitialized***
as the pop function will perform a quick initialization of the object
(using an INIT\_MOVE operator)
, otherwise it shall be an initialized object (the pop function will 
perform a SET operator).

Returns true if a data was popped, false otherwise (buffer empty or unlikely data race).
This function is thread safe. 

##### unsigned name\_pop\_bulk(unsigned n, type tab[n], buffer\_t buffer)

Pop from the buffer 'buffer' as many objects as possible to fill in 'tab'
and at most 'n'.

If the buffer is built with the BUFFER\_PUSH\_INIT\_POP\_MOVE option,
the object pointed by 'data' shall be ***uninitialized***
as the pop function will perform a quick initialization of the object
(using an INIT\_MOVE operator)
, otherwise it shall be an initialized object (the pop function will 
perform a SET operator).

It returns the number of objects popped.
This function is thread safe. 



### M-SNAPSHOT

This header is for created snapshots.

A snapshot is a mechanism enabling a reader thread and a writer thread,
 **working at different speeds**, to exchange messages in a fast, reliable and thread safe way
(the message is always passed atomically from a thread point of view) without waiting
for synchronization.
The consumer thread always accesses to the latest published data of the producer thread.

It is implemented in a fast way as the writer directly writes the message in the buffer
that will be passed to the reader (avoiding copy of the buffer) and a simple exchange
of index is sufficient to handle the switch.

This container is designed to be used for easy synchronization inter-threads.

This is linked to [shared atomic register](https://en.wikipedia.org/wiki/Shared_register) in the literature 
and [snapshot](https://en.wikipedia.org/wiki/Shared_snapshot_objects) names vector of such registers
where each element of the vector can be updated separately. They can be classified according to the
number of producers/consumers:

* SPSC (Single Producer, Single Consumer),
* MPSC (Multiple Producer, Single Consumer),
* SPMC (Single Producer, Multiple Consumer),
* MPMC (Multiple Producer, Multiple Consumer),

The provided containers by the library are designed to handle huge
structure efficiently and as such deal with the memory reclamation needed to handle them.
If the data you are sharing are supported by the atomic header (like bool or integer), 
using atomic\_load and atomic\_store is a much more efficient and simple way
to do even in the case of MPMC.


#### SNAPSHOT\_SPSC\_DEF(name, type[, oplist])
#### SNAPSHOT\_SPSC\_DEF\_AS(name, name\_t, type[, oplist])

Define the snapshot 'name ## \_t' (or 'name\_t') and its associated methods as "static inline" functions.
Only a single reader thread and a single writer thread are supported.
It is a SPSC atomic shared register.
In practice, it is implemented using a triple buffer (lock-free).

'name' shall be a C identifier that will be used to identify the snapshot.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

Example:

        SNAPSHOT_DEF(snapshot_uint, unsigned int)
        snapshot_uint_t message;
        void f(unsigned int i) {
                unsigned *p = snapshot_uint_get_write_buffer(message);
                *p = i;
                snapshot_uint_write(message);
        }
        unsigned int g(void) {
                unsigned *p = snapshot_uint_read(message);
                return *p;
        }


SNAPSHOT\_SPSC\_DEF\_AS is the same as SNAPSHOT\_SPSC\_DEF except the name of the type name\_t is provided.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the circular queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details)
but none is thread safe:

* void name\_init(snapshot\_t snapshot)
* void name\_clear(snapshot\_t snapshot)
* void name\_init\_set(snapshot\_t snapshot, const snapshot\_t org)
* void name\_set(snapshot\_t snapshot, const snapshot\_t org)
* void name\_init\_move(snapshot\_t snapshot, snapshot\_t org)
* void name\_move(snapshot\_t snapshot, snapshot\_t org)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### type *name\_write(snapshot\_t snap)

Publish the 'in-progress' data of the snapshot 'snap so that the read
thread can have access to the data.
It returns the pointer to the new 'in-progress' data buffer 
of the snapshot (which is not yet published but will be published 
for the next call of name\_write).
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_read(snapshot\_t snap)

Get access to the last published data of the snapshot 'snap'.
It returns the pointer to the data.
If a publication has been performed since the last call to name\_read
a new pointer to the data is returned. 
Otherwise the previous pointer is returned.
This function is thread-safe and performs atomic operation on the snapshot.

##### bool name\_updated\_p(snapshot\_t snap)

Return true if a new publication is available since the last time it was read.
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_get\_write\_buffer(snapshot\_t snap)

Return a pointer to the active 'in-progress' data of the snapshot 'snap'.
It is the same as the last return from name\_write.
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_get\_read\_buffer(snapshot\_t snap)

Return a pointer to the last already read published data of the snapshot 'snap'.
It is the same as the last return from name\_read.
It doesn't perform any switch of the data that has to be read.
This function is thread-safe and performs atomic operation on the snapshot.


#### SNAPSHOT\_SPMC\_DEF(name, type[, oplist])
#### SNAPSHOT\_SPMC\_DEF\_AS(name, name\_t, type[, oplist])

Define the snapshot 'name ## \_t' (or 'name\_t') and its associated methods as "static inline" functions.
A snapshot is an atomic shared register where only the latest state is
important and accessible: it is like a global variable of type 'type'
but ensuring integrity and coherency of the data across multiple threads.
One single writer and multiple (=N) readers are supported.
In practice, it is implemented using a 'N+2' buffer (lock-free).

'name' shall be a C identifier that will be used to identify the snapshot.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT, INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

SNAPSHOT\_SPMC\_DEF\_AS is the same as SNAPSHOT\_SPMC\_DEF except the name of the type name\_t is provided.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the circular queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_clear(snapshot\_t snapshot)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init(snapshot\_t snapshot, size\_t numReaders)

Initialize the communication snapshot 'snapshot' with 'numReaders' reader threads.
'numReaders' shall be less than 2046.
This function is not thread safe.

##### type *name\_write(snapshot\_t snap)

Publish the 'in-progress' data of the snapshot 'snap so that the read
threads can have access to the data.
It returns the pointer to the new 'in-progress' data buffer 
of the snapshot (which is not yet published but will be published 
for the next call of name\_write).
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_read\_start(snapshot\_t snap)

Get access to the last published data of the snapshot 'snap'.
It returns the pointer to the data.
If a publication has been performed since the last call to name\_read\_start
a new pointer to the data is returned. 
Otherwise the previous pointer is returned.

It marks the data has being reserved by the thread,
so afterwards, using the pointer is safe until the end of the reservation.
This function is thread-safe and performs atomic operation on the snapshot.

For each call to name\_read\_start a matching call to
name\_read\_end shall be performed by the same thread before
any new call to name\_read\_start.

##### type *name\_read\_end(snapshot\_t snap, type *old)

End the reservation of the data 'old'.
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_get\_write\_buffer(snapshot\_t snap)

Return a pointer to the active 'in-progress' data of the snapshot 'snap'.
It is the same as the last return from name\_write.
This function is thread-safe and performs atomic operation on the snapshot.


#### SNAPSHOT\_MPMC\_DEF(name, type[, oplist])
#### SNAPSHOT\_MPMC\_DEF\_AS(name, name\_t, type[, oplist])

Define the snapshot 'name ## \_t' (or 'name\_t') and its associated methods as "static inline" functions.
A snapshot is an atomic shared register where only the latest state is
important and accessible: it is like a global variable of type 'type'
but ensuring integrity and coherency of the data across multiple threads.
Multiple (=M) writers and multiple (=N) readers are supported.
In practice, it is implemented using a 'M+N+2' buffer (lock-free)
by avoiding copying the data.

'name' shall be a C identifier that will be used to identify the snapshot.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (INIT, INIT\_SET, SET and CLEAR),
otherwise it won't generate compilable code.

SNAPSHOT\_MPMC\_DEF\_AS is the same as SNAPSHOT\_MPMC\_DEF except the name of the type name\_t is provided.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the circular queue.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_clear(snapshot\_t snapshot)

#### Specialized methods

##### void name\_init(snapshot\_t snapshot, size\_t numReaders, size\_t numWriters)

Initialize the communication snapshot 'snapshot' with 'numReaders' reader threads
and 'numWriters' writer threads.
The sum of 'numReaders' and 'numWriters' shall be less than 2046.
This function is not thread safe.

##### type *name\_write\_start(snapshot\_t snap)

Return the current 'in-progress' data of the snapshot 'snap
so that the writer thread can update this data.
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_write\_end(snapshot\_t snap, type *data)

Mark the provided 'in-progress' data of the snapshot 'snap
as available for the reader threads: this will be the new seen data.
This function is thread-safe and performs atomic operation on the snapshot.

##### type *name\_read\_start(snapshot\_t snap)

Get access to the last published data of the snapshot 'snap'.
It returns the pointer to the data.
If a publication has been performed since the last call to name\_read\_start
a new pointer to the data is returned. 
Otherwise the previous pointer is returned.

It marks the data has being reserved by the thread,
so afterwards, using the pointer is safe until the end of the reservation.
This function is thread-safe and performs atomic operation on the snapshot.

For each call to name\_read\_start a matching call to
name\_read\_end shall be performed by the same thread before
any new call to name\_read\_start.

##### type *name\_read\_end(snapshot\_t snap, type *old)

End the reservation of the data 'old'.
This function is thread-safe and performs atomic operation on the snapshot.



### M-SHARED

This header is for creating shared pointer.
A [shared pointer](https://en.wikipedia.org/wiki/Smart_pointer)
 is a smart pointer that retains shared ownership of an object.
Several shared pointers may own the same object, sharing ownership of an object. 


#### SHARED\_PTR\_DEF(name, type[, oplist])
#### SHARED\_PTR\_DEF\_AS(name, name\_t, type[, oplist])

Define the shared pointer 'name\_t' and its associated methods as "static inline" functions.
A shared pointer is a mechanism to keep tracks of all registered users of an object
and performs an automatic destruction of the object only when all users release
their need on this object.

'name' shall be a C identifier that will be used to identify the shared pointer.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The tracking of ownership is atomic and the destruction of the object is thread safe.

The object oplist is expected to have at least the following operators (CLEAR to clear the object and DEL to free the allocated memory).

There are designed to work with buffers with policy BUFFER\_PUSH\_INIT\_POP\_MOVE
to send a shared pointer across multiple threads.

SHARED\_PTR\_DEF\_AS is the same as SHARED\_PTR\_DEF except the name of the type name\_t
is provided.

Example:

        SHARED_PTR_DEF(shared_mpz, mpz_t, (CLEAR(mpz_clear)))
        void f(void) {
                shared_mpz_t p;
                mpz_t z;
                mpz_init(z);
                shared_mpz_init2 (p, z);
                buffer_uint_push(g_buff1, p);
                buffer_uint_push(g_buff2, p);
                shared_mpz_clear(p);
        }


#### SHARED\_PTR\_RELAXES\_DEF(name, type[, oplist])
#### SHARED\_PTR\_RELAXES\_DEF\_AS(name, name\_t, type[, oplist])

Theses are the same as SHARED\_PTR\_DEF / SHARED\_PTR\_DEF\_AS
except that they are not thread safe.
See SHARED\_PTR\_DEF for other details.


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the shared pointer.

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init(shared\_t shared)

Initialize the shared pointer 'shared' to represent the NULL pointer
(no object is therefore referenced).

##### void name\_init2(shared\_t shared, type *data)

Initialize the shared pointer 'shared' to reference the object '*data'
and takes ownership of this object.
User code shall not use '*data' (or any pointer to it) anymore
as the shared pointer gets the exclusive ownership of the object.

##### void name\_init\_new(shared\_t shared)

Initialize the shared pointer 'shared' to a new object of type 'type'.
The default constructor of type is used to initialize the object.
This method is only created only if the INIT method is provided.

##### void name\_init\_set(shared\_t shared, const shared\_t src)

Initialize the shared pointer 'shared' to the same object than the one
pointed by 'src' (sharing ownership).
This function is thread safe from 'src' point of view.

##### bool name\_NULL\_p(const shared\_t shared)

Return true if shared doesn't reference any object (i.e. is the NULL pointer).

##### void name\_clear(shared\_t shared)

Clear the shared pointer (destructor):
the shared pointer loses its ownership of the object and
it destroys the shared object if no longer any other shared pointers own it.
This function is thread safe.

##### void name\_reset(shared\_t shared)

'shared' loses ownership of its shared object and destroys it
if no longer any other shared pointers own it.
Then it makes the shared pointer 'shared' references no object (NULL)
(it doesn't reference its object any-longer and loses its ownership of it).
This function is thread safe.

##### void name\_set(shared\_t shared, const shared\_t src)

'shared' loses ownership of its object and destroy it
if no longer any other shared pointers own it.
Then it sets the shared pointer 'shared' to the same object 
than the one pointed by 'src' (sharing ownership).
This function is thread safe.

##### void name\_init\_move(shared\_t shared, shared\_t src)

Move the shared pointer from the initialized 'src' to 'shared'.

##### void name\_move(shared\_t shared, shared\_t src)

'shared' loses ownership of its object and destroy it
if no longer any other shared pointers own it.
Then it moves the shared pointer from the initialized 'src' to 'shared'.

##### void name\_swap(shared\_t shared1, shared\_t shared2)

Swap the references of the objects owned by the shared pointers 'shared1' and 'shared2'.

##### bool name\_equal\_p(const shared\_t shared1, const shared\_t shared2)

Return true if both shared pointers own the same object.

##### const type *name\_cref(const shared\_t shared)

Return a constant pointer to the shared object owned by the shared pointer.
The pointer shall be kept only until another use of shared pointer method.
Keeping the pointer otherwise is undefined behavior.

##### type *name\_ref(const shared\_t shared)

Return a pointer to the shared object pointed by the shared pointer.
The pointer shall be kept only until another use of shared pointer method.
Keeping the pointer otherwise is undefined behavior.

TODO: Document shared resource



### M-I-SHARED

This header is for creating intrusive shared pointer.

#### ISHARED\_PTR\_INTERFACE(name, type)

Extend the definition of the structure of an object of type 'type' by adding the
necessary interface to handle it as a shared pointer named 'name'.
It shall be put within the structure definition of the object (See example).

#### ISHARED\_PTR\_STATIC\_INIT(name, type)

Provide the static initialization value of an object defined with a 
ISHARED\_PTR\_INTERFACE extra fields. It shall be used only for global
variables with the \_init\_once function.

Usage (provided that the interface is used as the first element of the structure):

        struct mystruct variable = { ISHARED_PTR_STATIC_INIT(ishared_double, struct mystruct) };

#### ISHARED\_PTR\_STATIC\_DESIGNATED\_INIT(name, type)

Provide the static initialization value of an object defined with a 
ISHARED\_PTR\_INTERFACE extra fields. It shall be used only for global
variables with the \_init\_once function.

It uses designated initializers to set the right fields.

Usage:

        struct mystruct variable = { ISHARED_PTR_STATIC_DESIGNATED_INIT(ishared_double, struct mystruct) };

#### ISHARED\_PTR\_DEF(name, type[, oplist])
#### ISHARED\_PTR\_DEF\_AS(name, name\_t, type[, oplist])

Define the associated methods to handle the shared pointer named 'name'
as "static inline" functions.
A shared pointer is a mechanism to keep tracks of all 'users' of an object
and performs an automatic destruction of the object whenever all users release
their need on this object.

The destruction of the object is thread safe and occurs when the last user
of the object releases it. The destruction of the object implies:

* calling the CLEAR operator to clear the object,
* calling the DEL operator to release the memory used by the object 
(if the method has not been disabled).

The object oplist is expected to have the following operators (CLEAR and DEL),
otherwise default methods are used. If there is no given oplist, the default
operators are also used. The created methods will use the operators to init, set
and clear the contained object.

There are designed to work with buffers with policy BUFFER\_PUSH\_INIT\_POP\_MOVE
to send a shared pointer across multiple threads.

It is recommended to use the intrusive shared pointer over the standard one if
possible (They are faster & cleaner).

The default is to use heap allocated entities, which are allocated by NEW &
freed by DEL.

It can be used for statically allocated entities. However, in this case,
you shall disable the operator NEW & DEL when expanding the oplist
so that the destruction doesn't try to free the objects, like this:

    (NEW(0), DEL(0))

NEW & DEL operators shall be either both defined, or both disabled.

ISHARED\_PTR\_DEF\_AS is the same as ISHARED\_PTR\_DEF except the name of the type name\_t
is provided.

Example (dynamic):

        typedef struct mystruct_s {
                ISHARED_PTR_INTERFACE(ishared_mystruct, struct mystruct_s);
                char *message;
        } mystruct_t;

        static inline void mystruct_init(mystruct_t *p) { p->message = NULL; }
        static inline void mystruct_clear(mystruct_t *p) { free(p->message); }

        ISHARED_PTR_DEF(ishared_mystruct, mystruct_t, (INIT(mystruct_init), CLEAR(mystruct_clear M_IPTR)))

        void f(void) {
                mystruct_t *p = ishared_mystruct_init_new();
                p->message = strdup ("Hello");
                buffer_mystruct_push(g_buff1, p);
                buffer_mystruct_push(g_buff2, p);
                ishared_mystruct_clear(p);
        }


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

It will define name\_t as a pointer to shared counted object.
This is a synonymous to a pointer to the object.

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### name\_t name\_init(type *object)

Return a shared pointer to 'object' which owns 'object'.
It initializes the private fields of 'object' handling the shared pointer,
returning the same pointer to the object from a value point of view,
but with the shared pointer field initialized.

As a consequence, the shared pointer part of 'object' shall not have been initialized yet.
The other part of 'object' may or may not be initialized before calling this method.

##### name\_t name\_init\_set(name\_t shared)

Return a new shared pointer to the same object than the one pointed by 'shared',
incrementing the ownership of the object.
This function is thread safe.

##### name\_t name\_init\_new(void)

Allocate a new object, initialize it and return an initialized shared pointer to it.
The used allocation function is the NEW operator.

This method is only created only if the INIT & NEW methods are provided and not disabled.

##### name\_t name\_init\_once(type *object)

Initialize the new object 'object' and return an initialized shared pointer to it.
The INIT operator of 'object' is ensured to be called only once, 
even if multiple threads try to initialize it at the same time.
Once the object is fully cleared, the initialization function may occur once again.

object shall be a global variable initialized with the
ISHARED\_PTR\_STATIC\_INIT macro.

This method is only created only if the INIT & NEW methods are provided and not disabled.

##### void name\_clear(name\_t shared)

Clear the shared pointer, releasing ownership of the object
and destroying the shared object if no longer
any other shared pointers own it.
This function is thread safe.

##### void name\_clear\_ptr(name\_t *shared)

Clear the shared pointer '*shared', releasing ownership of the object
and destroying the shared object if no longer
any other shared pointers own it.
This function is thread safe.
Afterwards, '*shared' is set to NULL.

##### void name\_set(name\_t *shared1, name\_t shared2)

Update the shared pointer '*shared1' to point to the same object than
the shared pointer 'shared2'.
Destroy the shared object pointed by '*shared1' if no longer any other shared
pointers own it, set the shared pointer 'shared' to the same object 
than the one pointed by 'src'.
This function is thread safe.



### M-I-LIST

This header is for creating intrusive doubly-linked list.

#### ILIST\_INTERFACE(name, type)

Extend an object by adding the necessary interface to handle it within 
an intrusive doubly-linked list.
This is the intrusive part.
It shall be put within the structure of the object to link, at the top
level of the structure.
See example of ILIST\_DEF.

#### ILIST\_DEF(name, type[, oplist])
#### ILIST\_DEF\_AS(name, name\_t, name\_it\_t, type[, oplist])

Define the intrusive doubly-linked list 
and define the associated methods to handle it as "static inline" functions.

'name' shall be a C identifier that will be used to identify the list.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

The oplist shall have at least the following operators (CLEAR),
otherwise it won't generate compilable code.

An object is expected to be part of only one list of a kind in the entire program at a time.
An intrusive list enables to move from an object to the next object without needing to go through the entire list,
or to remove an object from a list in O(1).
It may, or may not, be better than standard list. It depends on the context.

The given interface won't allocate anything to handle the objects as
all allocations and initialization are let to the user.
However the objects within the list can be automatically be cleared
(by calling the CLEAR method to destruct the object) on list destruction.
Then the memory allocation, performed by the user program, can also be reclaimed
by defining a DEL operator to free the used memory in the object oplist.
If there is no DEL operator, it is up to the user to free the used memory.

The list iterates from back to front.

ILIST\_DEF\_AS is the same as ILIST\_DEF except the name of the types name\_t, name\_it\_t
are provided.

Example:

        typedef struct test_s {
          int n;
          ILIST_INTERFACE (ilist_tname, struct test_s);
        } test_t;

        ILIST_DEF(ilist_tname, test_t)

        void f(void) {
                test_t x1, x2, x3;
                ilist_tname_t list;

                x1.n = 1;
                x2.n = 2;
                x3.n = 3;

                ilist_tname_init(list);
                ilist_tname_push_back (list, &x3);
                ilist_tname_push_front (list, &x1);
                ilist_tname_push_after (&x1, &x2);

                int n = 1;
                for M_EACH(item, list, ILIST_OPLIST(ilist_tname)) {
                        assert (n == item->n);
                        n++;
                }
                ilist_tname_clear(list);
        }



#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

#### name\_t

Type of the list of 'type'.

#### name\_it\_t

Type of an iterator over this list.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t list)
* void name\_clear(name\_t list)
* void name\_reset(name\_t list)
* type *name\_back(const name\_t list)
* type *name\_front(const name\_t list)
* bool name\_empty\_p(const name\_t list)
* void name\_swap(name\_t list1, name\_t list2)
* void name\_it(name\_it\_t it, name\_t list)
* void name\_it\_set(name\_it\_t it, const name\_it\_t ref)
* void name\_it\_last(name\_it\_t it, name\_t list)
* void name\_it\_end(name\_it\_t it, name\_t list)
* bool name\_end\_p(const name\_it\_t it)
* bool name\_last\_p(const name\_it\_t it)
* bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)
* void name\_next(name\_it\_t it)
* void name\_previous(name\_it\_t it)
* type *name\_ref(name\_it\_t it)
* const type *name\_cref(const name\_it\_t it)
* size\_t name\_size(const name\_t list)
* void name\_remove(name\_t list, name\_it\_t it)

#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### void name\_init\_field(type *obj)

Initialize the additional fields of the object '*obj' handling the list.
This function shall be used in the object constructor.

##### void name\_push\_back(name\_t list, type *obj)

Push the object '*obj' itself (and not a copy) at the back of the list 'list'.

##### void name\_push\_front(name\_t list, type *obj)

Push the object '*obj' itself (and not a copy) at the front of the list 'list'.

##### void name\_push\_after(type *position, type *obj)

Push the object '*obj' itself (and not a copy) after the object '*position'.

##### type *name\_pop\_back(name\_t list)

Pop the object from the back of the list 'list'
and return a pointer to the popped object,
given back the ownership of the object to the caller program
(which becomes responsible to calling the destructor of this object).

##### type *name\_pop\_front(name\_t list)

Pop the object from the front of the list 'list'
and return a pointer to the popped object,
given back the ownership of the object to the caller program
(which becomes responsible to calling the destructor of this object).

##### void name\_unlink(type *obj)

Remove the object '*obj' from the list.
It gives back the ownership of the object to the caller program
which becomes responsible to calling the destructor of this object.

##### type *name\_next\_obj(const name\_t list, const type *obj)

Return the object that is after the object '*obj' in the list
or NULL if there is no more object.

##### type *name\_previous\_obj(const name\_t list, const type *obj)

Return the object that is before the object '*obj' in the list
or NULL if there is no more object.

##### void name\_insert(name\_t list, name\_it\_t it, type x)

This method is the same as the generic one,
except it is only created only if the NEW & INIT_SET methods are provided.

##### void name\_splice\_back(name\_t list1, name\_t list2, name\_it\_t it)

Move the element pointed by 'it' from the list 'list2' to the back position of 'list1'.
'it' shall be an iterator of 'list2'.
Afterwards, 'it' points to the next element of 'list2'.

##### void name\_splice(name\_t list1, name\_t list2)

Move all the element of 'list2' into 'list1", moving the last element
of 'list2' after the first element of 'list1'.
Afterwards, 'list2' is emptied.


### M-CONCURRENT

This header is for transforming a standard container (LIST, ARRAY, DICT, DEQUE, ...)
into an equivalent container but compatible with concurrent access by different threads. 
In practice, it puts a mutex lock to access the container.

As such it is quite generic. However it is less efficient than containers
specially tuned for multiple threads.
There is also no iterators.

#### CONCURRENT\_DEF(name, type[, oplist])
#### CONCURRENT\_DEF\_AS(name, name\_t, type[, oplist])

Define the concurrent container 'name' based on container 'type' of oplist 'oplist',
and define the associated methods to handle it as "static inline" functions.

'name' shall be a C identifier that will be used to identify the concurrent container.
It will be used to create all the types (including the iterator)
and functions to handle the container.
This definition shall be done once per name and per compilation unit.

It scans the 'oplist' of the type to create equivalent function,
so the exact generated methods depend on explicitly the exported methods in the oplist:
The init method is only defined if the base container exports the INIT operator,
same for the clear method and the CLEAR operator,
and so on for all created methods.

In the description below,
subtype\_t is the type of the element within the given container 'type' (if it exists),
key\_t is the key type of the element within the given container 'type' (if it exists),
value\_t is the value type of the element within the given container 'type' (if it exists).

CONCURRENT\_DEF\_AS is the same as CONCURRENT\_DEF except the name of the type name\_t
is provided.

Example:

        /* Define a stack container (STACK)*/
        ARRAY_DEF(array1, int)
        CONCURRENT_DEF(parray1, array1_t, ARRAY_OPLIST(array1))

        /* Define a queue container (FIFO) */
        DEQUE_DEF(deque_uint, unsigned int)
        CONCURRENT_DEF(cdeque_uint, deque_uint_t, M_OPEXTEND(DEQUE_OPLIST(deque_uint, M_BASIC_OPLIST), PUSH(deque_uint_push_front)))

        extern parray1_t x1;
        extern cdeque_uint_t x2;

        void f(void) {
             parray1_push (x1, 17);
             cdeque_uint_push (x2, 17);
        }


#### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

##### name\_t

Type of the concurrent container of 'type'.

#### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t concurrent)
* void name\_init\_set(name\_t concurrent, const name\_t src)
* void name\_init\_move(name\_t concurrent, name\_t src)
* void name\_set(name\_t concurrent, const name\_t src)
* void name\_move(name\_t concurrent, name\_t src)
* void name\_reset(name\_t concurrent)
* void name\_clear(name\_t concurrent)
* void name\_swap(name\_t concurrent1, name\_t concurrent2)
* bool name\_empty\_p(const name\_t concurrent)
* void name\_set\_at(name\_t concurrent, key\_t key, value\_t value)
* bool name\_erase(name\_t concurrent, const key\_t key)
* void name\_push(name\_t concurrent, const subtype\_t data)
* void name\_push\_move(name\_t concurrent, subtype\_t *data)
* void name\_pop\_move(subtype\_t *data, name\_t concurrent)
* void name\_get\_str(string\_t str, name\_t concurrent, bool append)
* void name\_out\_str(FILE *file, name\_t concurrent)
* bool name\_parse\_str(name\_t concurrent, const char str[], const char **end)
* bool name\_in\_str(name\_t concurrent, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t container)
* m\_serial\_return\_code\_t name\_in\_str(name\_t container, m\_serial\_read\_t serial)
* bool name\_equal\_p(name\_t concurrent1, name\_t concurrent2)
* size\_t name\_hash(name\_t concurrent)


Returns true in case of success, false otherwise.


#### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

##### bool name\_get\_copy(value\_t *value, name\_t concurrent, key\_t key)

Read the value associated to the key 'key'. 
If it exists, it sets '*value' to it and returns true.
Otherwise it returns false (*value is unchanged).
This method is only defined if the base container exports the GET\_KEY operator.

##### void name\_safe\_get\_copy(value\_t *value, name\_t concurrent, key\_t key)

Read the value associated to the key 'key'. 
If it exists, it sets '*value' to it.
Otherwise it creates a new value (default constructor) and sets '*value' to it.
This method is only defined if the base container exports the SAFE\_GET\_KEY operator.

##### void name\_pop(subtype\_t *data, name\_t concurrent)

Pop data from the container and set it in '*data'.
There shall be at least one data to pop.
Testing with the operator EMPTY\_P before calling this function is not enough 
as there can be some concurrent scenario where another thread pop the last value.
name\_pop\_blocking should be used instead.
This method is only defined if the base container exports the POP operator.

##### bool name\_get\_blocking(value\_t *value, name\_t concurrent, key\_t key, bool blocking)

Read the value associated to the key 'key'. 
If it exists, it sets '*value' to it and returns true.
Otherwise if blocking is true, it waits for the data to be filled. 
After the wait, it sets '*value' to it and returns true.
Otherwise if blocking is false, it returns false.
This method is only defined if the base container exports the GET\_KEY operator.

##### bool name\_pop\_blocking(type\_t *data, name\_t concurrent, bool blocking)

Pop a value from the container and set '*data' with it.
If the container is not empty, it sets '*data' and return true.
Otherwise if blocking is true, it waits for the data to be pushed. 
After the wait, it sets '*data' to it and returns true.
Otherwise if blocking is false, it returns false.
This method is only defined if the base container exports the POP and EMPTY\_P operators.

##### bool name\_pop\_move\_blocking(type\_t *data, name\_t concurrent, bool blocking)

Pop a value from the container and initialize & set '*data' with it.
If the container is not empty, it initializes & sets '*data' and return true.
Otherwise if blocking is true, it waits for the data to be pushed. 
After the wait, it initializes & sets '*data' to it and returns true.
Otherwise if blocking is false, it returns false (*data remains uninitialized!).
This method is only defined if the base container exports the POP\_MOVE and EMPTY\_P operators.



### M-BITSET

This header is for using bitset.

A [bitset](https://en.wikipedia.org/wiki/Bit_array) can be seen 
as a specialized version of an array of bool, where each item takes only 1 bit.
It enables for compact representation of such array.

Example:

        void f(void) {
                bitset_t set;
                bitset_init(set);
                for(int i = 0; i < 100; i ++)
                        bitset_push_back(set, i%2);
                bitset_clear(set);
        }


#### methods, types & constants

The following methods are available:

##### bitset\_t

This type defines a dynamic array of bit and is the primary type of the module.

##### bitset\_it\_t

This type defines an iterator over the bitset.

##### void bitset\_init(bitset\_t array)

Initialize the bitset 'array' (aka constructor) to an empty array.

##### void bitset\_init\_set(bitset\_t array, const bitset\_t ref)

Initialize the bitset 'array' (aka constructor) and set it to the value of 'ref'.

##### void bitset\_set(bitset\_t array, const bitset\_t ref)

Set the bitset 'array' to the value of 'ref'.

##### void bitset\_init\_move(bitset\_t array, bitset\_t ref)

Initialize the bitset 'array' (aka constructor) by stealing as many 
resources from 'ref' as possible. Afterwards 'ref' is cleared.

##### void bitset\_move(bitset\_t array, bitset\_t ref)

Set the bitset 'array' by stealing as many resources from 'ref' as possible.
Afterwards 'ref' is cleared.

##### void bitset\_clear(bitset\_t array)

Clear the bitset 'array (aka destructor).

##### void bitset\_reset(bitset\_t array)

Reset the bitset (the bitset becomes empty but remains initialized).

##### void bitset\_push\_back(bitset\_t array, const bool value)

Push a new element into the back of the bitset 'array' 
with the value 'value'.

##### void bitset\_push\_at(bitset\_t array, size\_t key, const bool value)

Push a new element into the position 'key' of the bitset 'array' 
with the value 'value'.
'key' shall be a valid position of the array: 
from 0 to the size of array (included).

##### void bitset\_pop\_back(bool *data, bitset\_t array)

Pop a element from the back of the bitset 'array' and set *data to this value
if data is not NULL (if data is NULL, the popped data is cleared).

##### void bitset\_pop\_at(bool *dest, bitset\_t array, size\_t key)

Set *dest to the value the element 'key' if dest is not NULL,
then remove the element 'key' from the bitset.
'key' shall be within the size of the bitset.

##### bool bitset\_front(const bitset\_t array)

Return the first element of the bitset.
The bitset shall have at least one element.

##### bool bitset\_back(const bitset\_t array)

Return the last element of the bitset.
The bitset shall have at least one element.

##### void bitset\_set\_at(bitset\_t array, size\_t i, bool value)

Set the element 'i' of bitset 'array' to 'value'.
'i' shall be within 0 to the size of the array (excluded).

##### void bitset\_flip\_at(bitset\_t array, size\_t i)

Flip the element 'i' of bitset 'array''.
'i' shall be within 0 to the size of the array (excluded).

##### bool bitset\_get(bitset\_t array, size\_t i)

Return the element 'i' of the bitset.
'i' shall be within 0 to the size of the array (excluded).

##### bool bitset\_empty\_p(const bitset\_t array)

Return true if the bitset is empty, false otherwise.

##### size\_t bitset\_size(const bitset\_t array)

Return the size of the bitset.

##### size\_t bitset\_capacity(const bitset\_t array)

Return the capacity of the bitset.

##### void bitset\_resize(bitset\_t array, size\_t size)

Resize the bitset 'array' to the size 'size' (initializing or clearing elements).

##### void bitset\_reserve(bitset\_t array, size\_t capacity)

Extend or reduce the capacity of the 'array' to a rounded value based on 'capacity'.
If the given capacity is below the current size of the bitset, 
the capacity is set to the size of the bitset.

##### void bitset\_swap(bitset\_t array1, bitset\_t array2)

Swap the bitsets 'array1' and 'array2'.

##### void bitset\_swap\_at(bitset\_t array, size\_t i, size\_t j)

Swap the elements 'i' and 'j' of the bitset 'array'.
'i' and 'j' shall reference valid elements of the array.

##### void bitset\_it(bitset\_it\_t it, bitset\_t array)

Set the iterator 'it' to the first element of 'array'.

##### void bitset\_it\_last(bitset\_it\_t it, bitset\_t array)

Set the iterator 'it' to the last element of 'array'.

##### void bitset\_it\_end(bitset\_it\_t it, bitset\_t array)

Set the iterator 'it' to the end of 'array'.

##### void bitset\_it\_set(bitset\_it\_t it1, bitset\_it\_t it2)

Set the iterator 'it1' to 'it2'.

##### bool bitset\_end\_p(bitset\_it\_t it)

Return true if the iterator doesn't reference a valid element anymore.

##### bool bitset\_last\_p(bitset\_it\_t it)

Return true if the iterator references the last element of the array,
or doesn't reference a valid element.

##### bool bitset\_it\_equal\_p(const bitset\_it\_t it1, const bitset\_it\_t it2)

Return true if both iterators reference the same element.

##### void bitset\_next(bitset\_it\_t it)

Move the iterator 'it' to the next element of the array.

##### void bitset\_previous(bitset\_it\_t it)

Move the iterator 'it' to the previous element of the array.

##### const bool *bitset\_cref(const bitset\_it\_t it)

Return a constant pointer to the element pointed by the iterator.
This pointer remains valid until the array or the iterator
is modified by another method.

##### void bitset\_get\_str(string\_t str, const bitset\_t array, bool append)

Generate a formatted string representation of the bitset 'array' and set 'str' to this
representation (if 'append' is false) or append 'str' with this representation 
(if 'append' is true).
This method is only defined if the header 'm-string.h' was included before
including 'm-bitset.h'

##### bool bitset\_parse\_str(bitset\_t array, const char str[], const char **endp)

Parse the formatted string 'str' that is assumed to be a string representation of a bitset
and set 'array' to this representation.
It returns true if success, false otherwise.
If endp is not NULL, it sets '*endp' to the pointer of the first character not
decoded by the function.

##### void bitset\_out\_str(FILE *file, const bitset\_t array)

Generate a formatted string representation of the bitset 'array' 
and outputs it into the FILE 'file'.

##### void bitset\_in\_str(bitset\_t array, FILE *file)

Read from the file 'file' a formatted string representation of a bitset
and set 'array' to this representation.

##### bool bitset\_equal\_p(const bitset\_t array1, const bitset\_t array2)

Return true if both bitsets 'array1' and 'array2' are equal.

##### size\_t bitset\_hash(const bitset\_t array)

Return a hash value of 'array'.

##### void bitset\_and(bitset\_t dst, const bitset\_t src)

Perform a bit-wise AND operation in 'dst' between 'dst' and 'src'
(effectively performing an intersection of the sets).

##### void bitset\_or(bitset\_t dst, const bitset\_t src)

Perform a bit-wise OR operation in 'dst' between 'dst' and 'src'
(effectively performing an union of the sets).

##### void bitset\_xor(bitset\_t dst, const bitset\_t src)

Perform a bit-wise XOR operation in 'dst' between dst and src.

##### void bitset\_not(bitset\_t dst)

Perform a bit-wise NOT operation for dst.

##### size\_t bitset\_clz(const bitset\_t src)

Return the leading zero position in 'src' (Count Leading Zero).

##### size\_t bitset\_popcount(const bitset\_t src)

Count the number of '1' in 'src'.



### M-STRING

This header is for using dynamic [string](https://en.wikipedia.org/wiki/String_(computer_science)).
The size of the string is automatically updated in function of the needs.

Example:

        void f(void) {
                string_t s1;
                string_init (s1);
                string_set_str (s1, "Hello, world!");
                string_clear(s1);
        }

The basic usage is a string of ASCII byte-character.
However, the functions supports also UTF-8 encoded string (except said otherwise).
It supports encoding/decoding of UTF8 code point.

#### methods, types & constants

The following methods are available:

##### string\_t

This type defines a dynamic string and is the primary type of the module.
The provided methods are just handy wrappers to the C library,
providing few algorithms on its own.

##### STRING\_FAILURE

Constant Macro defined as the index value returned in case of error.
(equal as -1U).

##### string\_fgets\_t

This type defines the different enumerate value for the string\_fgets function:

* STRING\_READ\_LINE: read a full line until the EOL character (included),
* STRING\_READ\_PURE\_LINE: read a full line until the EOL character (excluded),
* STRING\_READ\_FILE: read the full file.

##### void string\_init(string\_t str)

Init the string 'str' to an empty string.

##### void string\_clear(string\_t str)

Clear the string 'str' and frees any allocated memory.

##### char *string\_clear\_get\_str(string\_t v)

Clear the string 'str' and returns the allocated array of char,
representing a C string, giving back ownership of this array to the caller.
This array will have to be freed. It can return NULL if no array
was allocated by the string.

##### void string\_reset(string\_t str)

Reset the string 'str' to an empty string.

##### size\_t string\_size(const string\_t str)

Return the size in bytes of the string.
It can be also the number of characters of the string
if the encoding type is one character per byte.
If the characters are encoded as UTF8, the function string\_length\_u is preferred.

##### size\_t string\_capacity(const string\_t str)

Return the capacity in bytes of the string.
The capacity is the number of bytes the string accept before a
reallocation of the underlying array of char has to be performed.

##### char string\_get\_char(const string\_t v, size\_t index)

Return the byte at position 'index' of the string 'v'.
'index' shall be within the allowed range of bytes of the string.

##### void string\_set\_char(string\_t v, size\_t index, const char c)

Set the byte at position 'index' of the string 'v' to 'c'.
'index' shall be within the allowed range of bytes of the string.

##### bool string\_empty\_p(const string\_t v)

Return true if the string is empty, false otherwise.

##### void string\_reserve(string\_t v, size\_t alloc)

Update the capacity of the string to be able to receive at least
'alloc' bytes.
Calling with 'alloc' lower or equal than the size of the string
enables the function to perform a shrink
of the string to its exact needs. If the string is
empty, it will free the memory.

##### void string\_set\_str(string\_t v, const char str[])

Set the string to the array of char 'str'. 
'str' is supposed to be 0 terminated as any C string.

##### void string\_set\_strn(string\_t v, const char str[], size\_t n)

Set the string to the array of char 'str' by copying at most 'n'
char from the array. 
'str' is supposed to be 0 terminated as any C string.

##### const char* string\_get\_cstr(const string\_t v)

Return a constant pointer to the underlying array of char of the string.
This array of char is terminated by 0, enabling the pointer to be passed
to standard C function.
The pointer remains valid until the string itself remains valid 
and the next call to a function that updates the string.

##### void string\_set (string\_t v1, const string\_t v2)

Set the string 'v1' to the value of the string 'v2'.

##### void string\_set\_n(string\_t v, const string\_t ref, size\_t offset, size\_t length)

Set the string to the value of the string 'ref' by skipping the first 'offset'
char of the array of char of 'ref' and copying at most 'length' char
in the remaining array of characters of 'ref'.
'offset' shall be within the range of index of the string 'ref'.
'ref' and 'v' cannot be the same string.

##### void string\_set\_si (string\_t v1, const int n)
##### void string\_set\_ui (string\_t v1, const unsigned n)

Set the string 'v1' to the character representation of the integer 'n'.

##### void string\_init\_set(string\_t v1, const string\_t v2)

Initialize 'v1' to the value of the string 'v2'.

##### void string\_init\_set\_str(string\_t v1, const char str[])

Initialize 'v1' to the value of the array of char 'str'.
The array of char shall be terminated with 0.

##### void string\_init\_move(string\_t v1, string\_t v2)

Initialize 'v1' by stealing as most resource from 'v2' as possible
and clear 'v2' afterward.

##### void string\_move(string\_t v1, string\_t v2)

Set 'v1' by stealing as most resource from 'v2' as possible
and clear 'v2' afterward.

##### void string\_swap(string\_t v1, string\_t v2)

Swap the content of both strings.

##### void string\_push\_back (string\_t v, char c)

Append the character 'c' to the string 'v' 

##### void string\_cat\_str(string\_t v, const char str[])

Append the array of char 'str' to the string 'v'. 
The array of char shall be terminated with 0.

##### void string\_cat(string\_t v, const string\_t v2)

Append the string 'v2' to the string 'v'.
NOTE: v2 can also be a 'const char *' in C11.

##### void string\_cats(string\_t v, const string\_t v2[, ...] )

Append all the strings 'v2' ... to the string 'v'.
NOTE: v2 can also be a 'const char *' in C11.

##### void string\_sets(string\_t v, const string\_t v2[, ...] )

Set the string 'v' to the concatenation of all the strings 'v2'
NOTE: v2 can also be a 'const char *' in C11.

##### int string\_cmp\_str(const string\_t v1, const char str[])
##### int string\_cmp(const string\_t v1, const string\_t str)

Perform a byte comparison of both string
by using the strcmp function and return the result of this comparison.

##### bool string\_equal\_str\_p(const string\_t v1, const char str[])

Return true if the string is equal to the array of char, false otherwise.

##### bool string\_equal\_p(const string\_t v1, const string\_t v2)

Return true if both strings are equal, false otherwise.

##### int string\_cmpi\_str(const string\_t v, const char str[])
##### int string\_cmpi(const string\_t v, const string\_t str)

This function compares both strings by ignoring the difference due to the case.
This function doesn't work with UTF-8 strings.
It returns a negative integer if the string is before the array,
0 if there are equal,
a positive integer if the string is after the array.

##### size\_t string\_search\_char (const string\_t v, char c [, size\_t start])

Search for the character 'c' in the string from the offset 'start'.
'start' shall be within the valid ranges of offset of the string.
'start' is an optional argument. If it is not present, the default
value 0 is used instead.
This doesn't work if the function is used as function pointer.
Return the offset of the string where the character is first found,
or STRING\_FAILURE otherwise.

##### size\_t string\_search\_rchar (const string\_t v, char c [, size\_t start])

Search backwards for the character 'c' in the string from the offset 'start'.
'start' shall be within the valid ranges of offset of the string.
'start' is an optional argument. If it is not present, the default
value 0 is used instead.
This doesn't work if the function is used as function pointer.
Return the offset of the string where the character is last found,
or STRING\_FAILURE otherwise.

##### size\_t string\_search\_str (const string\_t v, char str[] [, size\_t start])
##### size\_t string\_search (const string\_t v, string\_t str [, size\_t start])

Search for the string 'str' in the string from the offset 'start'.
'start' shall be within the valid ranges of offset of the string.
'start' is an optional argument. If it is not present, the default
value 0 is used instead.
This doesn't work if the function is used as function pointer.
Return the offset of the string where 'str' is first found,
or STRING\_FAILURE otherwise.

##### size\_t string\_pbrk(const string\_t v, const char first\_of[] [, size\_t start])

Search for the first occurrence in the string 'v' from the offset 'start' of
any of the bytes in the string 'first\_of'.
'start' shall be within the valid ranges of offset of the string.
'start' is an optional argument. If it is not present, the default
value 0 is used instead.
This doesn't work if the function is used as function pointer.
Return the offset of the string where 'str' is first found,
or STRING\_FAILURE otherwise.

##### int string\_strcoll\_str(const string\_t str1, const char str2[])
##### int string\_strcoll(const string\_t str1, const string\_t str2[])

Compare the two strings str1 and str2.
It returns an integer less than, equal to, or greater than zero if 's1' is found,
respectively,  to  be  less than, to match, or be greater than s2. The
comparison is based on strings interpreted as appropriate for the program's
current locale.

##### size\_t string\_spn(const string\_t v1, const char accept[])

Calculate the length (in bytes) of the initial
segment of the string that consists entirely of bytes in accept.
       
##### size\_t string\_cspn(const string\_t v1, const char reject[])

Calculate the length (in bytes) of the initial
segment of the string that consists entirely of bytes not in reject.
       
##### void string\_left(string\_t v, size\_t index)

Keep at most the 'index' left bytes of the string,
terminating the string at the given index.
index can be out of range.

##### void string\_right(string\_t v, size\_t index)

Keep the right part of the string, after the index 'index'.

##### void string\_mid (string\_t v, size\_t index, size\_t size)

Extract the medium string from offset 'index' and up to 'size' bytes.

##### size\_t string\_replace\_str (string\_t v, const char str1[], const char str2[] [, size\_t start])
##### size\_t string\_replace (string\_t v, const string\_t str1, const string\_t str2 [ , size\_t start])

Replace in the string 'v' from the offset 'start'
the string 'str1' by the string 'str2' once.
Returns the offset of the replacement or STRING\_FAILURE if no replacement
was performed.
str1 shall be a non empty string.

##### size\_t string\_replace\_all\_str (string\_t v, const char str1[], const char str2[])
##### size\_t string\_replace\_all (string\_t v, const string\_t str1, const string\_t str2)

Replace in the string 'v' all the occurrences of 
the string 'str1' by the string 'str2'.
str1 shall be a non empty string.

##### void string\_replace\_at (string\_t v, size\_t pos, size\_t len, const char str2[])

Replace in the string 'v' the sub-string defined as starting from 'pos' and
of size 'len' by the string str2.
It assumes that pos+len is before the end of the string of 'v'.

##### void string\_init\_printf(string\_t v, const char format[], ...)

Initialize 'v' to the formatted string 'format' with the given variable argument lists.
'format' is like the printf function family.

##### void string\_init\_vprintf(string\_t v, const char format[], va\_list args)

Initialize 'v' to the formatted string 'format' with the given variable argument lists 'args'.
'format' is like the printf function family.

##### int string\_printf (string\_t v, const char format[], ...)

Set the string 'v' to the formatted string 'format'.
'format' is like the printf function family with the given variable argument list.
Return the number of characters printed (excluding the final null char), or a negative value in case of error.

##### int string\_vprintf (string\_t v, const char format[], va\_list args)

Set the string 'v' to the formatted string 'format'.
'format' is like the vprintf function family with the variable argument list 'args'.
Return the number of characters printed (excluding the final null char), or a negative value in case of error.

##### int string\_cat\_printf (string\_t v, const char format[], ...)

Appends to the string 'v' the formatted string 'format'.
'format' is like the printf function family.

##### bool string\_fgets(string\_t v, FILE \*f, string\_fgets\_t arg)

Read from the opened file 'f' a stream of characters and set 'v'
with this stream.
It stops after the character end of line
if arg is STRING\_READ\_PURE\_LINE or STRING\_READ\_LINE,
and until the end of the file if arg is STRING\_READ\_FILE.
If arg is STRING\_READ\_PURE\_LINE, the character end of line
is removed from the string.
Return true if something has been read, false otherwise.

##### bool string\_fget\_word (string\_t v, const char separator[], FILE \*f)

Read a word from the file 'f' and set 'v' with this word.
A word is separated from another by the list of characters in the array 'separator'.
(Example: "^ \t.\n").
It is highly recommended for separator to be a constant string.
'separator' shall be at most composed of 100 bytes.
It works with UTF8 stream with the restriction that the separator character shall only be ASCII character.

##### void string\_fputs(FILE \*f, const string\_t v)

Put the string in the file.

##### bool string\_start\_with\_str\_p(const string\_t v, const char str[])
##### bool string\_start\_with\_string\_p(const string\_t v, const string\_t str)

Return true if the string starts with the same characters than 'str',
false otherwise.

##### bool string\_end\_with\_str\_p(const string\_t v, const char str[])
##### bool string\_end\_with\_string\_p(const string\_t v, const string\_t str)

Return true if the string ends with the same characters than 'str',
false otherwise.

##### size\_t string\_hash(const string\_t v)

Return a hash of the string.

##### void string\_strim(string\_t v [, const char charTab[]])

Remove from the string any leading or trailing space-like characters
(space or tabulation or end of line).
If 'charTab' is given, it get the list of characters to remove from
this argument.

##### bool string\_oor\_equal\_p(const string\_t v, unsigned char n)

Provide the OOR\_EQUAL\_P method of a string.

##### void string\_oor\_set(string\_t v, unsigned char n)

Provide the OOR\_SET method of a string.

##### void string\_get\_str(string\_t v, const string\_t v2, bool append)

Convert a string into a formatted string usable for I/O:
Outputs the input string with quote around,
replacing any \" by \\\" within the string
into the output string.

##### bool string\_parse\_str(string\_t v, const char str[], const char **endp)

Parse the formatted string 'str' that is assumed to be a string representation of a string
and set 'v' to this representation.
This method is only defined if the type of the element defines a PARSE\_STR method itself.
It returns true if success, false otherwise.
If endp is not NULL, it sets '*endp' to the pointer of the first character not
decoded by the function.

##### void string\_out\_str(FILE *f, const string\_t v)

Write a string into a FILE as a formatted string:
Outputs the input string while quoting around,
replacing any \" by \\\" within the string,
and quote special characters.

##### bool string\_in\_str(string\_t v, FILE *f)

Read a formatted string from a FILE. The string shall follow the formatting rules of string\_out\_str.
It returns true if it has successfully parsed the string,
false otherwise. In this case, the position within the FILE
is undefined.

##### string\_unicode\_t

Define a type suitable to store an unicode code point as an integer.

##### string\_it\_t

Define an iterator over the string, enabling to
iterate the string over UTF8 encoded code point.

The basic element of the string is a byte.
An UTF-8 code point is composed of one to four bytes, encoded in a variable way.
However, a Unicode character can be also composed of one or more code point.
(See [UTF-8](https://en.wikipedia.org/wiki/UTF-8) )

This library doesn't provide any treatment over code points.


##### void string\_it(string\_it\_t it, const string\_t str)

Initialize the iterator 'it' to iterate over the UTF-8 code points of
string 'str'.

##### void string\_it\end(string\_it\_t it, const string\_t str)

Initialize the iterator 'it' to the end iteration of the string 'str'.

##### void string\_it\_set(string\_it\_t it1, const string\_it\_t it2)

Initialize the iterator 'it1' to the same position than 'it2.

##### void string\_it\_pos(string\_it\_t it, const string\_t str, size\_t pos)

Initialize the iterator 'it' to iterate over the UTF-8 code points
of string 'str', starting from position 'pos'.
'pos' shall be within the size of the string.
'pos' shall reference the starting point of an UTF-8 code point.

##### bool string\_end\_p (string\_it\_t it)

Return true if the iterator has reached the end of the string,
false otherwise.

##### void string\_next (string\_it\_t it)

Move the iterator to the next UTF8 encoded code point.

##### void string\_previous (string\_it\_t it)

Move the iterator to the previous UTF8 encoded code point.

##### string\_unicode\_t string\_get\_cref (const string\_it\_t it)

Return the unicode code point associated to the iterator.
It returns -1 in case of error in decoding the UTF8 string.

##### size\_t string\_it\_get\_pos (const string\_it\_t it)

Return the position in the stream of the iterator 'it'.

##### void string\_it\_set\_ref(string\_it\_t it, string\_t str, string\_unicode\_t u)

Replace the unicode code point referenced by 'it' to the unicode code point 'u'
in the string 'str'.
If 'u' is 0, then it will resize the string to the size of the referenced iterator
only keeping the unicode code points before the iterator.
'it' shall be a valid iterator of 'str'.

##### bool string\_it\_equal\_p (const string\_it\_t it1, const string\_it\_t it2)

Return true if both iterators reference the same position, false otherwise.

##### void string\_push\_u (string\_t str, string\_unicode\_t u)

Push the unicode code point 'u' into the string 'str'
encoding it as a variable UTF8 encoded code point.

##### bool string\_pop\_u (string\_unicode\_t *u, string\_t str)

Pop the last unicode code point from the string 'str'
encoded as a variable UTF8 encoded code point
and store in '*u' the popped unicode code point if 'u' is not NULL.
It returns true in case of success or false otherwise (no character to pop or no valid UTF8).

##### size\_t string\_length\_u(string\_t str)

Return the number of UTF8 encoded code point in the string.

##### bool string\_utf8\_p(string\_t str)

Return true if the string is a valid UTF8, false otherwise.
It doesn't check for unique canonical form for UTF8 string.

##### STRING\_CTE(string)

Macro to convert a constant C string into a temporary string\_t variable
suitable only for being called within a function.

##### STRING\_OPLIST

The oplist of a string\_t

##### BOUNDED\_STRING\_DEF(name, size)

Define a bounded string of size 'size', aka char[size+1] (including the final nul char).

###### Created types

The following types are automatically defined by the previous definition macro if not provided by the user:

####### name\_t

Type of the concurrent container of 'type'.

####### Generic methods

The following methods of the generic interface are defined (See generic interface for details):

* void name\_init(name\_t bounded\_string)
* void name\_init\_set(name\_t bounded\_string, const name\_t src)
* void name\_set(name\_t bounded\_string, const name\_t src)
* void name\_reset(name\_t bounded\_string)
* void name\_clear(name\_t bounded\_string)
* size\_t name\_size(const name\_t bounded\_string)
* size\_t name\_capacity(const name\_t bounded\_string)
* bool name\_empty\_p(const name\_t bounded\_string)
* void name\_get\_str(string\_t str, name\_t bounded\_string, bool append)
* void name\_out\_str(FILE *file, name\_t bounded\_string)
* bool name\_parse\_str(name\_t bounded\_string, const char str[], const char **end)
* bool name\_in\_str(name\_t bounded\_string, FILE *file)
* m\_serial\_return\_code\_t name\_out\_serial(m\_serial\_write\_t serial, const name\_t bounded\_string)
* m\_serial\_return\_code\_t name\_in\_str(name\_t bounded\_string, m\_serial\_read\_t serial)
* bool name\_equal\_p(name\_t bounded\_string1, name\_t bounded\_string2)
* int name\_cmp(const name\_t bounded\_string1, const name\_t bounded\_string2)
* size\_t name\_hash(name\_t bounded\_string)

####### Specialized methods

The following specialized methods are automatically created by the previous definition macro:

####### void name\_init\_set\_cstr)(bounded\_t string, const char str[])

Initialize the bounded string to the given C string 'str', truncating if needed.

####### void name\_set\_cstr)(bounded\_t string, const char str[])

Set the bounded string 'string' to the given C string 'str', truncating if needed.

####### void name\_set\_cstrn)(bounded\_t string, const char str[], size\_t n)

Set the bounded string 'string' to the 'n' first byte characters of the given C string 'str', truncating if needed.

####### void name\_set\_n)(bounded\_t string, const bounded\_t string, size\_t offset, size\_t length)

Set the bounded string 'string' to the 'length' first byte characters from the offset 'offset'
of the bounded given C string 'str', truncating if needed.

####### const char *name\_get\_cstr)(const bounded\_t string)

Return the C string representation of the bounded string.

####### char name\_get\_char(const bounded\_t string, size\_t index)

Return the byte character at the given index of the string.
index shall be within the size of the string.

####### void name\_cat)(bounded\_t string, const bounded_t  str)
####### void name\_cat\_cstr)(bounded\_t string, const char str[])

Concat the given C string to the bounded string.

####### int name\_cmp\_cstr)(const bounded\_t string, const char str[])

Compared the bounded string to the given C string:
return a negative integer if the bounded string is before the C string,
0 if there are equals,
a positive integer otherwise.

####### bool name\_equal\_cstr\_p)(const bounded\_t string, const char str[])

Return true if the bounded string is equal to the given C string.*

####### int name\_printf)(bounded\_t string, const char format[], ...)

Set the bounded string to the format C string.

####### int name\_cat\_printf)(bounded\_t string, const char format[], ...)

Concat to the bounded string the format C string.

####### bool name\_fgets)(bounded\_t string, FILE *f, string\_fgets\_t arg)

Read at most 'max\_size+1' bytes from the file 'f' and set the bounded string to it.

####### bool name\_fputs)(FILE *f, const bounded\_t string)

Put the bounded string to the file 'f'.

####### bool name\_oor\_equal\_p)(const bounded\_t string, unsigned char n)

Provide the OOR\_EQUAL\_P method of a string.

####### void name\_oor\_set)(bounded\_t string, unsigned char n)

Provide the OOR\_SET method of a string.


### M-CORE

This header is the internal core of M\*LIB, providing a lot of functionality 
by extending the preprocessing capability.
Working with these macros is not easy and the developer needs to know
how the macro preprocessing works.
It also adds the needed macro for handling the oplist.
As a consequence, it is needed by all other header files.

A few macros are using recursion to work.
This is not an easy feat to do as it needs some tricks to work (see
reference).
This still work well with only one major limitation: it can not be chained.
For example, if MACRO is a macro implementing recursion, then
MACRO(MACRO()) won't work.


Example:

        M_MAP(f, 1, 2, 3, 4)
        M_REDUCE(f, g, 1, 2, 3, 4)
        M_SEQ(1, 20, f)
        
#### Compiler Macros

The following compiler macros are available:

##### M\_ASSUME(cond)

M\_ASSUME is equivalent to assert, but gives hints to compiler
about how to optimize the code if NDEBUG is defined.

##### M\_LIKELY(cond) / M\_UNLIKELY(cond)

M\_LIKELY / M\_UNLIKELY gives hints on the compiler of the likelihood
of the given condition.

#### Preprocessing macro extension

##### M\_MAX\_NB\_ARGUMENT

Maximum default number of argument that can be handled by this header.
It is currently 52 (even if some local macros may have increased this limit).

##### M\_C(a,b)
##### M\_C3(a,b,c)
##### M\_C4(a,b,c,d)

Return a symbol corresponding to the concatenation of the input arguments.

##### M\_F(base, suffix)

Return a function name corresponding to the concatenation of the input arguments.
In developer mode, it can be overrided before inclusion of any header to support user customization of suffix. 
To do this you need to define M\_F as M\_OVERRIDE\_F, then define as many suffix macros as needed. As suffix macro shall be named M_OVERRIDE_ ## suffix () and each generated suffix shall start with a comma (preliminary interface).

Example:

        #define M_F(a,b) M_OVERRIDE_F(a,b)
        #define M_OVERRIDE__clear() , _cleanup ,
        #include "m-core.h"

##### M\_INC(number)

Increment the number given as argument and return 
a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).
The number shall be within the range [0..M\_MAX\_NB\_ARGUMENT-1].

##### M\_DEC(number)

Decrement the number given as argument and return 
a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).
The number shall be within the range [1..M\_MAX\_NB\_ARGUMENT].

##### M\_ADD(x, y)

Return x+y (resolution is performed at preprocessing time).
x, y shall be within [0..M\_MAX\_NB\_ARGUMENT].
If the result is not in [0..M\_MAX\_NB\_ARGUMENT+1], it returns M_OVERFLOW.

##### M\_SUB(x, y)

Return x-y (resolution is performed at preprocessing time).
x, y shall be within [0..M\_MAX\_NB\_ARGUMENT].
If the result is not in [0..M\_MAX\_NB\_ARGUMENT], it returns M_UNDERFLOW.

##### M\_BOOL(cond)

Convert an integer or a symbol into 0 (if 0) or 1 (if not 0 or symbol unknown).
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_INV(cond)

Inverse 0 into 1 and 1 into 0. It is undefined if cond is not 0 or 1
(You could use M\_BOOL to convert). 
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_AND(cond1, cond2)

Perform a logical 'and' between cond1 and cond2. 
cond1 and cond2 shall be 0 or 1.
(You could use M\_bool to convert).
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_OR(cond1, cond2)

Perform a logical 'or' between cond1 and cond2. 
cond1 and cond2 shall be 0 or 1.
(You could use M\_bool to convert).
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_NOTEQUAL(x, y)

Return 1 if x != y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_EQUAL(x, y)

Return 1 if x == y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_LESS\_THAN\_P(x, y)

Return 1 if x < y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_LESS_OR\_EQUAL\_P(x, y)

Return 1 if x <= y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_GREATER_OR\_EQUAL\_P(x, y)

Return 1 if x >= y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_GREATER_THAN\_P(x, y)

Return 1 if x > y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within [0..M\_MAX\_NB\_ARGUMENT].

##### M\_COMMA\_P(arglist)

Return 1 if there is a comma inside the argument list, 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_EMPTY\_P(expression)

Return 1 if the argument 'expression' is 'empty', 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

NOTE: It should work for a wide range of inputs
except when it is called with a macro function that takes
more than one argument and without its arguments
(in which case it generates a compiler error).
It also won't work for expression starting with ')'

##### M\_PARENTHESIS\_P(expression)

Return 1 if the argument 'expression' starts with a parenthesis and ends with it
(like '(...)'), 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_KEYWORD\_P(reference\_keyword, get\_keyword)

Return 1 if the argument 'get\_keyword' is equal to 'reference\_keyword',
0 otherwise.
reference\_keyword shall be a keyword in the following list:

* and
* or
* add (or sum, they are considered equivalent) 
* mul (or product, they are considered equivalent)
* void
* bool
* char
* short
* int
* long
* float
* double
* TYPE
* SUBTYPE
* IT_TYPE
* M\_OVERFLOW
* M\_UNDERFLOW
* SEQUENCE
* MAP
* KEYVAL
* KEYVAL\_PTR

##### M\_IF(cond)(action\_if\_true, action\_if\_false)

Return the pre-processing token 'action\_if\_true' if 'cond' is true,
action\_if\_false otherwise (meaning it is evaluated
at macro processing stage, not at compiler stage).

'cond' shall be a 0 or 1 as a preprocessing constant.
(You could use M\_bool to convert).

##### M\_IF\_EMPTY(cond)(action\_if\_true, action\_if\_false)

Return the pre-processing token 'action\_if\_true' if 'cond' is empty,
action\_if\_false otherwise (meaning it is evaluated
at macro processing stage, not at compiler stage).

'cond' shall be a preprocessing constant equal to 0 or 1.
(You could use M\_bool to convert).

##### M\_DELAY1(expr)
##### M\_DELAY2(expr) 
##### M\_DELAY3(expr)
##### M\_DELAY4(expr) 
##### M\_ID(...)

Delay the evaluation by 1, 2, 3 or 4 steps.
This is necessary to write macros that are recursive.
The argument is a macro-function that has to be deferred.
M\_ID is an equivalent of M\_DELAY1.

##### M\_EVAL(expr)

Perform a complete stage evaluation of the given expression,
removing recursive expression within it.
Only ONE M\_EVAL expression is expected in the evaluation chain.
Can not be chained.

##### M\_APPLY(func, args...)

Apply 'func' to '(args...) ensuring
that a() isn't evaluated until all 'args' have been also evaluated.
It is used to delay evaluation.

##### M\_EAT(...)

Clobber the input, whatever it is.

##### M\_RET\_ARG'N'(arglist...)

Return the argument 'N' of the given arglist.
N shall be within [1..76].
The argument shall exist in the arglist.
The arglist shall have at least one more argument that 'N'.

##### M\_GET\_AT(list, index)

Return the index 'index' of the list 'list',
which is a list of arguments encapsulated with parenthesis,
(it is not a true C array).
Return the pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

        M_GET_AT((f_0,f_1,f_2),1)
        ==>
        f_1

##### M\_SKIP\_ARGS(N,...)

Skip the Nth first arguments of the argument list.
N shall be within [0..M\_MAX\_NB\_ARGUMENT].

        M_SKIP_ARGS(2, a, b, c, d)
        ==>
        c, d

##### M\_KEEP\_ARGS(N,...)

Keep the Nth first arguments of the argument list.
N shall be within [0..M\_MAX\_NB\_ARGUMENT].

        M_KEEP_ARGS(2, a, b, c, d)
        ==>
        a, b

##### M\_MID\_ARGS(first, len,...)

Keep the medium arguments of the argument list,
starting from the 'first'-th one (zero based) and up to 'len' arguments.
first and len shall be within [0..M\_MAX\_NB\_ARGUMENT].
first+len shall be within the argument of the argument list.

        M_MID_ARGS(2, 1, a, b, c, d)
        ==>
        c

##### M\_REVERSE(args...)

Reverse the argument list. 
        
        M_REVERSE(a, b, c, d)
        ==>
        d, c, b, a

##### M\_MAP(func, args...)

Apply 'func' to each argument of the 'args...' list of argument.

      M_MAP(f, 1, 2, 3)
      ==>
      f(1) f(2) f(3)

##### M\_MAP\_C(func, args...)

Apply 'func' to each argument of the 'args...' list of argument,
putting a comma between each expanded 'func(argc)'

      M_MAP_C(f, 1, 2, 3)
      ==>
      f(1) , f(2) , f(3)

##### M\_MAP2(func, data, args...)

Apply 'func' to each couple '(data, argument)' 
with argument an element of the 'args...' list.

      M_MAP2(f, d, 1, 2, 3)
      ==>
      f(d, 1) f(d, 2) f(d, 3)

##### M\_MAP2\_C(func, data, args...)

Apply 'func' to each couple '(data, argument)' 
with argument an element of the 'args...' list,
putting a comma between each expanded 'func(argc)'

      M_MAP2_C(f, d, 1, 2, 3)
      ==>
      f(d, 1) , f(d, 2) , f(d, 3)

##### M\_MAP3(func, data, args...)

Apply 'func' to each tuple '(data, number, argument)' 
with argument an element of the 'args...' list
and number from 1 to N (the index of the list).

      M_MAP3(f, d, a, b, c)
      ==>
      f(d, 1, a) f(d, 2, b) f(d, 3, c)

##### M\_MAP3\_C(func, data, args...)

Apply 'func' to each tuple '(data, number, argument)' 
with argument an element of the 'args...' list,
and number from 1 to N (the index of the list)
putting a comma between each expanded 'func(argc)'

      M_MAP3_C(f, d, a, b, c)
      ==>
      f(d, 1, a) , f(d, 2, b) , f(d, 3, c)

##### M\_CROSS\_MAP(func, arglist1, arglist2)

Apply 'func' to each pair composed of one argument of arglist1 and one argument of arglist2,

      M_CROSS_MAP(f, (1, 3), (2, 4) )
      ==>
      f(1, 2) f(1, 4) f(3, 2) f(3, 4)

##### M\_CROSS\_MAP2(func, data, arglist1, arglist2)

Apply 'func' to each triplet composed of data, one argument of arglist1 and one argument of arglist2,

      M_CROSS_MAP2(f, 5, (1, 3), (2, 4) )
      ==>
      f(5, 1, 2) f(5, 1, 4) f(5, 3, 2) f(5, 3, 4)

##### M\_MAP\_PAIR(func, args...)

Map a macro to all given pair of arguments (Using recursion).
Can not be chained.

      M_MAP_PAIR(f, a, b, c, d)
      ==>
      f(a, b) f(c, d)

OBSOLETE macro

##### M\_REDUCE(funcMap, funcReduce, args...)

Map the macro funcMap to all given arguments 'args'
and reduce all theses computation with the macro 'funcReduce'.

        M_REDUCE(f, g, a, b, c)
        ==>
        g( f(a) , g( f(b) , f(c) ) )

##### M\_REDUCE2(funcMap, funcReduce, data, args...)

Map the macro funcMap to all pair (data, arg) of the given argument list 'args' 
and reduce all theses computation with the macro 'funcReduce'.

        M_REDUCE2(f, g, d, a, b, c)
        ==>
        g( f(d, a) , g( f(d, b) , f(d, c) ) )

##### M\_REDUCE3(funcMap, funcReduce, data, args...)

Map the macro funcMap to all tuple (data, number arg) of the given argument list 'args' 
with number from 1 to N( the size of args)
and reduce all theses computation with the macro 'funcReduce'.

        M_REDUCE3(f, g, d, a, b, c)
        ==>
        g( f(d, 1, a) , g( f(d, 2, b) , f(d, 3, c) ) )

##### M\_SEQ(init, end)

Generate a sequence of number from 'init' to 'end' (included)

        M_SEQ(1, 6)
        ==>
        1,2,3,4,5,6

##### M\_REPLICATE(N, value)

Replicate the value 'value' N times.

        M_REPLICATE(5, D)
        ==>
        D D D D D

##### M\_REPLICATE\_C(N, value)

Replicate the value 'value' N times, separating then by commas.

        M_REPLICATE_C(5, D)
        ==>
        D , D , D , D , D

##### M\_FILTER(func, data, ...)

Filter the arglists by keeping only the element that match the function 'func(data, element)'

       M_FILTER(M_NOTEQUAL, 8, 1, 3, 4, 8, 9, 8, 10)
       ==>
       1 3 4 9 10

##### M\_FILTER\_C(func, data, ...)

Filter the arglists by keeping only the element that match the function 'func(data, element)'
separated by commas.

       M_FILTER(M_NOTEQUAL, 8, 1, 3, 4, 8, 9, 8, 10)
       ==>
       1 , 3 , 4 , 9 , 10

##### M\_NARGS(args...)

Return the number of argument of the given list.
args shall not be an empty argument.

##### M\_IF\_NARGS\_EQ1(argslist)(action\_if\_one\_arg, action\_otherwise)

Return the pre-processing token 'action\_if\_one\_arg' if 'argslist' has only one argument, action\_otherwise otherwise
(meaning it is evaluated at macro processing stage, not at compiler stage).

##### M\_IF\_NARGS\_EQ2(argslist)(action\_if\_two\_arg, action\_otherwise)

Return the pre-processing token 'action\_if\_two\_arg' if 'argslist' has two arguments, action\_otherwise otherwise
(meaning it is evaluated at macro processing stage, not at compiler stage).

##### M\_IF\_DEBUG(action)

Return the pre-processing token 'action' if the build is compiled in debug mode
(i.e. NDEBUG is undefined).

##### M\_DEFAULT\_ARGS(nbExpectedArg, (defaultArgumentlist), argumentList )

Helper macro to redefine a function with one or more default values.
defaultArgumentlist is a list of the default value to complete the
list argumentList to reach the number nbExpectedArg arguments.
Example:
    int f(int a, int b, long p, void *q);
    #define f(...) f(M_DEFAULT_ARGS(4, (0, 1, NULL), __VA_ARGS__))
The last 3 arguments have their default value as 0 (for b),
1 (for p) and NULL (for q).

Experimental macro. It may disappear or change in a broken way.

##### M\_DEFERRED\_COMMA

Return a comma ',' at a later phase of the macro processing steps
(delay evaluation).

##### M\_AS\_STR(expression)

Return the string representation of the evaluated expression.


#### C11 Macro

Theses macros are only valid if the program is built in C11 mode:

##### M\_PRINTF\_FORMAT(x)

Return the printf format associated to the type of 'x'.
'x' shall be a basic C variable, printable with printf.

##### M\_PRINT\_ARG(x)

Print using printf the variable 'x'.

The format of 'x' is deduced provided that it is a standard numerical C type.
If m-string is included, it supports also the type 'string\_t' natively.
If the argument is extended (i.e. in the format '(var, optype)' with optype being either an oplist or a type
with a globally registered oplist), then it will call the OUT\_STR method of the oplist on the variable 'var'.

##### M\_FPRINT\_ARG(file, x)

Print into a file 'file' using fprintf the variable 'x'.
See M\_PRINT\_ARG for details on the supported variable.

##### M\_GET\_STRING\_ARG(string,x,append)

Print into the string\_t 'string' the variable 'x'. 
The format of 'x' is deduced provided that it is a standard numerical C type.
It needs the header 'm-string.h' for working (this macro is only a 
wrapper around it).
It supports also the type 'string\_t'.

##### M\_PRINT(args...)

Print using printf all the variable in 'args'. 
See M\_PRINT\_ARG for details on the supported variable.

##### M\_FPRINT(file, args...)

Print into a file 'file' using fprintf all the variables in 'args'. 
See M\_PRINT\_ARG for details on the supported variable.

##### M\_AS\_TYPE(type, x)

Within a C11 \_Generic statement, all expressions must be valid C
expression even if the case if always false, and is not executed.
This can seriously limit the \_Generic statement.

This macro overcomes this limitation by returning :

* either the input 'x' if it is of type 'type',
* or the value 0 view as a type 'type'.

So the returned value is **always** of type 'type' and is safe in a \_Generic statement.


#### C Macro

Theses macros expand their code at compilation level:

##### M\_MIN(x, y)
 
Return the minimum of 'x' and 'y'. x and y shall not have any side effect.

##### M\_MAX(x, y)
 
Return the maximum of 'x' and 'y'. x and y shall not have any side effect.

##### M\_POWEROF2\_P(n)
 
Return true if 'n' is a power of 2. n shall not have any side effect.

##### M\_SWAP(type, a, b)
 
Swap the values of 'a' and 'b'. 'a' and 'b' are of type 'type'. a and b shall not have any side effect.

##### M\_ASSIGN\_CAST(type, a)
 
Check if 'a' can be assigned to a temporary of type 'type' and returns this temporary.
If it cannot, the compilation failed.

##### M\_CONST\_CAST(type, a)
 
Check if 'a' can be properly casted to (const type *) and returns this casted pointer if possible.
If it cannot, the compilation failed.

##### M\_TYPE\_FROM\_FIELD(type, ptr, fieldType, field)
 
Assuming 'ptr' is a pointer to a fieldType object that is stored within a structure of type 'type'
at the position 'field', it returns a pointer to the structure.
NOTE: It is equivalent to the container\_of macro of the Linux kernel.

##### M\_CSTR(format, ...)

Return a constant string constructed based on the printf-liked formatted string
and its arguments.

The string is constructed at run time and uses a temporary space on the stack.
If the constructed string is longer than M\_USE\_CSTR\_ALLOC-1,
the string is truncated. Example:

        strcmp( M_CSTR("Len=%d", 17) , "Len=17" ) == 0


#### HASH Functions

##### M\_USE\_HASH\_SEED

A User modifiable macro defining the initial random seed (of type size\_t).
It shall be define before including any header of M\*LIB,
so that hash functions use this variable
as their initial seed for their hash computation of an object. 
It can be used to generate less predictable hash values at run-time, 
which may protect against 
[DOS dictionary attacks](https://events.ccc.de/congress/2011/Fahrplan/attachments/2007_28C3_Effective_DoS_on_web_application_platforms.pdf).
It shall be unique for a running instance of M\*LIB.

Note that using a random seed is not enough to protect efficiently against
such attacks. A cryptography secure hash may be also needed.
If it is not defined, the default is to use the value 0,
making all hash computations predictable.

##### M\_HASH\_DECL(hash)

Declare and initialize a new hash computation, named 'hash' that
is an integer.

##### M\_HASH\_UP(hash, value)

Update the 'hash' variable with the given 'value'
by incorporating the 'value' within the 'hash'. 
'value' can be up to a 'size\_t' variable.

##### uint32\_t m\_core\_rotl32a (uint32\_t x, uint32\_t n)
##### uint64\_t m\_core\_rotl64a (uint64\_t x, uint32\_t n)

Perform a rotation of 'n' bits of the input 'x'.
n shall be within 1 and the number of bits of the type minus 1.

##### uint64\_t m\_core\_roundpow2(uint64\_t v)

Round to the next highest power of 2.

##### unsigned int m\_core\_clz32(uint32\_t limb)
##### unsigned int m\_core\_clz64(uint64\_t limb)

Count the number of leading zero and return it.
limb can be 0.

##### size\_t m\_core\_hash (const void *str, size\_t length)

Compute the hash of the binary representation of the data pointer by 'str'
of length 'length'. 'str' shall have the maximum alignment restriction
of all types that size is less or equal than 'length'.


#### OPERATORS Functions

##### M\_GET\_INIT oplist
##### M\_GET\_INIT\_SET oplist
##### M\_GET\_INIT\_MOVE oplist
##### M\_GET\_SET oplist
##### M\_GET\_MOVE oplist
##### M\_GET\_SWAP oplist
##### M\_GET\_CLEAR oplist
##### M\_GET\_NEW oplist
##### M\_GET\_DEL oplist
##### M\_GET\_REALLOC oplist
##### M\_GET\_FREE oplist
##### M\_GET\_MEMPOOL oplist
##### M\_GET\_MEMPOOL\_LINKAGE oplist
##### M\_GET\_HASH oplist
##### M\_GET\_EQUAL oplist
##### M\_GET\_CMP oplist
##### M\_GET\_TYPE oplist
##### M\_GET\_SUBTYPE oplist
##### M\_GET\_OPLIST oplist
##### M\_GET\_SORT oplist
##### M\_GET\_IT\_TYPE oplist
##### M\_GET\_IT\_FIRST oplist
##### M\_GET\_IT\_LAST oplist
##### M\_GET\_IT\_END oplist
##### M\_GET\_IT\_SET oplist
##### M\_GET\_IT\_END\_P oplist
##### M\_GET\_IT\_LAST\_P oplist
##### M\_GET\_IT\_EQUAL\_P oplist
##### M\_GET\_IT\_NEXT oplist
##### M\_GET\_IT\_PREVIOUS oplist
##### M\_GET\_IT\_REF oplist
##### M\_GET\_IT\_CREF oplist
##### M\_GET\_IT\_REMOVE oplist
##### M\_GET\_IT\_INSERT oplist
##### M\_GET\_ADD oplist
##### M\_GET\_SUB oplist
##### M\_GET\_MUL oplist
##### M\_GET\_DIV oplist
##### M\_GET\_RESET oplist
##### M\_GET\_PUSH oplist
##### M\_GET\_POP oplist
##### M\_GET\_REVERSE oplist
##### M\_GET\_GET\_STR oplist
##### M\_GET\_OUT\_STR oplist
##### M\_GET\_IN\_STR oplist
##### M\_GET\_SEPARATOR oplist
##### M\_GET\_EXT\_ALGO oplist
##### M\_GET\_INC\_ALLOC oplist
##### M\_GET\_OOR\_SET oplist
##### M\_GET\_OOR\_EQUAL oplist

Return the associated method to the given operator within the given oplist.

##### M\_BOOL\_OPLIST

Oplist for C Boolean (\_Bool / bool)

##### M\_BASIC\_OPLIST

Oplist for C basic types (int / float)

##### M\_ENUM\_OPLIST(type, init\_value)

Oplist for a C standard enumerate of type 'type',
of initial value 'init\_value'

##### M\_CSTR\_OPLIST

Oplist for the C type 'const char *', seen as a constant string.
(The string of characters is not copied).

##### M\_POD\_OPLIST

Oplist for a structure C type without any init & clear methods
prerequisites (plain old data).

##### M\_A1\_OPLIST

Oplist for an array of size 1 of a structure C type without any init & clear
methods prerequisites.

##### M\_EMPTY\_OPLIST

Oplist for a type that shall not be instantiated.
Each method does nothing.

##### M\_CLASSIC\_OPLIST(name)

Create the oplist with the operators using the pattern 'name', i.e.
name##\_init, name##\_init\_set, name##\_set, name##\_clear, name\_t

##### M\_OPFLAT oplist

Remove the parenthesis around an oplist.

##### M\_OPCAT(oplist1,oplist2)

Concat two oplists in one. 'oplist1''s operators will have higher priority to 'oplist2'

##### M\_OPEXTEND(oplist, ...)

Extend an oplist with the given list of operators.
Theses new operators will have higher priority than the ones
in the given oplist.

##### M\_TEST\_METHOD\_P(method, oplist)
##### M\_TEST\_METHOD\_ALTER\_P(method, oplist)

Test if a method is present in an oplist. Return 0 or 1.
M\_TEST\_METHOD\_P does not work if the returned method is something within parenthesis (like OPLIST*)
If this case is possible, you shall use the M_TEST_METHOD_ALTER_P macro (safer but slower).

##### M\_IF\_METHOD(method, oplist) 

Perform a preprocessing M\_IF, if the method is present in the oplist.
Example: M\_IF\_METHOD(HASH, oplist)(define function that uses HASH method, ) 

##### M\_IF\_METHOD\_BOTH(method, oplist1, oplist2)     

Perform a preprocessing M\_IF if the method exists in both oplist.
Example: M\_IF\_METHOD\_BOTH(HASH, oplist1, oplist2) (define function , )

##### M\_IF\_METHOD\_ALL(method, ...)

Perform a preprocessing M\_IF if the method exists for all oplist.
Example: M\_IF\_METHOD\_ALL(HASH, oplist1, oplist2, oplist3) (define function, ) 

##### M\_GET\_PROPERTY(oplist, propname)

Return the content of the property named 'propname' as defined in the PROPERTIES field of the 'oplist',
or 0 if it is not defined.

##### M\_TEST\_NOEXCEPT\_P(oplist, operator)

Test if the operator has the property NOEXCEPT or not in the oplist.
If this property is set to 1 for this operator, it means that there is a guarantee that the operator won't raise any error.
If the operator has not this property or it is set to 0, it means that it may raise some error (default).

##### M\_DO\_INIT\_MOVE(oplist, dest, src)
##### M\_DO\_MOVE(oplist, dest, src)

Perform an INIT\_MOVE/MOVE if present, or emulate it otherwise (Internal macros).
Note: default methods for INIT\_MOVE/MOVE are not robust enough yet.

##### M\_INIT\_WITH\_THROUGH\_EMPLACE\_TYPE(oplist, dest, src)

Use the provided EMPLACE\_TYPE of the oplist to emulate an INIT\_WITH operator.
It transforms the different emplace types by a C11 _Generic switch in order to call the
right initialization function in function of the type of argument.

The EMPLACE\_TYPE shall use a LIST based format for listing the different emplace types.
This method is compatible with C11 or above.
This method shall be used with API\_1 adapter.

##### M\_SET\_THROUGH\_INIT\_SET(oplist, dest, src)

Emulate the SET semantics using a combination of CLEAR and INIT\_SET of the given oplist.
This method shall be used with API\_1 adapter.

##### M\_GLOBAL\_OPLIST(a)

If 'a' is a type that has registered a global oplist, it returns the registered oplist,
otherwise it return 'a'.

'a' shall be a type or an oplist.
NOTE: It tests the existence of the symbol M\_OPL\_##a().
See M\_OPL\_ to register an oplist to a type.
Global registered oplists are limited to typedef types.
   
##### M\_GLOBAL\_OPLIST\_OR\_DEF(a)

If 'a' is a type that has registered a global oplist, it returns the registered oplist,
otherwise it return the oplist M\_BASIC\_OPLIST.

The return value shall be evaluated once again to get the oplist
(this is needed due to technical reasons) like this:

       M_GLOBAL_OPLIST_OR_DEF(mpz_t)()

'a' shall be a type or an oplist.
NOTE: It tests the existence of the symbol M\_OPL\_##a().
See M\_OPL\_ to register an oplist to a type.
Global registered oplists are limited to typedef types.


#### Syntax enhancing

These macros are quite useful to lighten the C style and make full use of the library.

##### M\_EACH(item, container, oplist|type)

This macro iterates over the given 'container' of oplist 'oplist'
(or of type 'type' with a globally registered oplist) and sets 'item' 
to reference one different element of the container for each iteration of
the loop.

'item' is a created pointer variable to the contained type
of the container, only available within the 'for' loop.
There can only have one M\_EACH per line.
It shall be used after the 'for' C keyword to perform a loop over the container.
The order of the iteration depends on the given container.

Example: 
         for M_EACH(item, list, LIST_OPLIST) { action; }

##### M\_LET(var1[,var2[,...]], oplist|type)

This macro defines the variable 'var1'(resp. var2, ...) of oplist 'oplist'.
If a type (instead of an oplist) is used, the oplist of the type shall have been registered globally.
It initializes 'var1' (resp. var2, ...) by calling the initialization method,
and clears 'var1' (resp. var2, ...) by calling the clear method
when the bracket associated to the M\_LET go out of scope.

Its formulation is based on the expression 'let var1 as type in'

Several forms are supported for the initialization method:

* If 'var1' (resp. var2, ...) has the form (v1, arguments...),
then the variable 'v1' will be initialized with the 
contains of the initializing list 'arguments...'.

** If 'arguments' is single and within parenthesis
or if there is more one argument
or if the property LET\_AS\_INIT\_WITH of the type is defined,
it will use the operator INIT\_WITH (if it exists) to emplace the variable
with the given arguments (The arguments are therefore expected to be compatible with
the INIT\_WITH operator).
** Otherwise it will use the operator INIT\_SET (argument is expected
to be the same type as the initialized type in such case).

* Otherwise it will use the empty initializer INIT operator.

LIMITATION:
An argument shall not contain any comma or it shall be put
between parenthesis. In particular, if the argument is a compound
literal the full compound literal shall be put between parenthesis
and casted to the right type outside the parenthesis (due to the conflict
with the INIT\_WITH detection it is needed to put something outside the
parenthesis like a cast).

An argument shall not have any post effect when it is evaluated.

There shall be at most one M\_LET macro per line of source code.

Example:

     M_LET(a, STRING_OPLIST) { do something with a }  or
     M_LET(a, b, c, string_t) { do something with a, b & c }
     M_LET( (a, ("Hello")), string_t) { do something with a initialized to "Hello" }

NOTE: The user code shall not perform a return or a goto (or longjmp) outside the {} or a call to an exit function
otherwise the clear code of the object won't be called .
However, you can use the break instruction to quit the block (the clear function will be executed).

You can chain the M\_LET macro to create several different variables.


##### M\_LET\_IF(init\_code, test\_code, clear\_code [, else\_code] )

This macro defines the variable(s) in 'init\_code',
executes the next block of instructions where the variable(s) is(are) used
if the initialization succeeds by testing 'test\_code',
then it executes the 'clear\_code'.

'test\_code' shall return a boolean indicating if the initialization 
succeeds (true) or not.
If the initialization fails, it won't call the 'clear\_code', but the 'else\_code' if it is present.

The syntax of 'init\_code' is the same as a one of a for loop.

There shall be at most one M\_LET\_IF macro per line of source code.

Example:

    M_LET_IF(int *p = malloc(100), p!=0, free(p)) {
      M_LET_IF( /* nothing */ , mtx_lock(&mut)!=thrd_error, mtx_unlock(&mut)) {
        printf ("OK! Let's use p in a mutex section\n");
      }
    }

NOTE:
The user code shall not perform a return or a goto (or longjmp) 
outside the {} or a call to an exit function
otherwise the clear\_code won't be called .
You can use the break instruction to quit the block
(the clear\_code will be executed) or you can use exception.
You can chain the M\_LET\_IF macro to create several different variables.


##### M\_DEFER(clear\_code[, ...])

This macro registers the execution of 'clear\_code' when reaching 
the further closing brace of the next block of instruction.
clear\_code shall be a valid expression.

There shall be at most one M\_DEFER macro per line of source code.

Example:

        m_mutex_lock(mut);
        M_DEFER(m_mutex_unlock(mut)) {
            // Use of the critical section.
        } // Now m_mutex_unlock is called

NOTE:
The user code shall not perform a return or a goto (or longjmp) 
outside the {} or a call to an exit function
otherwise the clear\_code won't be called.
You can use the break instruction to quit the block
(the clear\_code will be executed) or you can use exception.


##### M\_CHAIN\_INIT(init\_code, clear\_code)

This macro executes 'init\_code' then
registers the execution of 'clear\_code' if an exception is triggered
until the further closing brace of the next block of instruction.
init\_code and clear\_code shall be a valid expression.
If exception are not enabled, it simply executes 'init\_code'.

There shall be at most one M\CHAIN\_INIT macro per line of source code.
It can be chained multiple times to register multiple registrations.

Therefore it enables support for chaining
initialization at the begining of a constructor for the fields of the constructed
object so that even if the constructor failed and throw an exception, 
the fields of the constructed object are properly cleared.

This is equivalent to C++ construct: 

        void type() : field1(), field2() { rest of constructor }

M_CHAIN_INIT shall be the first instructions of the constructor function.

Example:

        void struct_init_set(struct_t d, struct_t s)
        {
          M_CHAIN_INIT(string_init_set(d->s1, s->s1), string_clear(d->s1) )
          M_CHAIN_INIT(string_init_set(d->s2, s->s2), string_clear(d->s2) ) {
            d->num = s->num;
          }
        }


#### Memory / Error macros

All these macro can be overridden before including the header m-core.h
so that they can be adapted to a particular memory pool.


##### type *M\_MEMORY\_ALLOC (type)

Return a pointer to a new allocated non-initialized object of type 'type'.
In case of allocation error, it returns NULL.

The default used function is the 'malloc' function of the LIBC.

##### void M\_MEMORY\_DEL (type *ptr)

Delete the cleared object pointed by the pointer 'ptr'
that was previously allocated by the macro M\_MEMORY\_ALLOC.
'ptr' can not be NULL.

The default used function is the 'free' function of the LIBC.

##### type *M\_MEMORY\_REALLOC (type, ptr, number)

Return a pointer to an array of 'number' objects of type 'type'
'ptr' is either NULL (in which the array is allocated), 
or a pointer returned from a previous call of M\_MEMORY\_REALLOC 
(in which case the array is reallocated).
The objects are not initialized, nor the state of previous objects changed
(in case of reallocation).
The address of the previous objects may have moved and the MOVE operator
is not used in this case.
In case of allocation error, it returns NULL.

The default used function is the 'realloc' function of the LIBC.

##### void M\_MEMORY\_FREE (type *ptr)

Delete the cleared object pointed by the pointer 'ptr'.
The pointer was previously allocated by the macro M\_MEMORY\_REALLOC.
'ptr' can not be NULL.

The default used function is the 'free' function of the LIBC.

##### void M\_MEMORY\_FULL (size\_t size)

This macro is called by M\*LIB when a memory error has been detected.
The parameter 'size' is what was tried to be allocated (as a hint).
The macro can :

* abort the execution,
* throw an exception (In this case, the state of the object is unchanged),
* set a global error variable and return.

NOTE: The last two cases are not properly fully supported yet.
Throwing an exception is not fully supported yet 
(Needs M\*LIB support to clear the skipped objects).

The default is to raise a fatal error.

##### void M\_ASSERT\_INIT(expression, object\_name)

This macro is called when an assertion used in an initialization context
is called to check the good creation of an object (like a thread, a mutex)
that string name is 'object\_name'. 
If the given 'expression' is false, the execution shall be aborted.
The assertion is kept in programs built in release mode.

The default is to raise a fatal error.

##### M\_RAISE\_FATAL(message...)

This macro is called by the user code to raise a fatal error and terminate
the program execution. This macro shall not return.

By default, it prints the error message on stderr and calls abort to terminate
program execution.

##### M\_ASSERT(expression)

This macro defines the generic assert macro used by M\*LIB.
See the assert C macro for details.

The default is the assert macro.

##### M\_ASSERT\_SLOW(expression)

This macro defines the specialized assert macro used by M\*LIB
which is used on slow assertion properties
(that have a significant impact on program time).
See the assert C macro for details.

The default is the assert macro if M\*LIB test suite, nothing otherwise.

##### M\_ASSERT\_INIT(expression)

This macro defines the specialized assert macro used by M\*LIB
which shall be kept on release program.
See the assert C macro for details.

The default is to raise a fatal error if the expression is false.

##### M\_ASSERT\_INDEX(index, maximum)

This macro defines the specialized assert macro used by M\*LIB
which is used for buffer overflow checking:
it checks if index is in the range 0 to maximum-1.

The default is to call M\_ASSERT with this property.


#### Generic Serialization objects

A [serialization](https://en.wikipedia.org/wiki/Serialization) is the process of translating
data structures into a format that can be stored or transmitted.
When the resulting format is reread and translated, 
it creates a semantically identical clone of the original object. 

A generic serialization object is an object that takes a C object (Boolean, integer, float, 
structure, union, pointer, array, list, hash-map, ...) and outputs it into a serialization way through
the following defined interface that defined the format of the serialization and where it is 
physically output. 

The M\*LIB containers define the methods \_out\_serial and \_in\_serial
if the underlying types define theses methods over the operators OUT\_SERIAL and IN\_SERIAL.
The methods for the basic C types (int, float, ...) are also defined (but only
in C11 due to technical limitation).

The methods \_out\_serial and \_in\_serial will request the generic serialization object
to serialize their current object:

* calling the interface of the generic serialization object if needed, 
* performing recursive call to serialize the contained-objects.

The final output of this serialization can be a FILE or a string.
Two kinds of serialization objects exist: one for input and one
for output. The serialization is fully recursive and can be seen as a collection of token.
The only constraint is that what is output by the output serialization object
shall be able to be parsed by the input serialization object.


The serialization input object is named as m\_serial\_read\_t, defined in m-core.h as a structure
(of array of size 1) with the following fields:

* m\_interface: a pointer to the constant m\_serial\_read\_interface\_s structure that defines all
methods that operate on this object to parse it. The instance has to be customized for the needs of the
wanted serialization.
* data: a table of M\_SERIAL\_MAX\_DATA\_SIZE of C types (Boolean, integer, size or pointer).
This data is used to store the needed data for the methods.

This is pretty much like a pure virtual interface object in C++. The interface has to define
the following fields with the following definition:

* read\_boolean:
   Read from the stream 'serial' a boolean. 
   Set '*b' with the boolean value if it succeeds.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise
* read\_integer:
   Read from the stream 'serial' an integer that can be represented with 'size\_of\_type' bytes.
   Set '*i' with the integer value if it succeeds. 
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise.
* read\_float:
   Read from the stream 'serial' a float that can be represented with 'size\_of\_type' bytes.
   Set '*r' with the float value if it succeeds. 
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise.
* read\_string:
   Read from the stream 'serial' a string.
   Set 's' with the string if it succeeds. 
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise
* read\_array\_start:
   Start reading from the stream 'serial' an array (which is defined as a sequential collection of object).
   Set '*num' with the number of elements, or (size\_t)-1 if it is not known.
   Initialize the object 'local' so that it can be used by the serialization object to serialize the array. 
   ('local' is an unique local serialization object of the array).
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the parsing of the array can continue (the array is not empty),
   M\_SERIAL\_OK\_DONE if it succeeds and the array ends (the array is empty),
   M\_SERIAL\_FAIL\_RETRY if it doesn't support unknown number of elements,
   M\_SERIAL\_FAIL otherwise.
* read\_array\_next:
   Continue reading from the stream 'serial' an array using 'local' to load / save data if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the array can continue (the array end is still not reached),
   M\_SERIAL\_OK\_DONE if it succeeds and the array ends,
   M\_SERIAL\_FAIL otherwise.
* read\_map\_start:
   Start reading from the stream 'serial' a map (an associative array).
   Set '*num' with the number of elements, or 0 if it is not known.
   Initialize 'local' so that it can be used to serialize the map. 
   ('local' is an unique serialization object of the map).
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the map continue,
   M\_SERIAL\_OK\_DONE if it succeeds and the map ends (the map is empty),
   M\_SERIAL\_FAIL otherwise
* read\_map\_value:
   Continue reading from the stream 'serial' the value separator token (if needed)
   using 'local' to load / save data if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the map continue,
   M\_SERIAL\_FAIL otherwise
* read\_map\_next:
   Continue reading from the stream 'serial' a map.
   using 'local' to load / save data if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the map continue,
   M\_SERIAL\_OK\_DONE if it succeeds and the map ends,
   M\_SERIAL\_FAIL otherwise
* read\_tuple\_start:
   Start reading a tuple from the stream 'serial'.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the tuple continues,
   M\_SERIAL\_FAIL otherwise
* read\_tuple\_id:
   Continue reading a tuple (a structure) from the stream 'serial'.
   using 'local' to load / save data if needed.
   Set '*id' with the corresponding index of the table 'field\_name[max]'
   associated to the parsed field in the stream.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the tuple continues,
   Return M\_SERIAL\_OK\_DONE if it succeeds and the tuple ends,
   M\_SERIAL\_FAIL otherwise
* read\_variant\_start:
   Start reading a variant (an union) from the stream 'serial'.
   Set '*id' with the corresponding index of the table 'field\_name[max]'
   associated to the parsed field in the stream.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds and the variant continues,
   Return M\_SERIAL\_OK\_DONE if it succeeds and the variant ends(variant is empty),
   M\_SERIAL\_FAIL otherwise
* read\_variant\_end:
   End reading a variant from the stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds and the variant ends,
   M\_SERIAL\_FAIL otherwise

The serialization output object is named as m\_serial\_write\_t, defined in m-core.h as a structure
(of array of size 1) with the following fields:

* m\_interface: a pointer to the constant m\_serial\_write\_interface\_s structure that defines all
methods that operate on this object to output it. The instance has to be customized for the needs of the
wanted serialization.
* data: a table of M\_SERIAL\_MAX\_DATA\_SIZE of C types (boolean, integer, size or pointer).
This data is used to store the needed data for the methods.

This is pretty much like a pure virtual interface object in C++. The interface has to defines
the following fields with the following definition:

* write\_boolean:
   Write the boolean 'b' into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_integer:
   Write the integer 'data' of 'size\_of\_type' bytes into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_float:
   Write the float 'data' of 'size\_of\_type' bytes into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_string:
   Write the null-terminated string 'data' of 'length' characters into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_array\_start:
   Start writing an array of 'number\_of\_elements' objects into the serial stream 'serial'.
   If 'number\_of\_elements' is 0, then either the array has no data,
   or the number\ of\ elements of the array is unknown.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_array\_next:
   Write an array separator between elements of an array into the serial stream 'serial' if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_array\_end:
   End the writing of an array into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_map\_start:
   Start writing a map of 'number\_of\_elements' pairs of objects into the serial stream 'serial'.
   If 'number\_of\_elements' is 0, then either the map has no data,
   or the number of elements is unknown.
   Initialize 'local' so that it can be used to serialize the map 
   (local is an unique serialization object of the map).
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_map\_value:
   Write a value separator between element of the same pair of a map into the serial stream 'serial' if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_map\_next:
   Write a map separator between elements of a map into the serial stream 'serial' if needed.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_map\_end:
   End the writing of a map into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_tuple\_start:
   Start writing a tuple into the serial stream 'serial'.
   Initialize 'local' so that it can serial the tuple 
   (local is an unique serialization object of the tuple).
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_tuple\_id:
   Start writing the field named field\_name[index] of a tuple into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_tuple\_end:
   End the write of a tuple into the serial stream 'serial'.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_variant\_start:
   Start writing a variant into the serial stream 'serial'.
   If index <= 0, the variant is empty.
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise 
   Otherwise, the field 'field\_name[index]' will be filled.
   Return M\_SERIAL\_OK\_CONTINUE if it succeeds, M\_SERIAL\_FAIL otherwise */
* write\_variant\_end:
   End Writing a variant into the serial stream 'serial'. 
   Return M\_SERIAL\_OK\_DONE if it succeeds, M\_SERIAL\_FAIL otherwise */

M\_SERIAL\_MAX\_DATA\_SIZE can be overloaded before including any M\*LIB header
to increase the size of the generic object. The maximum default size is 4 fields.

The full C definition are:

        // Serial return code
        typedef enum m_serial_return_code_e {
         M_SERIAL_OK_DONE = 0, M_SERIAL_OK_CONTINUE = 1, M_SERIAL_FAIL = 2
        } m_serial_return_code_t;
        
        // Different types of types that can be stored in a serial object to represent it.
        typedef union m_serial_ll_u {
          bool   b;
          int    i;
          size_t s;
          void  *p;
        } m_serial_ll_t;
        
        /* Object to handle the construction of a serial write/read of an object
           that needs multiple calls (array, map, ...)
           It is common to all calls to the same object */
        typedef struct m_serial_local_s {
         m_serial_ll_t data[M_USE_SERIAL_MAX_DATA_SIZE];
        } m_serial_local_t[1];
        
        // Object to handle the generic serial read of an object.
        typedef struct m_serial_read_s {
         const struct m_serial_read_interface_s *m_interface;
         m_serial_ll_t data[M_USE_SERIAL_MAX_DATA_SIZE];
        } m_serial_read_t[1];
        
        // Interface exported by the serial read object.
        typedef struct m_serial_read_interface_s {
          m_serial_return_code_t (*read_boolean)(m_serial_read_t serial,bool *b);
          m_serial_return_code_t (*read_integer)(m_serial_read_t serial, long long *i, const size_t size_of_type);
          m_serial_return_code_t (*read_float)(m_serial_read_t serial, long double *f, const size_t size_of_type);
          m_serial_return_code_t (*read_string)(m_serial_read_t serial, struct string_s *s); 
          m_serial_return_code_t (*read_array_start)(m_serial_local_t local, m_serial_read_t serial, size_t *s);
          m_serial_return_code_t (*read_array_next)(m_serial_local_t local, m_serial_read_t serial);
          m_serial_return_code_t (*read_map_start)(m_serial_local_t local, m_serial_read_t serial, size_t *);
          m_serial_return_code_t (*read_map_value)(m_serial_local_t local, m_serial_read_t serial);
          m_serial_return_code_t (*read_map_next)(m_serial_local_t local, m_serial_read_t serial);
          m_serial_return_code_t (*read_tuple_start)(m_serial_local_t local, m_serial_read_t serial);
          m_serial_return_code_t (*read_tuple_id)(m_serial_local_t local, m_serial_read_t serial, const char *const field_name [], const int max, int *id);
          m_serial_return_code_t (*read_variant_start)(m_serial_local_t local, m_serial_read_t serial, const char *const field_name[], const int max, int*id);
          m_serial_return_code_t (*read_variant_end)(m_serial_local_t local, m_serial_read_t serial);
        } m_serial_read_interface_t;
        
        
        // Object to handle the generic serial write of an object.
        typedef struct m_serial_write_s {
         const struct m_serial_write_interface_s *m_interface;
         m_serial_ll_t data[M_USE_SERIAL_MAX_DATA_SIZE];
        } m_serial_write_t[1];
        
        // Interface exported by the serial write object.
        typedef struct m_serial_write_interface_s {
          m_serial_return_code_t (*write_boolean)(m_serial_write_t serial, const bool b);
          m_serial_return_code_t (*write_integer)(m_serial_write_t serial, const long long i, const size_t size_of_type);
          m_serial_return_code_t (*write_float)(m_serial_write_t serial,  const long double f, const size_t size_of_type);
          m_serial_return_code_t (*write_string)(m_serial_write_t serial, const char s[], size_t length); 
          m_serial_return_code_t (*write_array_start)(m_serial_local_t local, m_serial_write_t serial, const size_t number_of_elements);
          m_serial_return_code_t (*write_array_next)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_array_end)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_map_start)(m_serial_local_t local, m_serial_write_t serial,  const size_t number_of_elements);
          m_serial_return_code_t (*write_map_value)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_map_next)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_map_end)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_tuple_start)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_tuple_id)(m_serial_local_t local, m_serial_write_t serial, const char * const field_name[], const int max, const int index);
          m_serial_return_code_t (*write_tuple_end)(m_serial_local_t local, m_serial_write_t serial);
          m_serial_return_code_t (*write_variant_start)(m_serial_local_t local, m_serial_write_t serial,  const char * const field_name[], const int max, const int index);
          m_serial_return_code_t (*write_variant_end)(m_serial_local_t local, m_serial_write_t serial);
        } m_serial_write_interface_t;

See [m-serial-json.h](#m-serial-json) for example of use.


### M-THREAD

This header is for providing very thin layer around OS implementation of threads, conditional variables and mutex.
It has back-ends for WIN32, POSIX thread, FreeRTOS tasks or C11 thread.

It was needed due to the low adoption rate of the C11 equivalent layer.

It uses the FreeRTOS tasks is it detects FreeRTOS.
Otherwise it uses the C11 threads.h if possible.
If the C11 implementation does not respect the C standard
(i.e. the compiler targets C11 mode,
the  \_\_STDC\_NO\_THREADS\_\_ macro is not defined
but the header threads.h is not available or not working),
then the user shall define manually the M\_USE\_THREAD\_BACKEND
macro to select the back end to use:

* 1: for C11
* 2: for WINDOWS
* 3: for PTHREAD
* 4: for FreeRTOS

The FreeRTOS backend uses two additional global configuration constants:

* M\_USE\_TASK\_STACK\_SIZE to define the stack size of a thread (default is the minimal)
* M\_USE\_TASK\_PRIORITY to define the priority of a thread (default is lowest).

Example:

        m_thread_t idx_p;
        m_thread_t idx_c;

        m_thread_create (idx_p, conso_function, NULL);
        m_thread_create (idx_c, prod_function, NULL);
        m_thread_join (idx_p;
        m_thread_join (idx_c);

#### Attributes

The following attributes are available:

##### M\_THREAD\_ATTR

An attribute used for qualifying a variable to be thread specific.
It is an alias for \_\_thread, \_Thread\_local or \_\_declspec( thread )
depending on the used backend.


#### methods

The following methods are available:

##### m\_mutex\_t

A type representing a mutex.

##### void m\_mutex\_init(m\_mutex\_t mutex)

Initialize the variable mutex and sets it to the unlocked position.
If the initialization fails, the program aborts.
Only one thread shall initialize the mutex.

##### void m\_mutex\_clear(m\_mutex\_t mutex)

Clear the variable mutex. 
Only one thread shall clear the mutex.

##### void m\_mutex\_lock(m\_mutex\_t mutex)

Lock the variable mutex for exclusive use.
If the mutex is not unlocked, it will wait indefinitely until it is.
The mutex shall not be already locked.

##### void m\_mutex\_unlock(m\_mutex\_t mutex)

Unlock the variable mutex for exclusive use.
The mutex shall not be already unlocked.

#### m\_cond\_t

A type representing a conditional variable, which shall be used within a mutex section.

##### void m\_cond\_init(m\_cond\_t cond)

Initialize the conditional variable cond. 
If the initialization failed, the program aborts.
Only one thread shall init the conditional variable.

##### void m\_cond\_clear(m\_cond\_t cond)

Clear the variable cond. 
If the variable is not initialized, the behavior is undefined.
Only one thread shall clear the conditional variable.

##### void m\_cond\_signal(m\_cond\_t cond)

Signal that the event associated to the variable cond 
has occurred to at least a single thread.
All usage of this conditional variable shall be done within the same mutex exclusve section.

##### void m\_cond\_broadcast(m\_cond\_t cond)

Signal that the event associated to the variable cond 
has occurred to all waiting threads.
All usage of this conditional variable shall be done within the same mutex exclusve section.

##### void m\_cond\_wait(m\_cond\_t cond, m\_mutex\_t mutex)

Wait indefinitely for the event associated to the variable cond
to occur.
IF multiple threads wait for the same event, which thread to awoken
is not specified.
All usage of this conditional variable shall be done within the mutex exclusve section.

#### m\_thread\_t

A type representing an id of a thread.

##### void m\_thread\_create(m\_thread\_t thread, void (\*function)(void\*), void \*argument)

Create a new thread and set the it of the thread to 'thread'.
The new thread run the code function(argument) with
argument a 'void \*' and function taking a 'void \*' and returning
nothing.
If the initialization fails, the program aborts.

##### void m\_thread\_join(m\_thread\_t thread)

Wait indefinitely for the thread 'thread' to exit.

#### m\_once\_t

A type representing a helper structure for m_once_call.
An object of type m\_once\_t shall be initialized at declaration level with M\_ONCE\_INIT\_VALUE

#### M\_ONCE\_INIT\_VALUE

Initial value to set an object of type m\_once\_t when declaring it.

#### void m\_once\_call(m\_once\_t obj, void (*func)(void))

Executes the function pointer func exactly once,
even if called concurrently, from several threads,
provided that they share the same object obj.


### M-WORKER

This header is for providing a pool of workers.
Each worker run in a separate thread and can handle work orders
sent by the main threads. A work order is a computation task.
Work orders are organized around synchronization points.
Workers can be disabled globally to ease debugging.

This implements parallelism just like OpenMP or CILK++.

Example:

        worker_t worker;
        worker_init(worker, 0, 0, NULL);
        worker_sync_t sync;
        worker_start(sync, worker);
        void *data = ...;
        worker_spawn (sync, taskFunc, data);
        taskFunc(otherData);
        worker_sync(sync);
        

Currently, there is no support for:

* throw exceptions by the worker tasks,
* unbalanced design: the worker tasks shall not lock a mutex without closing it (same for other synchronization structures).

Thread Local Storage variables have to be reinitialized properly
with the reset function. This may result in subtle difference between the
serial code and the parallel code.

#### methods

The following methods are available:

#### worker\_t

A pool of worker.

The type is incompatible between C++, C/CLANG and C(all others) due to technical constraints,
so a variable of such a type shall always be accessed by the same compiler.

#### worker\_sync\_t

A synchronization point between workers.

#### void worker\_init(worker\_t worker[, unsigned int numWorker, unsigned int extraQueue, void (*resetFunc)(void), void (*clearFunc)(void) ])

Initialize the pool of workers 'worker' with 'numWorker' workers.
if 'numWorker' is 0, then it will detect how many core is available on the
system and creates as much workers as there are cores.

Before starting any work, the function 'resetFunc'
is called by the worker to reset its state (or call nothing if the function
pointer is NULL).

'extraQueue' is the number of tasks that can be accepted by the work order
queue in case if there is no worker available.

Before terminating, each worker will call 'clearFunc' if the function is not NULL.

Default values are respectively 0, 0, NULL and NULL.

#### void worker\_clear(worker\_t worker)

Request termination to the pool of workers, and wait for them to terminate.
It is undefined if there is any work order in progress.

#### void worker\_start(worker\_block\_t syncBlock, worker\_t worker)

Start a new synchronization block for a pool of work orders
linked to the pool of worker 'worker'.

#### void worker\_spawn(worker\_block\_t syncBlock, void (*func)(void *data), void *data)

Register the work order 'func(data)' to the the synchronization point 'syncBlock'.
If no worker is available (and no extraQueue), the work order 'func(data)' will be handled
by the caller. Otherwise the work order 'func(data)' will be handled
by an asynchronous worker and the function immediately returns.
The object(s) referenced by 'data' shall remain available (not destructed) until the
control flow reaches the next synchronization point (worker\_sync),
as theses object(s) may be delayed read by other threads until this point.

#### bool worker\_sync\_p(worker\_block\_t syncBlock)

Test if all work orders registered to this synchronization point are
terminated (true) or not (false). 

#### void worker\_sync(worker\_block\_t syncBlock)

Wait for all work orders registered to this synchronization point 'syncBlock'
to be terminated.

#### size\_t worker\_count(worker\_t worker)

Return the number of workers of the pool.

#### void worker\_flush(worker\_t worker)

Flush any work order in the queue by the current thread until none remains.

#### WORKER\_SPAWN(syncBlock, input, core, output)

Request the work order 'core' to the synchronization point syncBlock.
If no worker is available (and no extra queue), the work order 'core' will be handled
by the caller. Otherwise the work order 'core' will be handled
by an asynchronous worker.

'core' is any C code that doesn't break the control flow (you
cannot use return / goto / break to go outside the flow).
'input' is the list of local input variables of the 'core' block within "( )".
'output' is the list of local output variables of the 'core' block within "( )".
These lists shall be exhaustive to capture all needed variables.

This macro needs either GCC (for nested function) or CLANG (for blocks)
or a C++11 compiler (for lambda and functional) to work.

The next synchronization point (worker\_sync) shall be present in the control flow
of the current C block.

NOTE1: Even if nested functions are used for GCC, it doesn't generate
a trampoline and the stack doesn't need to be executable as all variables are captured by the library.

NOTE2: For CLANG, you need to add -fblocks to CFLAGS and -lBlocksRuntime to LIB (See CLANG manual).

NOTE3: It will generate warnings about shadowed variables. There is no way to avoid this.

NOTE4: arrays and not trivially movable object are not supported as input / output variables due to 
current technical limitations.

#### M\_WORKER\_SPAWN\_DEF2(name, (name1, type1, oplist1), ...)

Define a specialization of worker\_spawn, called m\_worker\_spawn ## name,
that takes as input a function with the given name and arguments,
avoiding casting the data to void*.
The specialized method shall have the following prototype:

    void function(type1 name1, type2 name2, ...);

m\_worker\_spawn ## name has the following prototypes.

    void m\_worker\_spawn ## name(m\_worker\_sync\_t block, void (*callback)(type1 name1, type2 name2, ...), type1 name1, type2 name2, ...);

The arguments are properly copied and cleared using their oplists if the work-order is enqueued for a worker. 


### M-ATOMIC

This header goal is to provide the C header 'stdatomic.h'
to any C compiler (C11 or C99 compliant) or C++ compiler.
If available, it uses the C11 header stdatomic.h,
otherwise if the compiler is a C++ compiler,
it uses the header 'atomic' and imports all definition
into the global name space (this is needed because the
C++ standard doesn't support officially the stdatomic header,
resulting in broken compilation when building C code with
a C++ compiler).

Otherwise it provides a non-thin emulation of atomic
using mutex. This emulation has a known theoretical limitation:
the mutex are never cleared. There is nothing to do
to fix this. In practice, it is not a problem.


### M-ALGO

This header is for generating generic algorithm to containers.

#### ALGO\_DEF(name, container\_oplist)

Define the available algorithms for the container which oplist is container\_oplist.
The defined algorithms depend on the availability of the methods of the containers
(For example, if there no CMP operator, there is no MIN method defined).

Example:

        ARRAY_DEF(array_int, int)
        ALGO_DEF(array_int, ARRAY_OPLIST(array_int))
        void f(void) {
                array_int_t l;
                array_int_init(l);
                for(int i = 0; i < 100; i++)
                        array_int_push_back (l, i);
                array_int_push_back (l, 17);
                assert( array_int_contains(l, 62) == true);
                assert( array_int_contains(l, -1) == false);
                assert( array_int_count(l, 17) == 2);
                array_int_clear(l);
        }

#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro that is used to identify the type.

In the following descriptions, it\_t is an iterator of the container
container\_t is the type of the container and type\_t is the type
of object contained in the container.

##### void name\_find(it\_t it, const container\_t c, const type\_t data)

Search for the first occurrence of 'data' within the container.
Update the iterator with the found position or return the end iterator. 
The search is linear.

##### void name\_find\_again(it\_t it, const type\_t data)

Search from the position 'it' for the next occurrence of 'data' within the container.
Update the iterator with the found position or return end iterator. 
The search is linear.

##### void name\_find\_if(it\_t it, const container\_t c, bool (*pred)(type\_t const))

Search for the first occurrence within the container than matches 
the predicate 'pred'
Update the iterator with the found position or return end iterator. 
The search is linear.

##### void name\_find\_again\_if(it\_t it, bool (*pred)(type\_t const))

Search from the position 'it' for the next occurrence matching the predicate
'pred' within the container.
Update the iterator with the found position or return end iterator. 
The search is linear.

##### void name\_find\_last(it\_t it, const container\_t c, const type\_t data)

Search for the last occurrence of 'data' within the container.
Update the iterator with the found position or return end iterator. 
The search is linear and can be backward or forwards depending
on the possibility of the container.

##### bool name\_contains(const container\_t c, const type\_t data)

Return true if 'data' is within the container, false otherwise.
The search is linear.

##### size\_t name\_count(const container\_t c, const type\_t data)

Return the number of occurrence of 'data' within the container.
The search is linear.

##### size\_t name\_count\_if(const container\_t c, bool (*pred)(type\_t const data))

Return the number of occurrence matching the predicate 'pred' within the container.
The search is linear.

##### void name\_mismatch(it\_t it1, it\_t it2, const container\_t c1, const container\_t c2)

Returns the first mismatching pair of elements of the two containers 'c1' and 'c2'.

##### void name\_mismatch\_again(it\_t it1, it\_t it2)

Returns the next mismatching pair of elements of the two containers 
from the position 'it1' of container 'c1' and 
from the position 'it2' of container 'c2'.

##### void name\_mismatch\_if(it\_t it1, it\_t it2, const container\_t c1, const container\_t c2, bool (*cmp)(type\_t const, type\_t const))

Returns the first mismatching pair of elements of the two containers using
the provided comparison 'cmp'.

##### void name\_mismatch\_again\_if(it\_t it1, it\_t it2, bool (*cmp)(type\_t const, type\_t const))

Returns the next mismatching pair of elements of the two containers using
the provided comparison 'cmp'
from the position 'it1' and
from the position 'it2'.

##### void name\_fill(container\_t c, type\_t value)

Set all elements of the container 'c' to 'value'.

##### void name\_fill\_n(container\_t c, size\_t n, type\_t value)

Set the container to 'n' elements equal to 'value'.
This method is defined only if the container exports a PUSH method.

##### void name\_fill\_a(container\_t c, type\_t value, type\_t inc)

Set all elements of the container 'c' to 'value + i * inc'
with i = 0.. size(c)
This method is defined only if the base type exports an ADD method.
This method is defined only if the container exports a PUSH method.

##### void name\_fill\_an(container\_t c, size\_t n, type\_t value)

Set the container to 'n' elements to 'value + i * inc'
with i = 0..n
This method is defined only if the base type exports an ADD method.
This method is defined only if the container exports a PUSH method.

##### void name\_for\_each(container\_t c, void (*func)(type\_t))

Apply the function 'func' to each element of the container 'c'.
The function may transform the element provided it doesn't reallocate the
object and if the container supports iterating over modifiable elements.

##### void name\_transform(container\_t d, container\_t c, void (*func)(type\_t *out, const type\_t in))

Apply the function 'func' to each element of the container 'c'
and push the result into the container 'd' so that 'd = func(c)'

'func' shall output in the initialized object 'out'
the transformed value of the constant object 'in'.
Afterwards 'out' is pushed moved into 'd'.

This method is defined only if the base type exports an INIT method.
This method is defined only if the container exports a PUSH\_MOVE method.
'c' and 'd' cannot be the same containers.

##### void name\_reduce(type\_t *dest, const container\_t c, void (*func)(type\_t *, type\_t const))

Perform a reduction using the function 'func' to the elements of the container 'c'.
The final result is stored in '*dest'.
If there is no element, '*dest' is let unmodified.

##### void name\_map\_reduce(type\_t *dest, const container\_t c, void (*redFunc)(type\_t *, type\_t const), void *(mapFunc)(type\_t *, type\_t const))

Perform a reduction using the function 'redFunc' 
to the transformed elements of the container 'c' using 'mapFunc'.
The final result is stored in '*dest'.
If there is no element, '*dest' is let unmodified.

##### bool name\_any\_of\_p(const container\_t c, void *(func)(const type\_t))

Test if any element of the container 'c' matches the predicate 'func'.

##### bool name\_all\_of\_p(const container\_t c, void *(func)(const type\_t))

Test if all elements of the container 'c' match the predicate 'func'.

##### bool name\_none\_of\_p(const container\_t c, void *(func)(const type\_t))

Test if no element of the container 'c' match the predicate 'func'.

##### type\_t *name\_min(const container\_t c)

Return a reference to the minimum element of the container 'c'.
Return NULL if there is no element.
This method is available if the CMP operator has been defined.

##### type\_t *name\_max(const container\_t c)

Return a reference to the maximum element of the container 'c'.
Return NULL if there is no element.
This method is available if the CMP operator has been defined.

##### void name\_minmax(type\_t **min, type\_t **max, const container\_t c)

Stores in '*min' a reference to the minimum element of the container 'c'.
Stores in '*max' a reference to the minimum element of the container 'c'.
Stores NULL if there is no element.
This method is available if the CMP operator has been defined.

##### void name\_uniq(container\_t c)

Assuming the container 'c' has been sorted, 
remove any duplicate elements of the container.
This method is available if the CMP and IT\_REMOVE operators have been defined.

##### void name\_remove\_val(container\_t c, type\_t val)

Remove all elements equal to 'val' of the container.
This method is available if the CMP and IT\_REMOVE operators have been defined.

##### void name\_remove\_if(container\_t c, bool (*func)(type\_t) )

Remove all elements matching the given condition (function func() returns true) of the container.
This method is available if the CMP and IT\_REMOVE operators have been defined.

##### void name\_add(container\_t dest, const container\_t value)

For each element of the container 'dest',
add the corresponding element of the container 'dest'
up to the minimum size of the containers.
This method is available if the ADD operator has been defined.

##### void name\_sub(container\_t dest, const container\_t value)

For each element of the container 'dest',
sub the corresponding element of the container 'dest'
up to the minimum size of the containers.
This method is available if the SUB operator has been defined.

##### void name\_mul(container\_t dest, const container\_t value)

For each element of the container 'dest',
mul the corresponding element of the container 'dest'
up to the minimum size of the containers.
This method is available if the MUL operator has been defined.

##### void name\_div(container\_t dest, const container\_t value)

For each element of the container 'dest',
div the corresponding element of the container 'dest'
up to the minimum size of the containers.
This method is available if the DIV operator has been defined.

##### bool void name\_sort\_p(const container\_t c)

Test if the container 'c' is sorted (=true) or not (=false).
This method is available if the CMP operator has been defined.

##### bool name\_sort\_dsc\_p(const container\_t c)

Test if the container 'c' is reverse sorted (=true) or not (=false)
This method is available if the CMP operator has been defined.

##### void void name\_sort(container\_t c)

Sort the container 'c'.
This method is available if the CMP operator has been defined.
The used algorithm depends on the available operators:
if a specialized SORT operator is defined for the container, it is used.
if SPLICE\_BACK and SPLICE\_AT operates are defined, a merge sort is defined,
if IT\_PREVIOUS is defined, an insertion sort is used,
otherwise a selection sort is used.

##### bool name\_sort\_dsc(const container\_t c)

Reverse sort the container 'c'.
This method is available if the CMP operator has been defined.
The used algorithm depends on the available operators:
if a specialized SORT operator is defined, it is used.
if SPLICE\_BACK and SPLICE\_AT operates are defined, a merge sort is defined,
if IT\_PREVIOUS is defined, an insertion sort is used,
otherwise a selection sort is used.

##### void name\_sort\_union(container\_t c1, const container\_t c2)

Assuming both containers 'c1' and 'c2' are sorted, 
perform an union of the containers in 'c1'.
This method is available if the IT\_INSERT operator is defined.

##### void name\_sort\_dsc\_union(container\_t c1, const container\_t c2)

Assuming both containers 'c1' and 'c2' are reverse sorted, 
perform an union of the containers in 'c2'.
This method is available if the IT\_INSERT operator is defined.

##### void name\_sort\_intersect(container\_t c1, const container\_t c2)

Assuming both containers 'c1' and 'c2' are sorted, 
perform an intersection of the containers in 'c1'.
This method is available if the IT\_REMOVE operator is defined.

##### void name\_sort\_dsc\_intersect(container\_t c, const container\_t c)

Assuming both containers 'c1' and 'c2' are reverse sorted, 
perform an intersection of the containers in 'c1'.
This method is available if the IT\_REMOVE operator is defined.

##### void name\_split(container\_t c, const string\_t str, const char sp)

Split the string 'str' around the character separator 'sp'
into a set of string in the container 'c'.
This method is defined if the base type of the container is a string\_t type,

##### void name\_join(string\_t dst, container\_t c, const string\_t str)

Join the string 'str' and all the strings of the container 'c' into 'dst'.
This method is defined if the base type of the container is a string\_t type,

#### ALGO\_FOR\_EACH(container, oplist, func[, arguments..])

Apply the function 'func' to each element of the container 'container' of oplist 'oplist' :
     
     for each item in container do
              func([arguments,] item)

The function 'func' is a method that takes as argument an object of the
container and returns nothing. It may update the object provided it
doesn't reallocate it.


#### ALGO\_TRANSFORM(contDst, contDstOplist, contSrc, contSrcOplist, func[, arguments..])

Apply the function 'func' 
to each element of the container 'contSrc' of oplist 'contSrcOplist' 
and store its output in the container 'contDst' of oplist 'contDstOplist'
so that 'contDst = func(contSrc)'. Exact algorithm is:
     
     clean(contDst)
     for each item in  do
         init(tmp)
              func(tmp, item, [, arguments])
         push_move(contDst, tmp)

The function 'func' is a method that takes 
as first argument the object to put in the new container,
and as second argument the object in the source container.


#### ALGO\_EXTRACT(containerDest, oplistDest, containerSrc, oplistSrc[, func[, arguments..]])

Extract the items of the container 'containerSrc' of oplist 'oplistSrc'
into the 'containerDest' of oplist 'oplistDest': 
     
     RESET (containerDest)
     for each item in containerSrc do
              [ if func([arguments,] item) ] 
                       Push item in containerDest

The optional function 'func' is a predicate that takes as argument an object of the
container and returns a boolean that is true if the object has to be added
to the other container.

Both containers shall either provide PUSH method, or SET\_KEY method.


#### ALGO\_REDUCE(dest, container, oplist, reduceFunc[, mapFunc[, arguments..])

Reduce the items of the container 'container' of oplist 'oplist'
into a single element by applying the reduce function:

     dest = reduceFunc(mapFunc(item[0]), reduceFunc(..., reduceFunc(mapFunc(item[N-2]), mapFunc(item[N-1]))))


'mapFunc' is a method which prototype is:
    
    void mapFunc(dest, item)

with both 'dest' & 'item' that are of the same type than the one of
the container. It transforms the 'item' into another form that is suitable
for the 'reduceFunc'.
If 'mapFunc' is not specified, identity will be used instead.

'reduceFunc' is a method which prototype is:
 
     void reduceFunc(dest, item)

It integrates the new 'item' into the partial 'sum' 'dest.

The reduce function can be the special keywords 'add', 'sum', 'and', 'or'
'product', 'mul' 
in which case the special function performing a sum/sum/and/or/mul/mul
operation will be used.

'dest' can be either the variable (in which case 'dest' is
assumed to be of type equivalent to the elements of the container)
or a tuple ('var', 'oplist') with 'var' being the variable name and 'oplist' its oplist
(with TYPE, INIT, SET methods). The tuple may be needed if the map function
transform a type into another.


#### ALGO\_INSERT\_AT(containerDst, containerDstOPLIST, position, containerSrc, containerSrcOPLIST)

Insert into the container 'contDst' at position 'position' all the values
of container 'contSrc'.



### M-FUNCOBJ

This header is for generating Function Object. 
A [function object](https://en.wikipedia.org/wiki/Function_object)
is a construct an object to be invoked 
or called as if it were an ordinary function, usually with the same 
syntax (a function parameter that can also be a function)
but with additional "within" parameters.

Example:

        // Define generic interface of a function int --> int
        FUNC_OBJ_ITF_DEF(interface, int, int)

        // Define one instance of such interface
        FUNC_OBJ_INS_DEF(sumint, interface, x, {
                return self->sum + x;
        }, (sum, int)   )

        int f(interface_t i)
        {
                return interface_call(i, 4);
        }
        int h(void)
        {
                sumint_t s;
                sumint_init_with(s, 16);
                int n = f(sumint_as_interface(s));
                printf("sum=%d\n", n);
                sumint_clear(s);
        }


#### FUNC\_OBJ\_ITF\_DEF(name, retcode\_type[, type\_of\_param1, type\_of\_param 2, ...])
#### FUNC\_OBJ\_ITF\_DEF\_AS(name, name\_t, retcode\_type[, type\_of\_param1, type\_of\_param 2, ...])

Define a function object interface of name 'name' 
emulating a function pointer returning retcode\_type (which can be void),
and with as inputs the list of types of paramN, thus generating a function
prototype like this:

        retcode_type function(type_of_param1, type_of_param 2, ...)

An interface cannot be used without an instance (see below)
that implements this interface. In particular, there is no init
nor clear function for an interface (only an instance provides such
initialization).

FUNC\_OBJ\_ITF\_DEF\_AS is the same as FUNC\_OBJ\_ITF\_DEF except the name of the type name\_t is provided.

It will define the following type and functions:

##### name\_t

Type representing an interface to such function object. 
There is only one method for such type (see below).

##### retcode\_type name\_call(name\_t interface, type\_of\_param1, type\_of\_param 2, ...)

The call function of the interface object.
It will call the particular implemented callback of the instance of this interface.
It shall only be used by an interface object derived from an instance.


#### FUNC\_OBJ\_INS\_DEF(name, interface\_name, (param\_name\_1, ...), { callback\_core }, (self\_member1, self\_type1[, self\_oplist1]), ...)
#### FUNC\_OBJ\_INS\_DEF\_AS(name, name\_t, interface\_name, (param\_name\_1, ...), { callback\_core }, (self\_member1, self\_type1[, self\_oplist1]), ...)

Define a function object instance of name 'name' 
implementing the interface 'interface\_name' (it is the same as 
used as name in FUNC\_OBJ\_ITF\_DEF).

The function instance is defined as per :

- the function prototype of the inherited interface,
- the parameters of the function are named as per the list param\_name\_list,
- the core of the function shall be defined in the brackets 
  within the callback\_core. The members of the function object can be
  accessed through the pointer named 'self' to access the member 
  attributes of the object (without any cast), 
  and the parameter names of the function
  shall be accessed as per their names in the param\_name\_list.
- optional member attributes of the function object can be defined 
  after the core (just like for tuple & variant):
  Each parameter is defined as a triplet: (name, type [, oplist])

It generates a function that looks like:

        interface_name_retcode_type function(interface_name_t *self, interface_name_type_of_param1 param_name_1, interface_name_type_of_param 2 param_name_2, ...) {
               callback_core
        }

FUNC\_OBJ\_INS\_DEF\_AS is the same as FUNC\_OBJ\_INS\_DEF except the name of the type name\_t is provided.

##### name\_t

Name of a particular instance to the interface of the Function Object interface\_name.

##### void name\_init(name\_t self)

Initialize the instance of the function with default value.
This method is defined only if all member attributes export an INIT method.
If there is no member, the method is defined.

##### void name\_init\_with(name\_t self, self\_type1 a1, self\_type2 a2, ...)

Initialize the instance of the function with the given values of the member attributes.

##### void name\_clear(name\_t self)

Clear the instance of the function.

##### interface\_name\_t name\_as\_interface(name\_t self)

Return the interface object derived from this instance.
This object can then be transmitted to any function
that accept the generic interface (mainly \_call).


### M-TRY

This header is for [exception handling](https://en.wikipedia.org/wiki/Exception_handling).
It provides basic functionality for throwing exception and catching then.
The setjmp and longjmp standard library functions (or some variants) are used to implement the try / catch / throw macro keywords.
It doesn't support the finally keyword.
When building with a C++ compiler, theses macro keywords simply use the original C++ keyword in a way to match
the specification below.

The whole program shall be compiled with the same exact compiler and the same target architecture.

Only one type of data is supported as exception. This is done to simplify the design and to avoid using exception
as a general purpose error mecanism. It should only be used for rare case of errors which cannot be dealt locally
in the program being executed.

In order to support [Resource Acquisition Is Initialization](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)
technique which frees resources using [destructor](https://en.wikipedia.org/wiki/Destructor_(computer_programming)),
M\*LIB ensures that the destructors of all variables created using the keyword M\_LET are properly called on throwing exception.
For this, different mechanisms are used to mark the objects and the CLEAR operator to be called on abnormal exceptions.
This is done by injecting different information in the stack to be able to handle then.
Therefore, contrary to C++, using exceptions in C will add a penalty performance to the program being executed in its nominal behavior.
The exact cost depends on the mechanism used for RAII support.
For GCC, it will use nested functions. For CLANG, it will use blocks (if compiled in blocks mode).
Otherwise it uses a standard compliant way, which is the slowest.
The variable which are not initialized through the macro M\_LET don't have their CLEAR method called on exceptions.

A typical program will look like:

    M_TRY(exception) {
      M_LET(x, string_t) {
        // ... do operation on x
        // Throw a memory error of 1024 bytes.
        M_THROW(M_ERROR_MEMORY, 1024);
      }
    } M_CATCH(exception, M_ERROR_MEMORY) {
      printf("There is no enough memory to allocate %zu bytes.\n", (size_t) exception->data[0]);
    }

In this example, the destructor of 'x' will be called before catching the exception and resuming
the program execution in the M\_CATCH block.

You shall include this header before any other headers of M\*LIB, so that 
it can configure the memory management of M\*LIB to throw exception on memory errors.
It does it by redefining the default macro M\_MEMORY\_FULL accordingly.

When using CLANG, you should add the following options to your compiler flags,
otherwise it will compile in degraded mode:

    -fblocks -lBlocksRuntime

When writing your own constructor, you should consider M\_CHAIN\_INIT
to support partially constructed object
if there are more than two source of throwing in your object
(any memory allocation is a source of throwing).

#### struct m\_exception\_s

This is the exception type. It is composed of the following fields:

* error\_code: The error code. It is used to identify the error that are raised the exception.
* line: The line number where the error was detected.
* filename: The filename (CSTR) where the error was detected.
* num: Number of entries in 'context' table
* context: an array of M\_USE\_MAX\_CONTEXT elements.

You can access the fields of the type directly.

#### M\_TRY(name)

Create a try block which name is 'name' that will catch thrown exception (if any)
and forward then to the associated catch block of the same name.
The try block will start just after the keyword, with a brace character.
If no catch block matches the error code raised, it will forward the error to the upper level try block.
Until there is no longer any try block. In which case, it will terminate the program.

#### M\_CATCH(name, error\_code)

Create a catch block associated to the try block named 'name'.
The catch block will start just after the keyword, with a brace character.
This catch block will be executed on throwing an exception with an error code matching error\_code.
If error\_code is 0, it will catch all exceptions.
Within the catch block, a pointer to the exception object of type m\_exception\_s is available through the 'name' variable.

#### M\_THROW(error\_code[, ...])

Throw the exception associated to the error\_code.
error\_code shall be a positive integer constant.
Additional arguments are used to fill in the error field of m\_exception\_s
that is used to identify the cause of the exception.
The line and filename fields of the exception are filled automatically by the macro.


### M-MEMPOOL

This header is for generating specialized and optimized memory pools:
it will generate specialized functions to allocate and free only one kind of an object.
The mempool functions are not specially thread safe for a given mempool,
but the mempool variable can have the attribute M\_THREAD\_ATTR
so that each thread has its own instance of the mempool.

The memory pool has to be initialized and cleared like any other variable.
Clearing the memory pool will free all the memory that has been allocated 
within this memory pool.


#### MEMPOOL\_DEF(name, type)

Generate specialized functions & types prefixed by 'name' to alloc & free an object of type 'type'.

Example:

        MEMPOOL_DEF(mempool_uint, unsigned int)

        mempool_uint_t m;

        void f(void) {
          mempool_uint_init(m);
          unsigned int *p = mempool_uint_alloc(m);
          *p = 17;
          mempool_uint_free(m, p);
          mempool_uint_clear(m);
        }

#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro that is used to identify the type.

##### name\_t

The type of a mempool.

##### void name\_init(name\_t m)

Initialize the mempool 'm'.

##### void name\_clear(name\_t m)

Clear the mempool 'm'.
All allocated objects associated to the this mempool that weren't explicitly freed
will be deleted too (without calling their clear method).

##### type *name\_alloc(name\_t m)

Create a new object of type 'type' and return a new pointer to the uninitialized object.

##### void name\_free(name\_t m, type *p)

Free the object 'p' created by the call to name\_alloc.
The clear method of the type is not called.



### M-SERIAL-JSON

This header is for defining an instance  of the serial interface
supporting import (and export) of a container
from (to) to a file in [JSON](https://en.wikipedia.org/wiki/JSON) format.
It uses the generic serialization ability of M\*LIB for this purpose,
providing a specialization of the serialization for JSON over FILE*.

Another way of seeing it is that you define your data structure 
using M\*LIB containers (building it using basic types, strings,
tuples, variants, array, dictionaries, ...) and then you
can import / export your data structure for free in JSON format.

If the JSON file cannot be translated into the data structure, a failure
error is reported (M\_SERIAL\_FAIL). For example, if some new fields are present
in the JSON file but not in the data structure.
On contrary, if some fields are missing (or in a different order) in the JSON
file, the parsing will still succeed (object fields are unmodified
except for new sub-objects, for which default value are used).

It is fully working with C11 compilers only.

The current locale of the program shall be compatible with the JSON format,
specially the fraction separator character shall be '.' in the current
locale to respect the JSON format.

#### C functions on FILE

##### m\_serial\_json\_write\_t

A synonym of m\_serial\_write\_t with a global oplist registered
for use with JSON over FILE*.

##### void m\_serial\_json\_write\_init(m\_serial\_write\_t serial, FILE *f)

Initialize the 'serial' object to be able to output in JSON format to the file 'f'.
The file 'f' shall remain open in 'wt' mode while the 'serial' is not cleared.

##### void m\_serial\_json\_write\_clear(m\_serial\_write\_t serial)

Clear the serialization object 'serial'.

##### m\_serial\_json\_read\_t

A synonym of m\_serial\_read\_t with a global oplist registered
for use with JSON over FILE *.

##### void m\_serial\_json\_read\_init(m\_serial\_read\_t serial, FILE *f)

Initialize the 'serial' object to be able to parse in JSON format from the file 'f'.
The file 'f' shall remain open in 'rt' mode while the 'serial' is not cleared.

##### void m\_serial\_json\_read\_clear(m\_serial\_read\_t serial)

Clear the serialization object 'serial'.

#### C functions on string

##### m\_serial\_str\_json\_write\_t

A synonym of m\_serial\_write\_t with a global oplist registered
for use with JSON over string\_t.

##### void m\_serial\_str\_json\_write\_init(m\_serial\_write\_t serial, string\_t str)

Initialize the 'serial' object to be able to output in JSON format to the string\_t 'str'.
The string 'str' shall remain initialized while the 'serial' object is not cleared.

##### void m\_serial\_str\_json\_write\_clear(m\_serial\_write\_t serial)

Clear the serialization object 'serial'.

##### m\_serial\_str\_json\_read\_t

A synonym of m\_serial\_read\_t with a global oplist registered
for use with JSON over const string (const char*).

##### void m\_serial\_str\_json\_read\_init(m\_serial\_read\_t serial, const char str[])

Initialize the 'serial' object to be able to parse in JSON format from the const string 'str'.
The const string 'str' shall remain initialized while the 'serial' object is not cleared.

##### const char * m\_serial\_str\_json\_read\_clear(m\_serial\_read\_t serial)

Clear the serialization object 'serial' and return a pointer to the first
unparsed character in the const string.

Example:

        // Define a structure of two fields.
        TUPLE_DEF2(my,
                   (value, int),
                   (name, string_t)
                   )
        #define M_OPL_my_t() TUPLE_OPLIST(my, M_BASIC_OPLIST, STRING_OPLIST )
        
        // Output in JSON file the structure my_t
        void output(my_t el1)
        {
          m_serial_write_t out;
          m_serial_return_code_t ret;
        
          FILE *f = fopen ("data.json", "wt");
          if (!f) abort();
          m_serial_json_write_init(out, f);
          ret = my2_out_serial(out, el1);
          assert (ret == M_SERIAL_OK_DONE);
          m_serial_json_write_clear(out);
          fclose(f);
        }
        
        // Get from JSON file the structure my_t
        void input(my_t el1)
        {
          m_serial_read_t  in;
          m_serial_return_code_t ret;
        
          f = fopen ("data.json", "rt");
          if (!f) abort();
          m_serial_json_read_init(in, f);
          ret = my2_in_serial(el2, in);
          assert (ret == M_SERIAL_OK_DONE);
          m_serial_json_read_clear(in);
          fclose(f);
        }
        

### M-SERIAL-BIN

This header is for defining an instance of the serial interface
supporting import (and export) of a container
from (to) to a file in an ad-hoc binary format.
This format only supports the current system and cannot be used to communicate 
across multiple systems (endianess, size of types are typically not abstracted
by this format).

It uses the generic serialization ability of M\*LIB for this purpose,
providing a specialization of the serialization for JSON over FILE*.

It is fully working with C11 compilers only.

#### C functions

##### void m\_serial\_bin\_write\_init(m\_serial\_write\_t serial, FILE *f)

Initialize the 'serial' object to be able to output in BIN format to the file 'f'.
The file 'f' has to remained open in 'wb' mode while the 'serial' is not cleared
otherwise the behavior of the object is undefined.

##### void m\_serial\_bin\_write\_clear(m\_serial\_write\_t serial)

Clear the serialization object 'serial'.

##### void m\_serial\_bin\_read\_init(m\_serial\_read\_t serial, FILE *f)

Initialize the 'serial' object to be able to parse in BIN format from the file 'f'.
The file 'f' has to remained open in 'rb' mode while the 'serial' is not cleared
otherwise the behavior of the object is undefined.

##### void m\_serial\_bin\_read\_clear(m\_serial\_read\_t serial)

Clear the serialization object 'serial'.


## Global User Customization

The behavior of M\*LIB can be customized globally by defining the following macros
before including any headers of M\*LIB.
If a macro is not defined before including any M\*LIB header,
the default value will be used.

Theses macros shall not be defined after including any M\*LIB header.

#### M\_USE\_UNDEF\_ATOMIC

Undefine the macro _Atomic in m-atomic.h if stdatomic.h is included.
It is needed on MSYS2 due to a bug in their headers which is not fixed yet. 

Default value: 1 (undef) on MSYS2, 0 otherwise.

#### M\_USE\_STDIO

This macro indicates if the system header stdio.h shall be included
and the FILE functions be defined (=1) or not (=0).
If it is not included, the macro M\_RAISE\_FATAL shall be defined by the user.

Default value: 1

#### M\_USE\_STDARG

This macro indicates if the system header stdarg.h shall be included
and the va\_args functions be defined (=1) or not (=0).

Default value: 1 (true)

#### M\_USE\_SMALL\_NAME

This macro indicates if the small names (without the m\_ prefix)
have to be defined or not. Historically, only the small name API
existed, so in order to keep API compatibility, the default is true.

Default value: 1

#### M\_USE\_CSTR\_ALLOC

Define the allocation size of the temporary strings created by M\_CSTR
(including the final null char).

Default value: 256.

#### M\_USE\_IDENTIFIER\_ALLOC

Define the allocation size of a C identifier in the source code
(excluding the final nil char).
It is used to represent a C identifier by a C string.

Default value: 128

#### M\_USE\_THREAD\_BACKEND

Define the thread backend to use by m-thread.h:

* 1: for C11 header threads.h
* 2: for WINDOWS header windows.h
* 3: for POSIX THREAD header pthread.h

Default value: auto-detect in function of the running system.

#### M\_USE\_WORKER

This macro indicates if the multi-thread code of m-worker.h shall be used (=1) or not (=0)
- In this case, a single-thread code is used -.

Default value: 1

#### M\_USE\_WORKER\_CLANG\_BLOCK

This macro indicates if the workers shall use the CLANG block extension (=1) or not (=0).

Default value: 1 (on clang), 0 (otherwise)

#### M\_USE\_WORKER\_CPP\_FUNCTION

This macro indicates if the workers shall use the C++ lambda function (=1) or not (=0).

Default value: 1 (compiled in C++), 0 (otherwise)

#### M\_USE\_BACKOFF\_MAX\_COUNT

Define the maximum iteration of the BACKOFF exponential scheme
for the synchronization waiting loop of multi-threading code.

Default value: 6

#### M\_USE\_SERIAL\_MAX\_DATA\_SIZE

Define the size of the private data (reserved to the serial implementation) in a serial object
(as a number of pointers or equivalent objects).

Default value: 4

#### M\_USE\_MEMPOOL\_MAX\_PER\_SEGMENT(type)

Define the number of elements to allocate in a segment per object of type 'type'.

Default value: number of elements that fits in a 16KB page.

#### M\_USE\_DEQUE\_DEFAULT\_SIZE

Define the default size of a segment for a deque structure.

Default value: 8 elements.

#### M\_USE\_HASH\_SEED

Define the seed to inject to the hash computation of an object.

Default value: 0 (predictable hash)

#### M\_USE\_FAST\_STRING\_CONV

Use fast integer conversion algorithms instead of using the LIBC.

Default value: 1 (because it also generates smaller code).

#### M\_USE\_DECL

If M\_USE\_FINE\_GRAINED\_LINKAGE is not defined,
it will request M\*LIB to change the linkage of its symbols globally:
instead of inlining the functions, it will emit weak symbols
for the functions that are not inlined.

And in exactly one translation unit, the macro M\_USE\_DEF
should also be defined so that it emits the normal definition
of the functions. In which case, it should contain all symbols
used by all other translation units.

You should compile your program with 
-ffunction-sections -fdata-sections 
and link with
-Wl,--gc-sections
in order to remove unused code and to merge identical code
otherwise it is likely to generate even bigger code than using the inlining linkage.

This works for GCC / CLANG in C mode.

#### M\_USE\_FINE\_GRAINED\_LINKAGE

It will request M\*LIB to change the linkage of its symbols dynamically at compile time:
in which case
M\_USE\_DECL / M\_USE\_DEF can be defined / undefined several times in a translation unit
and M\*LIB will emit the change the linkage of the symbols being declared accordingly:

* definition like if M\_USE\_DECL and M\_USE\_DEF are defined
* declaration like if M\_USE\_DECL is defined
* inline like otherwise

This enables to inline some functions and not others.

M\_USE\_DECL / M\_USE\_DEF shall be defined without effective value.

See M\_USE\_DECL for more details.


#### M\_MEMORY\_ALLOC

See [m-core.h](#m-core)

#### M\_MEMORY\_REALLOC

See [m-core.h](#m-core)

#### M\_MEMORY\_FULL

See [m-core.h](#m-core)

#### M\_RAISE\_FATAL

See [m-core.h](#m-core)

#### M\_ASSERT

See [m-core.h](#m-core)

#### M\_ASSERT\_SLOW

See [m-core.h](#m-core)

#### M\_ASSERT\_INIT

See [m-core.h](#m-core)

#### M\_ASSERT\_INDEX

See [m-core.h](#m-core)


## License

All files of M\*LIB are distributed under the following license.

Copyright (c) 2017-2023, Patrick Pelissier
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

+ Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
+ Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
