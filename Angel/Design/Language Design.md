# Goal 
Angel is a programming language that combines the best of: 
prolog: declarative logic, solving, matching
python: ranges, preparsing into 
functional: all user defined functions are pure, higher order functions 
The goal is to make a scientific programming language that can model the knowledge of a certain 
domain and reason to answer queries. 

# Use Cases 
The use cases below do not yet form a coherent and complete specification of the language. 
They explore some of the options to look for opportunities and complications. More of these 
will emerge during development and be added here or directly as unit tests. These unit 
tests will form the true specification of the language.  

## Axiomatic predicates
an axiom is considered true if it is contained in the knowledge
```
cat
cat? 
> true 
mouse?
> false
```
An axiom it added to the list of "facts" of the namespace.
It is equivalent to `cat: true`, any non defined axiom is `false` 

## functional predicates
A function is true if its label and all arguments match, else it is false
```
cat(ginny) 
cat(ginny)?
> true
cat(gizmo)?
> false
```

## functions with valance>1
A function can have multiple arguments, separates by the collection operator `,`. 
```
color(car, blue)
color(car, green)? 
> false 
color(tree, blue)>
> false 
```

Technically each function has a list of arguments of a size 0 or more. An axiomatic, so 
`cat()` is equivalent to `cat`.

## Clause
a predicate is true if it is implied and its antecedent is true. The predicate implied is 
separated from it's condition with the `:` operator. This means the implication is true if the 
conditions are true. 
New knowledge (the set of statements that are true) can be infered from this during a query. 
If the implication MATCHES the query, the query is recursively replaced by the condition. 
If the condition is a conjunction, each element will be queried in turn until the entire query 
returns a value. If that value is true, the original implication will also be true. If the query 
is false, then another clause may that matches the original query will be tried until none matches. 

```
cat(ginny) : hairy(ginny)
hairy(ginny)
cat(ginny)?
...hairy(ginny)?  true 
> true
```

## Clauses with variable arguments 
These define a generic case. The variables have the scope of that clause. When matching the query, 
the equivalence is added to the hypothesis and while matching the other predicates in that clause 
it will be used to instantiate variable predicates.
```
cat(X): hairy(X)
hairy(ginny)
cat(ginny)?
... X=ginny 
... hairy(ginny)?
true
> true
```


## Query 
Queries are predicates postfixed with the `?` operator. These queries start inference. 
It will try to match that predicate until it is proven evaluate to `true`, else `false` 
The query set can expand or change when matching closes until resolved, but as soon as the list of 
knowledge evaluates to a value (true, false, an integer or a collection of such) inference is complete. 

```
cat? 
> true 
```

## Variable query
If the query contains one or more variable, the inference engine returns the value(s) of each (or combination) 
for which the query is true.  
```
cat(ginny)
cat(X)? 
> X = ginny 
```
Each variable in a query has scope of the query. If the query evaluates to true  
then these variables will have collected ALL the conditions under which that constant is true 
TODO: is there a point if the result is not a boolean? what if it's a falsey value like 0? 

If the query has multiple variables, there may be multiple combinations of values for which the query is true. 

## Variable predicates

Seemingly have no purpose, perhaps with predicates. Mostly they seem harmful 
because technically they would make everything true. 
TODO: warning? or ignore

```
X 
X(ginny): X=cat 
```

## Namespace 
A namespace is a *set* of predicates, so their order does not matter but they are unique. 
By default each axiom is added to the root namespace, but to separate domains of knowledge,
while still allowing them to refer to each other, a namespace can be wrapped around this set 
```music: 
{
   composer(bach)
   instrument(violin)
}
```
Within the namespace, predicates can be matched. If the predicate is not found the parent namespace is 
checked, until the root namespace. Predicates across namespace can be matched by using the `.` operator 
to navigate to a namespace. 
```
music.composer(X)?
> X = bach
```

## Integers 
integers match if they are evaluate to equal
```
cat(ginny): legs(4)
legs(4)
cat(ginny)?
> true
```

## Conjunction 
A conjunection if expressions is true if all its elements are true. The & operator is used for conjunection. A conjunection is false as soon as one element is false. This is avaluated in a lazy way for performance and for axioms with side effects 
axioms with side effects 
```
cat(ginny): hairy(ginny) & noisy(ginny)
hairy(ginny)
noisy(ginny)
cat(ginny)?
> true
```

## Disjunction 
A disjunction is true if at least one of its elements is true. 
The `|` operator is used for disjunction and and lazy evaluation continues inference as soon as the first element of the 
disjunction is found to be true.
```
cat(ginny): noisy(ginny) | shy(ginny)
shy(ginny)
cat(ginny)?
> true
```

### Conjunction axioms 

Since the set of axioms in a namespace is already the conjunction of all true axioms, 
using a conjunction on the left side in a Horn clause is easily redefined as individual Horn clauses. 
For efficiency this can done after parsing, but for debugging traceability should remain to the original source 
and its location in the source file.  
```
cat(ginny) & cat(gizmo) 
cat(ginny)?
> true 
```
This is a valid use case, because it can be used to avoid code duplication for shared conditions in a clause.
```
cat(X) & fuzball(X) : hairy(X) 
```
converted to: 
```
cat(X): hairy(X)
fuzball(x): hairy(X)
```

### Disjunction axioms

TODO:  unlike conjunction axioms, it's not clear how to convery this to horn clauses (unlike conjunection clauses).  
If it was `xor` then each one could be converted to a horn clause by postulating that the other elements of the 
disjunction must then be false. There is no xor operator defined yet, but for this example assume it is `||` 
```
cat(X) || dog(X) : hairy(X)
```
converted to 
```
cat(X): hairy(X) & ~dog(X)
dog(X): hairy(X) & ~cat(X) 
```

More complex compinations of conjunections, disjunctions and negations could be implied 
and a pretty solid truth table/logic solver may already be needed to convert them to horn clauses
This is why this is lower priority and at first only horn clauses are allowed.
When converting to horn clauses, for debugging, some traceability to the original clause and from
there to the source location is useful. 


## Negations 
The `~` operator is used. 
```
cat(x): ~dog(X) 
```

## Negation predicates
Axioms that are negated are just the same as being defined as `false`. There is not much of a use case for this
because undefined predicates are by default assumed to be false. 
```
~cat(rover)
```
Clauses with a negated conclusion can be written as well. Either way this is dangerous for the horn clauses 
because paradoxes can be written this way that can only be "resolved" by prioritizing earliest matches.

```
cat(ginny)
hairy(ginny)
~cat(X): hairy(X)
cat(ginny)?
> error 
```

## Non-boolean values 
So far all functions have returned a boolean type value, true or false. Functions can also return non boolean values of 
different types: 
* integers (always signed 64 bit)
* floating point (alwasys double precision)
* strings 
* collections of other types, such as list or set. 
All they have to do is to define a clause that itself returns a non boolean value at some point. 
```
f(X): X+2
pi: 3.141592
```

## Converting number to and from boolean. 
Functional integer and floating point functions can be mixed into the boolean inference engine using comparison operators. 
There is no implicit conversion, to avoid inference erros. 
```
greater(X,Y): X>Y
equal(X,Y): X=Y
```

## Integer and floating point operators 
Of course all basic integer and floating point operators `+-*/^` are supported, as well as negation. These of course obey PEMDAS 
and if either of the operands is floating point then the other operand is cast to floating point as part of the operation 
``` 
f(X): X-0.5
f(3)?
> 2.5
```

## built in integer and floating point operators 
* `float(X)` - casts a value to float whether it is boolean, integer, already float or string. 
* `int(x)` - casts a value to an integer whether it is boolean, float, already integer or string. 
* `bool(X)` - casts a value to a boolean, including collections if they are not empty.
* `str(X)` - converts the value to a string in such a form that it would convert back with the functions above.
More functions may be imported as a math module, see below.

## Collection operators 

Collections support operators as well. If either the left or the right hand side is singular and the other is not 
it is upgraded to a single element set. 
* `@` - element off, converts to True if the left operarand is part of (if singular) or a subset of (if a collection) of the right operator 
* `&` - disjunction: the collection, with the elements that are both in the left and the right hand side. 
* `|` - conjunection: the collection  with the elements that are either in the left or the right hand side 
* `+` - a collection with the elments of the right hand side added to the list of the elements of the left hand side 
* `-` - subtraction:  the collection, with the elements of the left hand side that are NOT in the right hand side. 
The type of the resulting collection is the same as the type of the left hand side, so eg subtracting a list from a 
set results in a set, but subtracting a set from a list results in a list.

## built in collection functions 
* `empty(X)` - returns true if the collection is 0 elements
* `size(X)` - returns the number of elements in the collection 
* `set(X)` - Create a set of the collection or element X
* `list(X)` - Create a list of the collection or element X 
* `iterable(X)` - Return true if X is iterable (sets, strings, and sequences)
* `indexed(X)` - Return true if X is indexed (strings, sequences)
These could also be in an optionally imported `colllection` library to reduce name clashes 

## String operators 
Strings are a lot like lists of characters, except that conjunctions and disjunctions make little sense. 
* `@` - if the left hand substring is in the right hand, it returns true 
* `+` - concatenates two strings
* `-` - removes all characters of the right hand side from the string.
Slicing can be used to access sub strings or individual characters. 

## String functions 
A string module could add other common string functions:
* `upper(X)` and `lower(X)`
* `isupper(X)` and `islower(X)`
* `strip(X, Y)` 
* `replace(X,Y)` 
* `format(X,[args])`
* `regex(x, expr)`
* `wildcard(W, M)` 

## Summation
A summation evaluates to the sum of its elements
```
cat(ginny) : legs(4) 
legs(2+2)
cat(ginny)?
> true
```

## Equations are true if the equation is true
```
cat(X) : legs(X,Y), Y=4
legs(ginny, 4)
cat(ginny)?
> true
```

## Equations can be inequal. they are true if the value is in range
Inequal equations 
```
cat(X) : legs(X,Y) Y>3
legs(ginny,4)
cat(ginny)?
> true
```
## Query variables 
```
cat(ginny)
cat(X)?
X=ginny
```
How it works is `cat(X)` **hypothesizes** `X=ginny`. This hypothesis remains until there is a contradiction, in which 
case it backtracks like prolog and tries another hypothesis if one is available.


## Sequences = ordered non-unique collection 
you can use the @ operator to check membership of a collection 
```
cat(X) : X @ [ginny,max])
cat(ginny)?
> true
```
How it works: hypothesize `X=ginny`, `ginny @ {ginny,max}` is true because ginny is an element of that collection. 

## Indexing 
When using an integer index on an ordered collection a single element is used. Unordered collections like sets support 
this by first being converted to a list.
```
[1,2,3][2] 
> 3
```

## Sequence indexing
```
[max,ginny,gizmo][0,2]?
> [max,gizmo]
```

## Slicing 
When using a range index, multiple elements are used
```
[1,2,3][1:2]
> [2,3]
```
## Infinite range 
When not specifying a value for a range index, that side is infinite. Any slice is clipped to the size of the collection, 
so infinite slices to the end. 
```
[1,2,3][1:]
> [2,3]
```

## Exclusive range
`< >` brackets are used for exclusive. This is less important, but it is needed for the equivalance of `X>3 = X@<3:> 
```
[1,2,3][1,2>
> [2]
```
## @ operator also works on ranges
```
2 @ [2:]
> true
```

## The boolean math is very prolog like so far, but  
Clauses (functions) don't just return boolean 
``` 
f(X): X+2
f(1)?
> 3
```

 
## axioms are used as variable assignment with that 
Equivalence operator `=` is not used because legs = 4, but 4 is not equivalent to legs. 
```
legs: 4
cat(X): legs>=3
cat(ginny)?
> true
```

## id axiomas are like enums 
```
cat: ginny 
cat ? 
> ginny 
```
but it would be hard to not try to match ginny and fail and decide she's not a cat.


## Sequence/range matching 
```
[X,Y,Z] = [max, ginny, gizmo]
> X=max & Y=ginny & Z=gizmo 
```
(not sure if the assumptions should take the form of a conjunction or a collection)

## Prolog like head tail matching
If needed to iterate to  apply functors on collections.
```
cats([H|T]) : cat(H) & cats(T)
cats([])
cat(max)
cat(ginny)
cats([max,ginny])?
> true
```
Instead of this syntax, python slicing may be more consistent.
The problem here may be that cats should only match with collections. A type check would work, but make it makes 
it squarely the user's problem.
```
cats(CATS) : indexed(CATS) & cat(CATS[0]) & cast(CATS[1:])
``` 

Both of these still leave the problem of an empty list, which the prolog syntax seems to automatically match. Of course 
this can also be handled, but it seems verbose 
```
cats(CATS) : indexed(CATS) & size(CATS) & cat(CATS[0]) & cast(CATS[1:])
```

A slightly more intuitive syntax may be to define the [] postfix to require the type to be indexable. 
type matching (like C++ templates?) is a bit of an odd one out of the language however and then why 
wouldn't it count for other types like floating points, strings, predicates. 
THis also stil leaves the empty sequence problem.
```
cats(CATS[]) : cat(CATS[0]) & cast(CATS[1:])
```

Perhaps the for each operator `*` can SFINAE match when an expression is iterable. This also leaves it open for Sets, 
because the indexed solution is a stronger prerequisite than iterable.  
```
cats(*CAT) : cat(CAT)
```
or maybe `cat(CATS) : cat(*CATS)` or both. The former has the advantage that it can SFINAE match iterable types before the conditions.

## For each 
Iteration with head tail is slow. It would be more efficient to have a way to express for each. In first order logic
this would be close to All x or there is an X. The `*` operator could be used as prefix to express "for each". The 
`@` operator to mean "for any". With a little extra rigor this could make the language fully equivalent with first order logic.

```
cat(ginny)
cat(max) 
cat(*[ginny, max])?
> true
cat(@[ginny, ron])?
> false
```
By default collections match if all elements match. For collections this is in order, for sets, constructed with `{}` not
```
siblings([ron, ginny])
siblings([gizmo, ginny])?
> false 
```
## Also assumption assignment
.... (same knowledge as above)
```
cats(X)?
> X=[max,ginny]
```

## Queries can return hypothesis if it isn't entirely fulfilled
```
f(X):X+2
f(Y)?
> Y = X+2
```

## Clauses are a disjunction so multiple matches can be a disjunction
```
cat : 1
cat : 2
cat? 
> 1 | 2
```
(disjunctions of booleans are simplified to a simple computes result, predicates have to be true or at least one of them. But not cast ? why not? )

## Also element of, when the final answer is ambiguous 
```
cat(max)
cat(ginny)
cat(X) ? 
> X @ [max,ginny]
```
This is flattened to equivalence `X=` by default because of the built in logic that `X@[Y]` is true if and only if `X=Y`

## With math the match can also be a range 
```
f(X): X^2

f(Y) & Y @ [4:25]
> X @ [2:5]
```
## variables can be functions, to support lambda/higher order functions 
```
map([], _)
map(S, F): S=[H|T], [F(H)|map(T,F)]
f(X): X*2
map(f, [0,1,2])?
[0,2,4]
```

## Equivalence
Besides the clause operator `:` there is also an equivalence operator `=`, which works like implication both ways. 
After parsing this is turned into a horn clause by replacing `A=B` with `A:B` and `B:A`.
```
hairy(X) = cat(X)
hairy(ginny)
cat(ginny)? 
> true 
```

## Some set theory and math axioms are built in to help simplify
```
(X @ [Y]) = (X=Y) # if Y is the only element of a set and X is in it, then X must be Y
(X >= Y) = (X @ [Y:]) # if X>=Y then X is an element of the range starting at Y (similar for > < and <=)
```

## Some math function are built in (or can be imported in a namespace) 
Since they are ineffcient (hopefully not impossible) to implement in Angel 
they trigger a C callback 
```
math.sin(X): $callback_sin 
math.sin(pi)
> 0 
```

* `math.floor(x)` - rounds down a floating point value and returns it as an integer if it is in range 
* `math.ceil(x)` - same but rounds up 
* `math.round(x)` - same but rounds to the nearest integer
Basically everything in the python math module. Other useful modules may be added for statistics. 
The Angel library should provide a runtime loadable library format that can be imported and add this knowledge in a namespace. 


## Some universal constants are also defined that way 
```
pi: 3.1415926535897932384626433832795
e: 2.7182818284590452353602874713527
```

## Some built ins could have side effects to help with debugging 
```
print(X): $callback_print  # will do nothing but write X to stdout
break: $callback_trace     # will interrupt executing and break into a debugger with the current state inspectable with queries and modifiable with additional axioms. 
resume: $callback_resume   # if the inference is currently interrupted by a break, it will continue running and be true. If not currently running it will be false  
trace(X): $callback_trace  # enable (true) or disable logging of the progress (state changes) of the inference at each step. 
```
The trace syntax will prepend each knew predicate with `...` which is also used in this document to illustrate the inner working. 

## File access
```
import(X): $callback_import # Read a file with the name X, parse it and add all predicates to the knowledge. Will return false if failed to parse or the file does not exist 
```

Perhaps also: reading&writing to files/streams gpios, serial or sockets. 

Most of these evaluate to true. A function that errors before having any side effect (eg eof when reading) evaluates to false.
This allows repeating options similar to SFINAE to attempt to achieve the desired effect or unhappy flow. 
Of course if succesful the side effect will have occurred and there is no way to turn it back usuaully (as in RAII),
so it is best to rely on lazy evaluation for pre conditions and put the side effect clause at the end. For this 
reason evaluation is lazy. This can also be used as a monadic way to attempt side effects. 
```
read_config(File) : read(File+'.json`) | read(File+'.yaml`)
```

## Simple solvable equations should also be queriably 
```
> X^2-2x+1 = 0

X @ [-1,1]
```

