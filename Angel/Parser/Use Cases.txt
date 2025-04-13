== Goal 

Angel is a programming language that combines the best of: 
prolog: declarative logic, solving, matching
python: ranges, preparsing into 
functional: all user defined functions are pure, higher order functions 

== axiomatic predicates
= an axioma is considered true if it is contained in the knowledge
cat
cat? 
> true 
mouse?
> false

== functional predicates
= a function is true if it's label and arguments match, else it is false
cat(ginny) 
cat(ginny)?
> true
cat(gizmo)?
> false

== Clause
= a predicate is true if it is implied and it's antecedent is true
cat(ginny) : hairy(ginny)
hairy(ginny)
cat(ginny)?
> true

== Expression clause 
= an antecedent is true if it's expression evaluates to a true boolean
cat(ginny) : true
cat(ginny)?
true

== Integers 
= integers match if they are evaluate to equal
cat(ginny): legs(4)
legs(4)
cat(ginny)?
> true

== Conjunection 
= a conjunection if expressions is true if all its elements are true
cat(ginny): hairy(ginny) & noisy(ginny)
hairy(ginny)
noisy(ginny)
cat(ginny)?
> true

== DIsjunction 
A disjunction is true if at least one of its elements is true
cat(ginny): noisy(ginny) | shy(ginny)
shy(ginny)
cat(ginny)?
> true

== Summation
A summation evaluates to the sum of its elements
cat(ginny) : legs(4) 
legs(2+2)
cat(ginny)?
> true

== Variables 
= variable matching is postponed (assumptions) until the value is assigned
cat(X) : noisy(X)
noisy(ginny)
cat(ginny)?
> true 
(how it works is cat(X) matches with the query to assume X=ginny, so noise(X) is noisy(ginny), this matches)

== Equations are true if the equation is true
cat(X) : legs(X,Y), Y=4
legs(ginny, 4)
cat(ginny)?
> true

== Equations can be inequal. they are true if the value is in range
= Inequal equations 
cat(X) : legs(X,Y) Y>3
legs(ginny,4)
cat(ginny)?
> true

== Query variables 
cat(ginny)
cat(X)?
X=ginny
(how it works is cat(X) assumes X is ginny. This assumption remains until there is a contradiction) 

== Sequences = ordered non-unique collection 
= you can use the @ operator to check membership of an array 
cat(X) : X @ [ginny,max])
cat(ginny)?
> true
(how it works, assume X=ginny, ginny @ [ginny,max] is true because ginny is in that array)

== Indexing 
= When using an integer index on an array a single element is used 
[1,2,3][2] 
> 3

== Sequence indexing
[max,ginny,gizmo][0,2]?
> [max,gizmo]

== Slicing 
= When using a range index, multiple elements are used
[1,2,3][1:2]
> [2,3]

== Infinite range 
= When not specifying a value for a range index, that side is infinite 
[1,2,3][1:]
> [2,3]

== Exclusive range (less important)
= < > brackets are used for exclusive 
[1,2,3][1,2>
> [2]

== @ operator also works on ranges
2 @ [2:]
> true

== The boolean math is very prolog like so far, but  
== clauses (functions) don't just return boolean 
 
f(X): X+2
f(1)?
> 3

== 
== axioms are used as variable assignment with that 
legs: 4
cat(X): legs>=3
cat(ginny)?
> true

== id axiomas are like enums 
cat: ginny 
cat ? 
> ginny 
(but it would be hard to not try to match ginny and fail and decide she's not a cat)

== Sequence/range matching 
[X,Y,Z] = [max, ginny, gizmo]
> X=max & Y=ginny & Z=gizmo 
(not sure if the assumptions should take the form of a conjunction or an array)

== Prolog like head tail matching? 
cats([H|T]) : cat(H) & cats(T)
cats([])
cat(max)
cat(ginny)
cats([max,ginny])?
> true

== Also assumption assignment
.... (same knowledge as above)
cats(X)?
> X=[max,ginny]

== Queries can return expressions if assumptions aren't entirely fulfilled
f(X):X+2
f(Y)?
> Y = X+2

== Clauses are a disjunction so multiple matches can be a disjunction
cat : 1
cat : 2
cat? 
> 1 | 2
(disjunctions of booleans are simplified to a simple computes result, predicates have to be true or at least one of them. But not cast ? why not? )

== Also element of, when the final answer is ambiguous 
cat(max)
cat(ginny)
cat(X) ? 
> X @ [max,ginny]

== With math the match can also be a range 
f(X): X^2

f(Y) & Y @ [4:25]
> X @ [2:5]

== variables can be functions, to support lambda/higher order functions 
map([], _)
map(S, F): S=[H|T], [F(H)|map(T,F)]
f(X): X*2
map(f, [0,1,2])?
[0,2,4]


== Besides the implication operator : there is also an equivalence operator 
which works like implication both ways
hairy(X) = cat(X)
hairy(ginny)
cat(ginny)? 
> true 

== Some set theory and math axioms are built in to help simplify
(X @ [Y]) = (X=Y) # if Y is the only element of a set and X is in it, then X must be Y
(X >= Y) = (X @ [Y:]) # if X>=Y then X is an element of the range starting at Y (similar for > < and <=)

== Some math function are built in (or can be imported in a namespace) 
Since they are ineffcient (hopefully not impossible) to implement in Angel 
they trigger a C callback 
sin(X): $callback_sin 
sin(pi)
> 0 

== Some universal constants are also defined that way 
pi: 3.1415926535897932384626433832795

== Some built ins could have side effects to help with debugging 
print(X): $callback_print #will do nothing but write X to stdout]
break(X): $callback_trace #will interrupt executing and break into a debugger with the current state inspectable 
# perhaps also: reading&writing to files, gpios,  
# Most of these evaluate to true (unless error eg eof) and the side effect will occur every time they are 
# matches, so it's best to put them at an end and rely on the lazy evaluation for timing 

== Simple solvable equations should also be queriably 

> X^2-2x+1 = 0

X @ [-1,1]



= So what is the exact implementation 

== simple axiom 
adds it to the list of "facts" of the namespace 
cat 
# it is equivalent to cat: true, any non defined axiom is false 

== Query will scan that axiom and if it is proven evaluate to true, else false 
# the query can expand or change while being resolved, but as soon as the list of 
knowledge includes a value (true, false, an integer or a collection of such) resolution is done 
cat? 
> true 

== variable axioms 
Seemingly have no purpose, perhaps with predicates. Mostly they seem harmful 
because technically they would make everything true
X 
X(ginny): X=cat 

== predicate axioms 
Are also added to the facts, but they allow matching with variables 
cat(ginny)
cat(X)? 
> X = ginny 

== Implications 
Using the : operator, which means true if all conditions are true 
knowledge can be derived. During a query, if the implication MATCHES the query, 
the conditions are added to the query (replaced?) and if that query evaluates, then the 
original query does. This is of course done recursively. If that new query fails, other 
implications that match are tried (in which order? for now the order they are defined )
cat(ginny): hairy(ginny) 
hairy(ginny) 
cat(ginny)? 
.... hairy(ginny)?  true 
> true 

== Implications with variabl predicates 
These define a generic case. 
The variables have the scope of that implication. 
when matching a query the variables are defined
cat(X): hairy(X)
hairy(ginny)
cat(ginny)?
...  X=ginny, hairy(ginny)?

== Conjunction 
The & operator is used for conjunection. A conjunection is false as soon as 
one element is false. This is avaluated in a lazy way for performance and for the few 
axioms (built in) with side effects 

cat(X): hairy(X) & tail(X) 
hairy(ginny)
tail(ginny) 
cat(ginny)?
... hairy(ginny)&tail(ginny)? 
... hairy(ginny)? true 
... tail(ginny)? true 
> true 

defining a conjunction axium should be the same as defining them individually 
cat(ginny) & cat(gizmo) 
cat(ginny)?
true 
Except that the implication can be shared to avoid code duplication. This does make query 
matching more complex. After passing these are converted to horn clause instead 

cat(X) & fuzzbal;(X) : hairy(X) 
converted to: cat(X): hairy(X), fuzzball(x): hairy(X)

== Negations 
~ operator is used for negations (! could still be usef for factorial or whatever)
axioms that are negated are just the same as being absent
~cat(ginny) is cat(ginny): false or just not appearing. When converting to horn clauses these might be removed 
if they are implied then it is negated 
~cat(X): wheels(X)

== Disjunctions 
The | operator is used for disjunction and inverted is true as soon as (lazily) one element is true.
So when converting to horn clause, the other options are negated and made part of the conditions 

cat(X) | dog(X) : hairy(X) 
converted to cat(X): hairy(X) & ~dog(X), dog(X): hairy(X) & ~cat(X) 

More complex compinations of conjunections, disjunctions and negations could be implied 
and a pretty solid truth table/logic solver may already be needed to convert them to horn clauses
This is why this is lower priority and at first only horn clauses are allowed.
When converting to horn clauses, for debugging, some traceability to the original clause and from
there to the source location is useful. 

== Querying a variable 
Each variable in a query has scope of the query. If the query evaluates to true  
then these variables will have collected ALL the conditions under which that constant is true 
TODO: is there a point if the result is not a boolean? what if it's a falsey value like 0? 


== Priority list 
- Parsing enough for axioms
- Axiom and query 
- Parsing enough for horn clauses 
- query horn clause 
= Parsing enough for predicates 
- Parse variables 
- query with variable and return 