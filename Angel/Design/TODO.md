## Quick todo / Rafactor
- [ ] Rewrite disjunction & conjunction as an Operation<> (although early out? Multiply also early out on 0? exponent on 1 or 0?)
- [ ] Predicate arguments are all single element comparisons and if they're not then it's shorthand for Equation{e}.
    Matching a predicate (or any list/container) is making a conjunction of these where the matched argument is filled in as second argument. These can be simplified to boolean or a hypothesis with remaining variables (or predicates)
- [ ] Make compare operators Comparators and functors that return a bool on two expressions instead of the compare<OP> functions
- [ ] Make the binary (and unary) operators also Operate() functors

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
in sequence instead of in tr)
-   [*] Parsing enough for axioms
-   [*] Parsing enough for clauses
-   [*] Parsing enough for predicates with arguments
-   [*] Parse set of axioms
-   [*] Parse variables 
-   [*] Parse conjunctions 
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
-   [ ] integer operators `+-*/^`
-   [ ] Inequality comparison, at least < for sort, perhaps != (should be easy in Expression at least)
-  [ ] Functional 
-   [*] lists
-   [ ] sets to the same level as lists (equality, var&tuple matching, operator. etc)
-   [ ] Be able to implement factorial like Haskell https://en.wikipedia.org/wiki/Haskell#Code_examples
    - [ ] With inequality match `factorial(<2): 1`
-   [ ] set and list operators `&|+-` (if both side containers?) 
-   [ ] Be able to implement quicksort like Hashell https://en.wikipedia.org/wiki/Haskell#Code_examples (x:xs means [x]+xs if xs is a list or as used here to split head:tail
-   [*] Membership operator x.{x,y} (done for List)
- Python
-   [ ] namespaces
-   [ ] Indexing lists (not sets, only ordered/indexible)  [x,y,z][1]
-   [ ] Add floating points and conversions
-   [ ] ranges [x..y] 
-   [ ] infinite ranges [..y]
-   [ ] More advanced ranges, with step ?  (syntax?) x..y*3 is step 3 but to from x*3 to y*3 ? 
-   [ ] Even more advanced with functional filter, eg even or fibonachi 
-   [ ] range slicing
-   [ ] For each operator *x..y
-   [ ] Assigning a free clause association is a lambda `l: sqr(n):n*n`.   
-   [*] Make knowledge and namespace expressions, clauses associations 
-   [ ] Sub namespaces
-   [ ] Figure out numpy like stuff. Is using @ * to multiply or fn() all items in a container sufficient?  
-   [ ] Multi dimensional arrays (as a separate collection type)?
-   [ ] Apply functions on sequences, sets and arrays with for each 
- [ ] Advanced
-   [ ] Tautology protection cat: cat (or longer) is an error (use trace, check linked list for equal)
-   [ ] Prefix comparators for matching. 
-   [ ] For any operator `@` while infering  
-   [ ] Unary for any (element of) operator when matching
-   [ ] Superset and subset operators  ⊆  ⊇  . Not sure what the ascii version is yet. maybe |= and &= ? 
-   [ ] For any & each operators also while matching (each sort of done with Tuple I guess)
-   [ ] Other MFINAE type matching or even some concept like syntax 
-   [ ] free pairs `a:b` 
-   [ ] Derive Clause from Pair or make an isClause() helper?
-   [ ] Lambdas are free clauses
-   [ ] [Fold expressions](https://wiki.haskell.org/Fold): `[list] + ...` means add all elements in the list (right fold) ... + [list] for left. For all binary operators. (NB left or right does not mean iterate backwards. Just order of recursion and applying the operator. Might also need an initial value same as operators with base or for empty lists. 3 ^ [list]...
-   [ ] Syntax for applying functions(lambdas?) (with two arguments?) `f(*[list], ...)` ? The `*` is for each but what would 
it mean if it was omitted? In this case the user may have to supply the empty list case
-   [ ] Namespace from `axiom:{set,}` preferably without the comma when parsing. Are commas ever needed for collections? for arguments?
-   [ ] dicts (sets of associations) work as structs
-   [ ] set access with `.` (list done, full feature for unique) 
-   [ ] Bag as unordered list if it has any advantages for speed and such 
-   [ ] ^ ⊕ is XOR (only one of the whole set is true) and ** ↑ (like python & knuth ) for power
-   [ ] Down arrow notation for built in logarithm (invser of up). Non unicode notation tbd perhaps \/ or // 
-   [ ] ! postfix operator for factorial 
-   [ ] sqrt prefix operator but what is the non unicode ? Implemented as shortcut for exponent(x, 0.5) 
-   [ ] square and cube uncide postfix for exponent(x,2) and exponent(x,3) 
-   [ ] Also on output of exponentiation, shorcuts for 2 operands and the 2nd is 0.5, 2 or 3
-   [ ] Optimization: Reduce copies when matching and inferring and add references to known knowledge 
-   [ ] Optimization: Also pass references to ranges, eg when slicing lists, matching with Tuple etc until a copy is necessary. 
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

