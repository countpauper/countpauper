# Goal 
Angel is a programming language that combines the best of: 
prolog: declarative logic, solving queries, matching
python: ranges, slicing, built-in and library binding, interactive console 
functional: all user defined clauses are pure, higher order functions that aren't evaluated until queried.   
The goal is to make a scientific programming language that can model the knowledge of a certain 
domain and reason to answer queries. 

# Interface 
## Interaction 
The default unit of interaction is a statement with a knowledge set. This can be a new predicate, which is added to the knowledge 
or a query if it is postfixed with a question mark instead of a condition. This is an interactive/command line only pre-parse.
This results in the output of the query with the hypothesis of variable instantiations. The knowledge starts by default with some built in functions, 
the most essential of which is parse(), which will parse the source code in the string and add all 
predicates in it to the root knowledge as a side effect.

## Console 
The default application for these interactions is a python console like app where standard input is read and when enter is pressed 
it is one interaction. This requires a newline to not be significant whitespace for parsing. Alternatively like python the 
enter is added to the input and only accepted as a new interaction with the result parsers succesfully, eg 
`predicate(bla):\n` doesnt parse because of a missing condition. This can still be added for convenience but is less essential. 
It may require implementation of backspace and cursors, otherwise the user could get stuck in an unparsable input `predicate(bla:\n`.
Perhaps double enter or ctrl-enter (as python) should force finalize the input. 
Every input is parsed into a separate knowledge. It's predicated are added to the root namespace and it's queries executed. 

## Command line 
The console application should interpret each argument that does not start with `-` as statement. Files can be executed with the 
`angel import("fn")`. All `print("")` built in statements would be written to standard output. 
Interpretation errors are written to stderr and language errors should probably be as well. 

Options starting with `-` or `--` are reserved for options, although no options are known yet. 

If after parsing the command line there is no single query to execute, the options and knowledge is remembered and interactive mode is 
started automatically. The `--interactive` option will force this.  


# Use Cases 
The use cases below do not yet form a coherent and complete specification of the language. 
They explore some of the options to look for opportunities and complications. More of these 
will emerge during development and be added here or directly as unit tests. These unit 
tests will form the true specification of the language.  

## Axiomatic predicates
an axiom is considered true if it is contained in the knowledge. 

```
cat
cat? 
> true 
mouse?
> false
```
An axiom it added to the list of known "facts" of the namespace.
It is equivalent to `cat: true`, any non defined axiom is by default `false`. Defining an axiom is then 
like assigning a value to a predicate or even more technically it's creating a key:value association and each knowledge is a 
list of associations of which the key is a predicate. Commas are not needed when parsing axioms.

## predicate arguments
An axiom without terms is true if its label and all arguments match, else it is false.
```
cat(ginny) 
cat(ginny)?
> true
cat(gizmo)?
> false
```

## functions with valance>1
A predicate can have multiple arguments, separates by the collection operator `,`. 
```
color(car, blue)
color(car, green)? 
> false 
color(tree, blue)>
> false 
```

Technically each predicate has a list of arguments of a size 0 or more. An axiomatic, so 
`cat()` is equivalent to `cat`.

## Clause
a predicate is true if it is implied and its antecedent is true. The predicate implied is 
separated from it's condition with the `:` operator. This means the implication is true if the 
conditions are true. This operator creates an association between the axiom on the
left and the antecedent on the right. 
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

## Variables 

Variables are preceded with `$` to distinguish them from axioms. Requiring the first letter to be a capital, like prolog, wouldn't 
work with unicode as not all languages have capitals and it would be very difficult to define in a regular expression. 

## Anonymous variable 
The anonymous variable, `_` in python and prolog, is instead simply defined as `$`. Any match assigned to this variable is 
immediately discarded and not instantiated in subsequent statements. 

## Clauses with variables as arguments 
These define a generic case. The variables have the scope of that clause. When matching the query, 
the instantiation is added to the hypothesis and while matching the antecadent predicates in that clause 
it will be used to substitute variable predicates.
```
cat($X): hairy($X)
hairy(ginny)
cat(ginny)?
... $X=ginny 
... hairy(ginny)?
true
> true
```

The hypothesis is instantiated left to right, so a repeat of a variable (or a predicate that includes it) already has to be a literal match
```
same($X,$X)
```

## Variable operators 
In a sense variables aren't a type, but the `$` is a prefix operator, which means "the value of". This applies both 
when matching (The value of X goes here) and when substituting (use the value of X). 
Normally the operand is an identifier, but technically it's also possible to have another variable as operand
for instance `$$X`. If `$X` is then hypothesized to be a specific identifier, `$$X` would be the variable value of that. 
It creates an interesting meta programming option although it's not clear what the use case is.  The value of non-identifiers
would not be defined and a runtime error in most cases. 

The all `*` and any `@` operators work the same, in fact that's what led to this, except that if X is a list then all of or 
any of the values in that list are matched or substituted. This means that `*$L` is also possible where `$L` is the name of a 
list. Conversely `$*L` where L is a list of identifiers becomes a list of variables with those identifiers. 
`$L=[X,Y] ⇔ $*L=[$X, $Y]`. 


## Matching comparators 
The arguments of the predicate of a clause are theoratically comparators. Matching is the same as infering a comparison. 
The default comparison is equality. `cat(ginny)` is equivalent (short hand for) `cat(=ginny)`. What technically happens 
when matching is that the comparison is instantiated. `ginny=ginny` and then computed. 

Mathching variables is then instaniating the comparison including the variable. This can only be solved with a hypothesis 
```
cat($X)
> cat(ginny)?
... ginny=$X  ``` instantiate the comparison 
... true : ginny=$x  # result of computing ginny=$X 
ginny=$x # hypothesis 
```

## Arithmetic unary comparators 
Comparitors `>` `>=` `<` `<=` `!=` can be explicitly used in clause predicates as well. The comparison will be with the specific operator.
``` 
large(>100)
> large(1)?
... 1>100
... false # result of the comparison, no other matches so: 
false
```
Again with undefined variables the arithmetic comparator becomes part of the hypothesis 


## Unary Comparators as antecedent 
If the unary comparator is the right hand side of a pair, it is considered the antecedent for the left 
hand side. The comparison is between the left and right hand side. 
```
$A:>2 # $A if $A>2
```
This is left as a pair unless $A is instantiated and the pair is added to a collection. Then the antecdent 
can be computed and only if it is true, the left hand side is added.
This allows this construction to be used as a quick filter 
```
A=[1,2,3]
> [*A:>2]?
... [1:1>2, 2:2>2, 3:3>3]
... [1:false, 2:false, 3:true]
[3]
```

## Non comparator filters 
TBD perhaps for non unary comparator filter, the anonyous placeholder variable can be used 
```
[cheese:is_food($)]
```
Does that make sense? 

To add an association to a collection, a nested pair may need to be used. Eg `[1:cheese:is_food($Nr:$)]`


## Container operators 
ALso the collection operators can be used. The simplest is the `@` any of, or in this case: member of compartor 
will match with any of the following set. 
```
cat(@[ginny, gizmo])
> cat(ginny)?
... ginny @ [ginny, gizmo]
... true 
true 
```
If the collection is a variable `@CATS` then ginny will then be hypothesized to be one of the $CATS. 

If the passed argument is itself a collection, the subset or superset operators can also be used. 

## All of operator 
THe all of operator is a little special in this regard. Since of course one argument can not match all of a collection
with more than one argument, multiple sequential arguments will be matched. This is not the same as matching two 
collections as that can be done with the equal, subset, and superset comparators.
The resulting syntax and behavior is a lot like pythons variable arguments and if the variable is a tuple it will be 
assigned these variable arguments 

```
friends(*{ginny, max})
> friends(max, ginny)? 
... [max,ginny]*{ginny,max}?
... true 
```
or
```
friendly(ginny)
friendly(max)
friends(*F): friendly(*F)
> friends(ginny,max)? 
... true: $F=[ginny,max]
... friendly(ginny)
... ... true 
... friendly(gizmo)
... ... true
true 
```

## Matching predicates
This may not be a very useful feature, but arguments of axioms can again be predicates with literal or variable 
arguments. 

```
derivative(sqr($X)):2*X
```

In that case a variable predicate identifier may be useful
```
animal(cat)
animal(fish)
taxonomy($X(_)): animal($X)|plant($X)
```

## Query 
Queries are predicates postfixed with the `?` operator. These queries start inference. 
It will try to match that predicate until it is proven evaluate to an elemental value like `true`, `false`, or an integer. 
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
cat($X)? 
> X = ginny 
```
Each variable in a query has scope of the query. If the query evaluates to true  
then these variables will have collected ALL the conditions under which that constant is true 

If the query has multiple variables, there may be multiple combinations of values for which the query is true. 

## Query logic 
Any query returns the set of possible results. If a query result is unconditional (or rather the condition is always true), the set just contains values, for instance boolean logic. If the set only contains one element it can be simplified to that element or one or more mathematical solutions (like solutions to a 2nd order function).

If variables in the query are instantiated during the matching, those become the antecedent for that value. The instantiation is in the form of an equation, which is true if the variable is equal to the instanced value.
```
cat(ginny)
> cat($X)?
true: $X=ginny # this reads as: cat($X) is true if $X=ginny
```
If there are multiple variables instanced they are a conjunction of conditions for the left hand side of the association. 
```
legs(ginny,4)
> legs($X, $Y)?
true: $X=ginny & $Y=4
```
Like the hypothesis, the namespace is also a set of associations, but still multiple matches can have the same result. 
When that happens, their hypotheses are a disjunction.
```
legs(ginny,4)
legs(lucky,3)
legs(snek,0): cat(snek)
> legs($X, $Y)?
true: ($X=ginny & $Y=4) | ($X=lucky&$Y=3)
```
Matches that are false do not have to be added to the hypothesis, as any non match is automatically assumed to be false. For boolean axioms there is then never a st with multiple entries. For non-boolean logic however multiple values are possible.
```
legs(ginny):4
legs(gizmo):4
legs(lucky):3
> legs($X)?
{4:$X=ginny|$X=gizmo, 3:$X=lucky}
```

## Multiple hypotheses 
Multiple predicates may match, which means multiple hypothes are possible. The hypotheses are basically a disjunction. 
From the one original substitution, additional variables may be added to that hypotheses or even disjunctions. 

Simple disjunctions can be simplified to set membership
```
cat(ginny)
cat(gizmo)
cat($X)?
> X @ [ginny, gizmo]
```

## Occam's razor and hypothesis priority 
When multiple matches are possible, the match(es) with the least amount of hypothesis are used. 
This is also used to implement recursion and logical induction. 
```
fibonachi(1)
fibonachi(X) : Y+Z = X & Y>0 & Z>0 & fibonachi(Y) & fibonachi(Z)
> fibonachi(1)
true
```
In this case the first clause matches and the second one does not. This prevents infinite recursion, trying to 
match endless X values.
How the inference engine generates valid Y and Z (integers) in this case to make Y+Z=X true is not yet determined.

## Expression matching and math 

When matching Y=X+2, perhaps X will be known and the summation expression can be computed and Y instantiated (1+2, Y=3).
In another case the matching will be the other way around. This is still solvable and should match. If Y is known (3) then 
the expression can be inverted. X=3-2: X=1. 
This math is relatively simple for most exprssions. Inverting an expression to get the unknown variable on the left side 
should be doable for +- */ ^ and even sqrt, using imaginray numbers. 

## Expression matching and logic
For logical expressions this should also be possible. This is (probably) trying to write complex logical expressions like 
A&B -> C|D into horn clauses. Usually the expressions are already written as horn clauses but when for instance matching lists
[X] [hairy&Y], if Y is known X can be true if hairy is true and Y. If X is known to be true, Y must also be true and vice versa.

## Hypothesis ranges 
Also set theory can be used to narrow down a hypothesis. This involves the set operators `>`,`<` and `@` as well as 
subsets, intersections and so on. 
`X>0 & X<4`, After the second expression, for the conjjnction to be true, X has to be in the open range `<0:4>`. If a second unknown is introduced `X>0 & X<Y` this may be matched with X=4. This then says something about Y: `Y>4`. 

## Hypthesis 
Also make design, expression, object, element and their functions 
There may need to be expression containers that can be computed and object containers that can be matched. 
But still the problem is matching expression containers and then needing to compute while matching 
[$X+2, ginny] match [4, ginny] IF $X is 2. But also ... (this is where the actual math inference comes in)
that could be the hypothesis if $X is as of yet unknown. because $X+2 = 4 : $X=4-2 

## Variable predicates

Seemingly have no purpose, perhaps with predicates. Mostly they seem harmful 
because technically they would make everything true. 
TODO: warning? or ignore

```
$X 
$X(ginny): X=cat 
```

## Variable instantiation and priority of matching 

## Namespace 
A namespace is a clause (pair), whose predicate (key) is the name and the antecedent is a bag of clauses, 
so their order does not matter but the predicates in the namespace do not have to be unique. 
Equal clauses imply a disjunction of the antecedents.
```
cat: short_hair
cat: maine_coon
```

By default each clause is added to the root namespace, but to separate domains of knowledge,
while still allowing them to refer to each other, a namespace can be wrapped around this set 

```music: 
{
   composer(bach)
   instrument(violin)
}
```
Within the namespace, predicates can be matched. If the predicate is not found the parent namespace is 
checked, until the root namespace. Predicates across child namespace can be matched by using the `.` operator 
to access the namespace. 
```
music.composer($X)?
> X = bach
```

## Association operator `.` 
This use of the `.` operator is in general used to access the right hand side of an association in a container. THe right hand side can be any expression and it is _matched_ to the container on the left.

Every container can be associative its elements are associations and the value can 
be accessed with this operator. 
```
cats: [ginny, gizmo]
> cats.ginny? 
1
``` 

Any item in a container that is not an association behaves as if it's right side is `true` and every 
value that is not in the container acts as if it's right side is `false`. The `.` operator can thereore
also be used to test if an item is in the container. For a non unique container it will return the container of matching items. 

If an assocation is in a container, which matches, unique containers will return the value. If there is no match
`false` is returned. For non-unique containers all right sides of assocations with matching left sides are 
returned in the same container type as the parent (eg bag or list).

In the rare use cases that the non-unique container is mixed assocition and single elements. the 
return value will be a list with the right hand side of all matching assoications and `true` for each 
other item that is equal to the requested key. 

Because there may be multiple matches even for unique containers, the `.` operator may result in a set 
of results for those as well. If the right hand side is an element then a single result will be returned. 

```
cats: {fuzzy(ginny), hairy(gizmo), hairy(max) }
> cats.hairy($)?
{hairy(gizmo), hairy(max)}
```
This is exactly how normal matching works when infering expressions. 

## Association operator `.` 

The association itself is also a container. The operator `.` isn't very useful. Matching can be used 
to isolate the left or right hand side. The `.` operator is more like a `get_if`. 
```
association: cat: ginny 
association.cat?
ginny 
association.dog? 
false
```

## Assigning assocation values. 
To use the `.` operator to create structural data, the right hand side also needs to be assignable.
This assinging is not done with the `=` operator, which is for matching. It's done by creating 
a new association with the `:` operator. 

If the `.` matches an association in a non-unique container, a reference is returned. This reference 
will allow assignment (is this still functional?).
This is close to learning new clauses. Knowledge clauses however do not need to be unique, so 
they can be appended to the knowledge. In unique sets the old association is removed. 

```
pets: {ginny:cat}
pets.ginny: dog 
```


## False value associations 
For unique containers, adding an assoication for which the value is false is equivalent to removing the element.
As only one key can exist, setting the value to false will remove it and its size will decrease. 
```
cats:{ginny, gizmo}
cats.ginny: false 
> cats.ginny? 
false
> size(cats)?
1
```

For non-unique containers, the `.` operator returns a list of all values and adding a `false` value 
will result in the `.` operator containing one or more `false` values. 
```
cats:[ginny, gizmo]
cats.ginny: false 
> cats.ginny? 
[true, false]
cats?
[ginny, gizmo, ginny:false]
```

## Namespace arguments 
The predicate of a namespace can also have arguments. If these are variables, then when matching they are 
substituted over the whole namespace. Somehow this should then reduce the valence when matching. 

```
genre($Music): {
   loves($f, $Music): fan($f, $Music)
}
genre(rock).fan(max)
genre(rock).loves(john)?
> true 
```

If the namespace arguments are not variable, the matching can go the other way around. There 
doesn't seem to be much of a use case, besides defining a set of axioms that share an argument 
with a little less duplication. It's a bit like a C++ template.

```
genre(rock): {
   fan(lizzy)
}

> genre($X).fan(lizzy)?
X@[rock]
```

## Integers 
integers match if they are evaluate to equal
```
cat(ginny): legs(4)
legs(4)
cat(ginny)?
> true
```

## Ranges 
Rangers are collections, like sets and lists, but they are contiguous. This allows them to be expressed with 
a start and an end as `a..b`. Ranges can be inclusive or exclusive on either end `<a..b]` or infinite `a..`.
Membership `@` can be computed on ranges. The for-each or for-any operators will iterate over all numbers in 
the range, but only if the size is not infinite. The size is infinite if either end is ifinite or if the set contains
floating points. In those cases iteration with for-each or for-any will result in an error.
Ranges are also a pair, like associations, of a begin and an end. On top of a begin and end, the range also has 
a few properties
* Open/Closed begin
* Open/Closed end 
If both begin and end are integer, then all elements in the range are integer. If both begin and end are imaginary 
then all elements are imaginary and the contents is a rectangle on the complex plane. 
Perhaps a range of N-dimnensional vectors is also allowed this way.

## Operations 
Operations are a class of expressions that consist of 0 or more elements and an operation. These operations can be any of the supported logical, mathematical, or comparsion operations like `|&+-*/<>=`.
Unlike nested operations of the same type are equivalent to concatenated operations. Of course different operations can be
nested using `()` in the syntax or precedence. 

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
If all earlier elements are true, the last element is returned as the truth value. Normally this is 
also true, but in some cases this can be used to return a conditional non boolean value, for instance 
as a pair. `[$A:=65 & "a"]` This will add the pair 65:"a" to the list if $A $is the ascii code for "A".
if it i snot the antecdent will be false and not added. If it is true it will be added with the
right hand pair "a' which is trueish.


## Disjunction 
A disjunction is true if at least one of its elements is true. 
The `|` operator is used for disjunction and and lazy evaluation continues inference as soon as the first element of the 
disjunction is found to be trueish. In that case it is returned.
```
cat(ginny): noisy(ginny) | shy(ginny)
shy(ginny)
cat(ginny)?
> true
```
Even if it's not a boolean, but trueish, it is returned as is. 
```
> false|3?
3
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
cat($X) & fuzball($X) : hairy($X) 
```
converted to: 
```
cat($X): hairy($X)
fuzball($x): hairy($X)
```

### Disjunction axioms

TODO:  unlike conjunction axioms, it's not clear how to convery this to horn clauses (unlike conjunection clauses).  
If it was `xor` then each one could be converted to a horn clause by postulating that the other elements of the 
disjunction must then be false. There is no xor operator defined yet, but for this example assume it is `||` 
```
cat($X) || dog($X) : hairy($X)
```
converted to 
```
cat($X): hairy($X) & ~dog($X)
dog($X): hairy($X) & ~cat($X) 
```

More complex compinations of conjunections, disjunctions and negations could be implied 
and a pretty solid truth table/logic solver may already be needed to convert them to horn clauses
This is why this is lower priority and at first only horn clauses are allowed.
When converting to horn clauses, for debugging, some traceability to the original clause and from
there to the source location is useful. 


## Negations 
The `~` operator is used. 
```
cat($x): ~dog($X) 
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
~cat($X): hairy($X)
cat(ginny)?
> error 
```

## Negated collections 
Negation is a logical operator, so a negated collection negates the collection membership. 
It's impractical to create a set with all elements not in a set, because there can be (nearly) infinite. 
Instated when a negated set is inferred, it is returned as the negated set. 

Subsequent operations can apply the negation, for instance negated set membership is equivalent and 
transformed to negated membership
```
dogs: {pluto, laika}
cat($X):$X.~dogs
> cat(ginny)?
... cat(ginny)
... ginny.~dogs
... ~ginny.dogs 
true 
```

## Sequences 
The `,` is the sequence operator that appends or inserts elements to a sequence (tuple in python). 
If it is a sequence of queries, then the result is of course a sequence of responses, although they share the same hypothesis.
Sometimes a sequence consists of imperative functions. In this document for instance it 
is used to enable trace as `trace,f(2)`. Since the scope of this sequence is shared the scoped trace function remains 
active while infering `f(X)`. Of course this can also be used with variables `X=2,f(X)`. The final result of the sequence 
operator is just the concatenated sequence, which can be an argument for a predicate `()` list `[]` or set `{}` operator 
to turn into a specific other type. The set operator checks if all items are associations to turn it into a dictionary. 

The terminating `,` is ignored, which allows making a sequence of one element `a,`. This is not needed for the predicate, 
list or set operators, which require a sequence and will automatically expand elemental types into a sequence of 1. 


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

## Structural types 
Angel is not an object oriented language. Still, similar to python, structured type are supported and are 
basically nested dictionaries. The dictionary is a set of associations and `:` is the association (aka pair) operator. A clause is 
then essentially an association of predicate and a clause. A namespace is however not a dictionary, because these 
predicates do not need to be unique. It is a non-unique unordered (ie bag) of associations. 
```
$X={a:1, b:2}
```
The keys can be any expression type. With strings they can contain spaces. They can also be integers making 
effectively a sparse array, which can be accessed with the access operator `X[a]`.
By default however they are identifiers (equivalent to enums, predicates without arguments). Dictionaries with 
identifiers can also be accessed with the `.` operator: `X.a`. 

## Ordering 
For sorted containers (Is set sorted or unordered)? and in general for the ordering operators, all expressions should be 
ordered. This is first done between types with a hash with the top bits divided in categories. The lower provide an order 
within the category, but on hash collisions they should provide ordering operators, mainly `<` to resolve it. 

The categories: 
* Numbers: All converted to floating point, then hashed to 48 bit
* Ids & Strings/Variables: some categories for unicode high bits for the first character, more precision around ASCII
* Containers: Very rough binary space partition for elements. The  bits and then the top 8 bits of the sub ordering
* Predicates: Top 16 bits of the string id and 8 bits valence
* Operators: Same encoding as containers for operands, but each first their operation precedence score 

## Converting number to and from boolean. 
Functional integer and floating point functions can be mixed into the boolean inference engine using comparison operators. 
There is no implicit conversion, to avoid inference erros. 
```
greater($X,$Y): $X>$Y
equal($X,$Y): $X=$Y
```

## Integer and floating point operators 
Of course all basic integer and floating point operators `+-*/^` are supported, as well as negation. These of course obey PEMDAS 
and if either of the operands is floating point then the other operand is cast to floating point as part of the operation 
``` 
f($X): $X-0.5
f(3)?
> 2.5
```

## built in integer and floating point operators 
* `float($X)` - casts a value to float whether it is boolean, integer, already float or string. 
* `int($x)` - casts a value to an integer whether it is boolean, float, already integer or string. 
* `bool($X)` - casts a value to a boolean, including collections if they are not empty.
* `str($X)` - converts the value to a string in such a form that it would convert back with the functions above.
* `is_float($X)` etc. - true if the type of the argument matches the built in. 
* `type($X)` - returns an id with the type name of the argument. `int`, `float` `string`, `bool`, `predicate` etc. 
More functions may be imported as a math module, see below.

## Container operators 

Containers support operators as well. If either the left or the right hand side is singular and the other is not 
it is upgraded to a single element set. 
* `.` - element access, converts to True if the left operarand is part of (if singular) or a subset of (if a collection) of the right operator 
* `&` - intersection: the collection, with the elements that are both in the left and the right hand side. 
* `|` - union: the collection  with the elements that are either in the left or the right hand side 
* `+` - a collection with the elements of the right hand side added to the list of the elements of the left hand side.  
* `-` - subtraction:  the collection, with the elements of the left hand side that are NOT in the right hand side. 
* `[x]` - access: extract or slice element(s) of a sequential container identified with index `x`, range `x:y`, sequence `x,y,z` or key `x`.
The type of the resulting collection is the same as the type of the left hand side, so eg subtracting a list from a 
set results in a set, but subtracting a set from a list results in a list.

## built in collection functions 
* `empty($X)` - returns true if the collection is 0 elements
* `size($X)` - returns the number of elements in the collection 
* `set($X)` - Create a set of the collection or element X
* `list($X)` - Create a list of the collection or element X 
* `iterable($X)` - Return true if X is iterable (sets, strings, lists, bags)
* `ordered($X)` - Return true if X is ordered and can therefore be indexed (strings, list)
These could also be in an optionally imported `colllection` library to reduce name clashes 

## String operators 
Strings are a lot like lists of characters, except that conjunctions and disjunctions make little sense. 
* `.` - if the right hand substring is in the left hand, it returns the position, else `false`.  
* `+` - concatenates two strings
* `-` - removes all characters of the right hand side from the string.
Slicing can be used to access sub strings or individual characters. 

## String functions 
A string module could add other common string functions:
* `upper($X)` and `lower($X)`
* `isupper($X)` and `islower($X)` 
* `strip($X, $Y)` - Returns a string where all Y are removed from the ends of X
* `replace($S, {$X:$Y})` - returns a string where all x are replaced by Y 
* `format($X,[args])` - returns a formated string 
* `regex($X, expr)` - returns true or false or a non empty list of captures
* `wildcard($W, $M)` - returns true or false if it matches

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
cat($X) : legs($X,$Y), $Y=4
legs(ginny, 4)
cat(ginny)?
> true
```

## Equations can be inequal. they are true if the value is in range
Inequal equations 
```
cat($X) : legs($X,$Y) Y>3
legs(ginny,4)
cat(ginny)?
> true
```
## Query variables 
```
cat(ginny)
cat($X)?
X=$ginny
```
How it works is `cat(X)` **hypothesizes** `ginny=X`. This hypothesis remains until there is a contradiction, in which 
case it backtracks like prolog and tries another hypothesis if one is available. 

## Query operators 
The default comparison operator for matching arguments is `=`. It hypotesises a single value for which 
the equation is true, but other comparison opeartors can be used as prefix. .

## Prefix comparison operators without variables. 
With or without variables, prefix comparison operators can be used when matching 
```
positive(>0)
> positive(-1)
false
```
The queried argument is filled in as the left hand side and if it's true it matches. If the right hand side is an Variable it 
becomes part of the hypothesis. If the query is a variable it also becomes part of the hypthesis. 
```
positive(>0)
> positive($X)
true: $X>0
```
or 
```
cat(@{max,ginny})
> cat(max)?
... true:max@{max,ginny}
true 
> cat($C)?
$C@{max,ginny}
```

### Tuples
Tuples are the All operator applied to a variable Id. 
When matching containers, instead of the single element variable the tuple variable prefix `*` can 
be used. 
```
cat(*x): cat(*X)
cat(ginny, itsybitsy)
> cat(ginny)?
... X=[ginny,itsybitsy]
... cat(ginny) & cat(itsybitsy)
... ... cat(ginny)?
... true
... ... cat(itsybitsy)?
... false
false 
```
This hypothesis that the variable $X is a container that matches the remaining elements in the container (could be empty).
The type of container is the same as the container being matched.

### Tuple hypothesis 
Any variable that contains a container can be substituted to match multiple elements. This is the inverse of the tuple. 

## Sequences = ordered non-unique collection 
you can use the @ operator to check membership of a collection 
```
cat($X) : $X @ [ginny,max]
cat(ginny)?
> true
```
How it works: hypothesize `X=ginny`, `ginny @ {ginny,max}` is true because ginny is an element of that collection. 

## Hypothesis while inferring

Variable hypotheses can be generated while matching, but also while inferring the antecedent of a clause. 
The scope of these variables is from the moment they are hypothesized until the end of the clause being inferred
or in other words, all expressions inferred to prove the predicate share that hypothesis. 

```
cat($X):= $LEGS = 4 & legs($X,$LEGS)
cat($X) := legs($X,$LEGS) & $LEGS=4 # equivalent
```

## N-ary operations 
Subsequent operators `|` and `&` and mathematical operators like `+` and `*` are grouped in a single operation expression. 
Comparison operators like `>`, `<=`, and `!=` can be as well, which then effectively determines if all elements are 
sorted, inverse sorted and or unique.  With matching variables may be inserted `5<$x<7<$Y`. 

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

## Slicing with interval
When using an interval  index, multiple elements are used
```
[1,2,3][1..2]
> [2,3]
```
## Infinite interval
When not specifying a value for an interval, that side is infinite. Any slice is clipped to the size of the collection, 
so infinite slices to the end. 
```
[1,2,3][1..]
> [2,3]
```

## Open interval 
`< >` brackets are used when the limits are excluded. 
This is less important, but it is needed for the equivalance of `X>3 = X@<3..
For inclusive no brackets are needed at all. 
NB It's a bit strange that the brackets are reversed from the comparator while in a way it's the range starting at a number >3.
```
[1,2,3][1..2>
> [2]
```

## @ operator also works on ranges
```
2 @ [2:]
> true
```

## Container operators 
Containers can be merged with `+`, removed with `-`, unioned with `|` and intersectioned with `&`.
For unique containers `+` and `|` are the same.
For non unique containers, `|` and `&` will keep find duplicates. With `|` the maximum duplicates of 
all operands is taken and for `&` the minimum. 

## Container left hand side with scalars. 
The left hand type is leading. A scalar operator, with a scalar right hand side, but a container left hand side 
will apply the operation to each element. `[1,2]+1` = `[1+1,2+1]` 
A scalar left hand side with a container right hand side is a cast error or the same. 
Logical operations are also included here `[cat(ginny)]&fuzzy(max) = `[cat(ginny)&fuzzy(max)]`

## All of and any off operators on operations 
Normally when the all of or any of operator is used, it has less precedence. `*[1,2]+1` = `*[1+1,2+1]`
Even if it has precedence, the inference moves the operator outwards. `(*[1,2])+1` = `*[1+1,2+1]`. 

## Right hand side all of 
if the left hand side is a scalar operation, the all of operation makes it n-ary: `3+*[1,2] = 3+1+2`. 
The any operator does somsething similar although it mostly makes sense for logical operations 
cat&@{girl, boy} = cat&girl | cat&boy.

## Random idea about any operator. 
Since on mathematics 3+@[1,2] is 3+1 or 3+2 or whatever, this could be interesting for matching but 
when interfering it just yields multiple answers that are equally possible. The code paths could 
split here if somehow this leads to a match or comparison later down the line. If it's purely 
mathematical however it could be used to generate a random solution. Sometimes 3+@[1,2] would be 4, sometimes 5.
Together with intervals it's even a relatively nice ay to generate random numbers in that interval, 
whether integer or float and with sets of ids it could choose a random element. 

## All of/any of comparisons 
If the operator is a comparison, the result becomes a conjunction or disjunction instead 
`@[1,2]<2 = 1<2 | 2<2`  
THis also works for the `@` binary operator so `@[1,2]@[2,3] = 1@[2,3] | 2@[2,3]`

## The boolean math is very prolog like so far, but  
Clauses (functions) don't just return boolean. In fact all clauses are functions that 
can either return boolean or (falsey) other types like integers, floating point, ranges, containers, strings. 
What matter is where that value finally ends up. Does it end up:
- In a query it will be cast to boolean to determine if there's a match or another match should be tried. 
- In an assignment, the variable is assigned that value 
- In a comparison operator, it becomes boolean. 
``` 
f($X): $X+2
f(1)?
> 3
```

## Errors
While matching any error language is returned (as std::expectation). If it is part of a match 
the match fails (MFINAE). If it is part of another operation the error is propagated to the query. 
Some debug statements could "catch" intermedtiate errors and tracing will log them as well. 

```
f($X): $X[1] + 2
f([cheese])?
> Index out of range [cheese][1]
f(4)?
> Integer 4 is not indexed. 
f($X): $X+2
trace, f(4)?
... f(4): 4[1]+2 # Integer 4 is not indexed.
... f(4): 4+2  
> 6 
```

## axioms are used as variable assignment with that 
Equivalence operator `=` is not used because legs = 4, but 4 is not equivalent to legs. 
```
legs: 4
cat($X): legs>=3
cat(ginny)?
> true
```

## id arguments are like enums
```
cat(ginny) 
cat(X) ? 
> X=ginny 
```

These ids are not predicated without axioms, because they don't have to be computed and matched to be true.
They are elements.


## Sequence/range matching 
```
[$X,$Y,$Z] = [max, ginny, gizmo]
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

Perhaps the for each operator `*` can MFINAE match when an expression is iterable. This also leaves it open for Sets, 
because the indexed solution is a stronger prerequisite than iterable.  
```
cats(*CAT) : cat(CAT)
```
or maybe `cat(CATS) : cat(*CATS)` or both. The former has the advantage that it can MFINAE match iterable types before the conditions.

The tail can then also be matched, like python variable arguments, by matching the remaining unmatched collection elements: 
```
cats([ginny, max, *others]) : ~dogs(others)
```
or even more like python to create valance-(x+n) predicates `cats(ginny, *others)`. The anonymous match `*` can be used
to discard the tail. It can possibly even match the middle of ordered lists `cats(ginny, *, max)`. For unordered
collections it already doesn't matter where the `*` is, it'll match any arguments not explicitly matched.

This functions by `*` being the prefix for a a special type of variable that is a container of the same type as the container being matched.  

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

In these situations the existential prefix operators `*` and `@`  are used when matching. They generate a collection or disjunction of all possible matches. They can also be used in associations with collections, like tuples 

```
> *[1,2,3]:<=2  # This is equivalent to [*NRS:<=2]  when $NRS=[1,2,3]
[2,3]
> @[1,2,3]:<=2  # this is equivalent to [@NRS:<=2] 
[2] # only one element matches, early out   
```

## Also assumption assignment
.... (same knowledge as above)
```
cats(X)?
> X=[max,ginny]
```

## Queries can return hypothesis if it isn't entirely fulfilled
```
f(X):$X+2
f(Y)?
> Y = $X+2
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
cat($X) ? 
> X @ [max,ginny]
```
This is flattened to equivalence `X=` by default because of the built in logic that `X@[Y]` is true if and only if `X=Y`

## With math the match can also be a range 
```
f(X): $X^2

f($Y) & $Y @ [4:25]
> X @ [2:5]
```
## variables can be functions, to support lambda/higher order functions 
```
map([], _)
map($S, $F): $S=[$H|$T], [$F($H)|map($T,$F)]
f($X): $X*2
map(f, [0,1,2])?
[0,2,4]
```

## Equivalence
Besides the clause operator `:` there is also an equivalence operator `=`, which works like implication both ways. 
After parsing this is turned into a horn clause by replacing `A=B` with `A:B` and `B:A`.
```
hairy($X) = cat($X)
hairy(ginny)
cat(ginny)? 
> true 
```

## Some set theory and math axioms are built in to help simplify
```
($X @ [$Y]) = ($X=$Y) # if Y is the only element of a set and X is in it, then X must be Y
($X >= $Y) = ($X @ [$Y:]) # if X>=Y then X is an element of the range starting at Y (similar for > < and <=)
```

## Some math function are built in (or can be imported in a namespace) 
Since they are ineffcient (hopefully not impossible) to implement in Angel 
they trigger a C callback 
```
math.sin($X): $callback_sin 
math.sin(pi)
> 0 
```

* `math.floor($x)` - rounds down a floating point value and returns it as an integer if it is in range 
* `math.ceil($x)` - same but rounds up 
* `math.round($x)` - same but rounds to the nearest integer
Basically everything in the python math module. Other useful modules may be added for statistics. 
The Angel library should provide a runtime loadable library format that can be imported and add this knowledge in a namespace. 


## Built in constants

Some universal constants are also defined that way. Since ids can be unicode, the unicode for common mathematical 
and physical constants can also be used. 
```
true: 1
false: 0
pi: 3.1415926535897932384626433832795
π: 3.1415926535897932384626433832795
tau: 2*pi
e: 2.7182818284590452353602874713527
∅: {}
inf: inf # defined to the floating point value 
nan: nan # defined to the floating point value 
∞: inf
½ = 0.5
```

## Imaginary numbers

TBD: imaginary numbers could be supported if `i` was defined as the imaginary number 0+1i. This would claim the popular
iterator place holder, but then that would be $i and loops are not easily defined. 
Like upgrading integers to floats if one operand is float, the next upgrade could be imaginary. Automatic 
downgrades to float when the imaginary part becomes 0 again might improve performance. 
All built in math functions and operators should support imaginary numbers. The i postfix on integer and floating 
point constants could also indicate imaginary. 

## Some built ins could have side effects to help with debugging 

`print($X)`: $callback_print  # will do nothing but write X to stdout
`help($X)` : $callback_help  # prints the documentation of a predicate with matching or with id %x. If it's a built in or an automatic description for user predicates. If it's a namespace it shows all predicates in the namespace.
`help()` : prints the contents of the root namespace.
`delete($X)` : Delete all functions matching the predicate $X.
`trace`/`traceing`: $callback_trace  # Sets the tracing variable to boolean $X to enable or disable logging of the progress (state changes) of the inference at each step. This is scoped (variable) so beyond the sequence or conjuncion in which it first appears it is automatically turned off. 
`break`: $callback_trace     # will interrupt executing and break into a debugger with the current hypothesis inspectable with queries and modifiable with additional axioms. For instance `print(X),trace,resume`.
`resume`: $callback_resume   # if the inference is currently interrupted by a break, it will continue running and be true. If not currently running it will be false  
`catch($X)`: $callback_catch  # an error in the scope is matched with `X` and ignored if it matches. This is scoped so if the error occurs after the query infering the catch it is no longer active.  

The trace syntax will prepend each knew query or hypothesis modification with `...` which is also used in this document to illustrate the inner working. 


## File access
```
import(X): $callback_import # Read a file with the name X, parse it and add all predicates to the knowledge. Will return false if failed to parse or the file does not exist 
```

Perhaps also: reading&writing to files/streams gpios, serial or sockets. 

Most of these evaluate to true. A function that errors before having any side effect (eg eof when reading) evaluates to false.
This allows repeating options similar, using MFINAE, attempt to achieve the desired effect or unhappy flow. 
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

## Threading 

The user shouldn't think about threading. The inference engine should be implemented with a  
(configurable) number of threads. 

Usually the knowledge is accessed read only and the state of inference is on stack, so multiple hypotheses 
threads can access it at once. Instead of straight forward recursively infering predicates, all predicates in a conjunction or disjunction, all elements in a for each/for any or all hypotheses in a match, can be added to a queue to prove. This queue can then be inferred on multiple threads. Of course these queue items 
should also be cancelable by the inferer, so when a counter example is found all redundant hypotheses are 
cut short. 

A small number of built in functions modify the knowledge. Commands like `import` or `delete`. To do this they need write access to the knowledge. When they do they should lock the knowledge. This will block the inference queue until they are done. 
