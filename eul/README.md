# Embedded Utilities Library 

## Purpose 

This is a library of loosely (and sometimes closely) related utilities. These can be: 
1) Containers
1) Mathematical or logical functions 
1) Design patterns
1) Algorithms 


## Requirements for suitability 
These utilities must be suitable for use in C++ embedded software projects. 
They are suitable if: 
- They do not use heap allocation by default (opt-in allowed)
- They do not generate excessive amounts of code through their templates
- They do not require generic runtime type information 
- They do not throw exceptions (debatable) but use std::expected to handle unhappy flow 

## Out of scope 

Containers or algorithms that have equivalent functionality in the STL, which are already suitable 
do not qualify. 

## Language 
The C++ standard is C++23 or higher. 

# Style 

## Naming 
The style tries to match the C++ standard template library as much as possible. That means: 
* snake_style naming for classes, functions, variables
* same name for the file name as for the main unit in that file 
* private member variables prefixed with `_` to reduce name clashes
* CamelCase for template arguments

## Namespace 
Everything should be in the `eul` namespace. Tests should be in the `eul::test` namespace and 
helpers that are not intended for the user of the library in the `eul::internal` namespace. 

## Container interfaces  
All containers should be iterable and the iterators should meet at least one [iterator concept](https://en.cppreference.com/w/cpp/header/iterator.html). They should also 
- Have size() and empty() const getters
- Preferably have an initialize_list constructor 
- Preferably be movable 



