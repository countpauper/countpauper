## Quick todo / Rafactor

- [ ] Try to remove substitute it from all non inference functions (get. add, substitute) and merge it with infer. It's mostly just infer for operations.  
- [ ] Subsitute is infer for variables, remove that too 
- [ ] There is too much internal simplifying (especially when adding to containers) and subtituting. 
        All operations infer, substitute,simplify, should not depend on order, so:
        - [ ] Only substitute matched variables with antecdent at the end of infering predicate (dont pass hypothesis?)
        - [ ] If simplify is infer (without knowledge) then it should never be needed explicitly. ALways recursive or iterative?  
- [*] $Variable is a Value/Ref operator with an Id content (none for anonymous) 
   - [*] Tuple wholy replaced by All(Id), BaseClass ValueOperator
   - [ ] Matching of All with List should be generalized to matching two sub containers/ranges with const_container_iterator    (All  Set match?)
      - [ ] Match All/Any in the middle [1, *V, 4] = [1,2,3,4] *V = 2,3. This requires extra recursion over sub ranges. 
- 


## Backlog 
- [*] Get back up 
-    [*] remove wstring and platform dependenty conversion 
-    [*] move, build
-    [*] Fix logic unit tests
-    [*] Recursively match and infer clauses, can't return a disjunction of all matched predicates. they won't have the same scope
-    [*] Clear distinction between language casts and operators (equal) and C++ operators/bool? Perhaps equal is the same (same type & value) but Cast is not implicit unless infering/matching
-    [*] Redesign polymorphic Expression with std::variant ? (expessions would get big? or op + vector)
-    [x] Refactor Object away (renamed to expression)
- [*] Redesign the parser 
-   [*] Knowledge is a set of clauses, clauses are pairs predicate: expression
-   [x] Disjunction and Conjunction are examples of NAry-operators (are they also expressions?)
-   [x] Sequences and sets perhaps as well (collections) with the , operator 
-   [*] Define constexpr context free grammar
-   [*] Recursive descent parser  
-   [*] Find a way for the production rules to produce
-   [*] Check the grammar for constraints of LL(1): left recursion 
-   [*] Parsing enough for axioms
-   [*] Parsing enough for clauses
-   [*] Parsing enough for predicates with arguments
-   [*] Parse set of axioms
-   [*] Parse variables 
-   [*] Parse conjunctions 
-   [*] Parse list 
-   [*] Parse set
-   [*] Parse errors and query exception workaround ends in ? 
-   [*] parse *tuple
- [ ] Prolog 
-   [*] query horn clause 
-   [*] Query predicate  
-   [*] Variable clauses with substitution 
-   [*] Query with variable and return hypotheses
-   [*] Sequences for valence 2+ predicates
-   [*] Disjunctions
-   [*] Negation  
- [ ] Console and built in commands useful for debugging
-   [*] help 
-   [*] help($topic)
-   [*] delete clauses (lock knowledge? Special Functions with write access?)
-   [*] print  
-   [*] trace 
-   [ ] break  
- [ ] Arithmetic 
-   [*] define constants 
-   [*] define arithmetic (non boolean) functions 
-   [*] integer operators `+-*/^`
-   [*] Inequality comparison, at least < for sort, perhaps != (should be easy in Expression at least)
-  [ ] Functional 
-   [*] lists
-   [*] sets to the same level as lists (equality, var&tuple matching, operator. etc)
-   [*] Be able to implement factorial like Haskell https://en.wikipedia.org/wiki/Haskell#Code_examples
    - [*] With inequality match `factorial(<2): 1`
-   [*] List operators - + & |
-   [*] Some way to filter. Using boolean associations seems appropriate as false means out `[*($V @ input: $V<2)]` or even better [*in:<2]
        [*]  associations with a single argument comparison will apply to the left side when simplifying. 3:<2 is 3 or 3:false, which is not added to the list
        [*] Add associations to list (and sets) checks false of right hand side          
        [*] associatons with a tuple will apply the associations to all elements of the tuple 
        [ ] Test conjunction & disjunction return the last/first truish value as is

Sort code: 
quicksort([]): []
quicksort([$H,*T]): quicksort([*T:<$H]) + [$H] + quicksort([*T:>=$H])

-   [*] set on list and list on set operators `&|+-` (if both side containers?) 
-   [*] Be able to implement quicksort like Haskell https://en.wikipedia.org/wiki/Haskell#Code_examples (x:xs means [x]+xs if xs is a list or as used here to split head:tail
-   [*] Membership operator x.{x,y} (done for List)
- Parser Update 
-   [ ] Optimize regex match with iterators (see TODO)
-   [ ] Reduce Term size (see TODO in test) by not precompiling regex or on the heap
-   [ ] Operator precedence when parsing (see TODO)
-   [ ] () braces (test)
-   [ ] All binary operation
-   [ ] Negative (non integers)
-   [ ] Unicode operators 
-   [ ] Prefix comparators 
-   [ ] Make sure both functional examples can be fully parsed 
-   [ ] Namespace 
- Python
-   [ ] namespaces
-   [ ] Indexing (a pair) lists (not sets, only ordered/indexible)  [x,y,z][1]
-   [ ] String is a container with operator +(concatenate) -(remove all) if rhs is string 
-   [ ] String oprator &: any overlap, operator |: TBD 
-   [ ] String operator: if rhs is another container, try to cast alle elements to string and then handle individually
-   [ ] String that can be indexed for substring 
-   [ ] String Get is is like find substring, perhaps return count of matches 
-   [ ] Get with a container of keys for Tuple and Set. Return type is same type as container. So [x,y,z].[x,a] = [true,false]
-   [ ] Index with container of more than one element [x,y,z][1,2]
-   [ ] Test that indexing with a boolean works a ternary operator [a,b][bool]
-   [ ] Add floating points (number) and conversions
-   [ ] ranges [x..y] 
-   [ ] infinite ranges [..y]
-   [ ] More advanced ranges, with step ?  (syntax?) x..y*3 is step 3 but to from x*3 to y*3 ? 
-   [ ] Even more advanced with functional filter, eg even or fibonachi 
-   [ ] range slicing
-   [ ] For each operator *x..y
-   [ ] Assigning a free clause association is a lambda `l: sqr(n):n*n`.   
-   [*] Make knowledge and namespace expressions, clauses associations 
-   [ ] Sub namespaces
-   [ ] Scalar operators on containers *[1,2]+2 = [1+1,2+1]
-   [ ] Scalar operators on ranges are applied to the limits (1..2)*2 = 2..4
-   [ ] Scalar operator on an all/any is all/any of its operands all([1,2])+2 = all([1+2,2+2]) (precedence correction)
-   [ ] Association operator on all operator: *[cat,fuzzy]:ginny = [cat:ginny,fuzzy:ginny]. See TODO in Association.cpp
-   [ ] Association on the other end: cat:*[gizmo, ginny] = [cat:gizzmo, cat:ginny]
-   [ ] Association on any operator `@[cat,dog]:max = cat:max | dog:max` or is itsimpy short hand for `@[cat:max,dog:max]`? Or maybe some notation with ⊆ except there is no rhs or lhs unless it is introduced here.  
        - [ ] and other away around `cat:@[ginny,gizmo]: ` .. either of the options above. 
-   [ ] All of and any of element in a comparison is a conjunection, disjunction respectively. 
-   [ ] Multiple all of/any of in a  comparison is a matrix conjunction/disjunction @[1,2]<@[3,4] = 1<3&1<4&2<3&2<4
-   [ ] Multi dimensional arrays (as a separate collection type)?
-   [ ] Apply functions on sequences, sets and arrays with for each 
    [ ] Like python everything a class, also everything is an expression, including expression types (introspection). TBD what type (besides type). It should be usable to check type of and cast to as a predicate. Perhaps types are just built in predicates and other built ins check `is`. Builtin `type(expression)` gives the id. This id can also be used internally, for instance for cast and other type limited exceptions 
- [ ] Advanced
-   [ ] Tautology protection cat: cat (or longer) is an error (use trace, check linked list for equal)
        Maybe it works automatically due to a (new/adjusted) inference engine:
        1. Match the query and generate a new query, replacing the predicates with the antecedents 
        2. Check if the result is the same as the query (loop protection and stop condition if no more predicates)
        (2b) if tracing this new result/query is what is printed 
        3. If not infer again, replacing each predicate with new antecedents.         
      The implementation might be strange though. Each iteration needs to Recursively infer operands(incl arguments)
       except predicates that still need to be matched, but with multiple equal assumption matches returned as a disjunction. 
       What if there are predicates in arguments that need to be recursively or iteratively inferred before mathcing? 

Example: 
```
> quicksort([1,5,3,2])
... quicksort([])+[1]+quicksort([5,3,2]) # antedent of first query 
... []+[1]+quicksort([3,2])+[5]+quicksort([]) # base case antecedent and antecedent of tail query
... [1]+quicksort([2])+[3]+quicksort([])+[5]+[] 
... [1]+quicksort([])+[2]+quicksort([])+[3]+[]+[5]
... [1]+[]+[2]+[]+[3,5] # no predicates, but can still be simplified
... [1,2,3,5]
[1,2,3,5] # cant be simplified further, is the answer 
```
This is more clear and more lazy evaluation functional than tracing into branches. The query might get big but 
then the stack is reset every time. For performance the importing thing is what is easiest to paralelize

-   [x] Prefix comparators for matching. 
-   [ ] Total ordering of Expressions with Hash 
-   [ ] For any operator `@` while infering  
-   [ ] Unary for any (element of) operator when matching
-   [ ] Match operations inverse math `$X+1 = 3: $X=3-1`, divide&multiply, power and inverse $X^2 = 4 : $X=4^1/2
-   [ ] Superset and subset operators  ⊆  ⊇  . Not sure what the ascii version is yet. maybe |= and &= ? 
-   [ ] For any & each operators also while matching (each sort of done with Tuple I guess)
-   [ ] Other MFINAE type matching or even some concept like syntax 
-   [ ] free pairs `a:b` 
-   [ ] Derive Clause from Pair or make an isClause() helper?
-      [ ] Maybe association causation :<- should be split to differentiate how they are simplified, but which operators. Maybe just internally and for the syntax only when parsing clauses. To reduce variant size it could be a boolean/enum/sub variant (pair, causation, range)
-   [ ] Lambdas are free clauses
-   [ ] [Fold expressions](https://wiki.haskell.org/Fold): `[list] + ...` means add all elements in the list (right fold) ... + [list] for left. For all binary operators. (NB left or right does not mean iterate backwards. Just order of recursion and applying the operator. Might also need an initial value same as operators with base or for empty lists. 3 ^ [list]...
-   [ ] Syntax for applying functions(lambdas?) (with two arguments?) `f(*[list], ...)` ? The `*` is for each but what would 
it mean if it was omitted? In this case the user may have to supply the empty list case
-   [ ] Namespace from `axiom:{set,}` preferably without the comma when parsing. Are commas ever needed for collections? for arguments?
-   [ ] dicts (sets of associations) work as structs
-   [*] set access with `.` (list done, full feature for unique) 
-   [ ] Bag as unordered list if it has any advantages for speed and such 
-   [ ] ^ ⊕ is XOR (only one of the whole set is true) and ** ↑ (like python & knuth ) for power
-   [ ] No unsigned integer & bit (ie `12&7=4`). Instead perhaps make some sort of bitmask ordered container or operator where 
[True, False] & [True, True] = [True&True, False&True]. Peraps it's just All: [*Left & *Right]. It would work more like numby matrix operations anyway. 
-   [ ] Down arrow notation for built in logarithm (inverse of up). Non unicode notation tbd perhaps \/ or // 
-   [ ] Substring matching "bla"+$S+"bla"="bladibla"? true:$S="di"
-   [ ] ! postfix operator for factorial 
-   [ ] sqrt prefix operator but what is the non unicode ? Implemented as shortcut for exponent(x, 0.5) 
-   [ ] square and cube uncide postfix for exponent(x,2) and exponent(x,3) 
-   [ ] Also on output of exponentiation, shorcuts for 2 operands and the 2nd is 0.5, 2 or 3
-   [ ] Variable operators `$$X` and `$*X` etc
- [ ] Native functions
-    [ ] Add native cast functions (id, int, float, str, list, set, dict)
-    [ ] Add native collections functions (size, empty, ordered, unique)
-    [ ] Add native collection casts
-    [ ] file IO
-    [ ] UI / stdin/out IO 
- [ ] LL Parser
-   [ ] Unit tests for simple grammars to create first/follow table
-   [ ] Implement LL(1) https://www.geeksforgeeks.org/types-of-parsers-in-compiler-design/
-   [ ] Consider LL(n)
-   [*] Clean up old parser 
-   [ ] Optional: extend BNF with elipsis `...` (also in its own) that repeats a rules terms until failure (and generates each - [ ] Custom functions
-    [ ] Import of custom native functions (eg math)
-    [ ] string functions 
-    [ ] file functions 
-    [ ] json parsing & dumping 
Interface
-    [ ] Command line options
-    [ ] Multi line input 
-    [ ] History 
-    [ ] Debugging 

Performance 
- [ ] Measure and clean up unnecessary copies (there may be a lot)
- [ ] add Reference (expression hidden type) to known knowledge instead/to postpone of a copy 
- [ ] Reduce Expression size to about 32 (2x index, 2x ptr) although a set is already 56 so maybe 64 is already tough
        - [ ] Find out why Set is so big (or the rest won't matter much). pbly multiset. 
        - [ ] Use a function ptr only callback for `Function` This might still reduce it from 72 to 64
        - [ ] Use a small string (0 terminated ptr only) for `String` and `Function` summary
- [ ] Also pass references to ranges, eg when slicing lists, matching with Tuple etc until a copy is necessary. Numpy like. 
- [ ] Parallelize, clean up the inference engine first if needed
- [ ] Fix unlocking the knowledge for built in functions in a thread safe way. 