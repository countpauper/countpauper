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

# Documentation 

TODO: link to a README for each utility 

# Ideas

## Errors 
`expectation` is great and all but the posix error codes are a bit limiting. Instead a more structured type can be used, which also uses the other bits that are likely available in the expected anyway due to alignment.
Each error is a struct { ErrorCategory category; Component source; uint16_t code; } 
Each ErrorCategory is identified with an 8 bit code as well. Error categories can be registered 
and provide functions to convert to string or provide some level of severity maybe. The Error 
can of course also be converted to a single 32bit code.
Components can also be registered and might be linked to logging categories. 

## Identifier 

Status: Identifier was split off from event. Needs 
1. Unit tests
1. Determine if copying should generate new IDs (and moving not). Sounds logical, but if done, state machine unit tests break.

State machine events, error categories and components and perhaps more need to have unique identifiers to recognize 
an instance/type. For global types the pointer to the single instance can identify it. For instances that may be created 
on the stack, an Identifier helper could hand out unique IDs. 
The underlying size of the ID could be templated  This can also help reduce the memory footprint, for instance for 
error caterogies to one byte.  
The `event` type currently has the id built in. This could be generalized. Of course it should also be tested 
that IDs don't run out. 
IDs themselves should not be copyable. If you want to copy an object with an ID then it should create a new ID for itself  

## Logging 
The logging from Angel could be reused. Perhaps as C++ wrappers around C logging.
It could add source categories which are automatically added to the log, alongside the level and time 

## Source categories 
Both errors and logging could benefit from some unit/component registration system. This may be 
per compilation unit (and leverage `__FILE__`, perhaps hashed). Either way it might need some registry. 
A null category should be available as zero cost default for error and logging (0 bytes administration and 0 bytes logged)

## Enum/Error mapping

A very common pattern is that one type of enum or error has to be converted to another in another abstraction layer, for decoupling (eg HAL to abstract application interface). This usually ends up
looking like some sort of lookup table (if not sparse) or switch case and often some sort of defualt is needed because the lower abstraction level may extend the enum/error codes.

It would be nice if this could be generated with templates. Preferably a lookup table would be usable instead of iterating over all the options to match. 

template<typename EnumA, typename EnumB>
EnumB value_map(EnumA)  sets up the basic type and provides the runtime mapping function and then 
map_value<FirstValue, MatchingValue> could be registered based on the decltype of FirstValue and MatchingValue  