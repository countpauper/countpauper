#pragma once
#include "BNF.h"

namespace Angel::Parser::BNF
{
    // literals
    Rule id{ "id", RegularExpression{"a-z+"} };
    Rule optionalWhitespace{"whitespace", Disjunction(Whitespace(), Nothing() ) };
    Rule integer{"integer", RegularExpression{"0-9+"} };
    Rule boolean{"boolean", Disjunction{Literal{"true"},Literal{"false"}} };

    // expressions
    Rule element{"element", Disjunction{Ref{id}, Ref{integer}, Ref{boolean} } };
    Rule expression{"expression", Disjunction{Ref{element}} };
    
    // Collections
    Rule elements{ "elements", Disjunction{Nothing(), Sequence{Ref{expression}, Loop{ Sequence{Ref{ optionalWhitespace }, Sequence{Literal(","), Ref{ optionalWhitespace }, Ref{expression}}}} } } };
    Rule sequence{"sequence", Sequence{ Literal("("), Ref(optionalWhitespace), Ref{ elements }, Ref{ optionalWhitespace }, Literal(")") } };
    Rule set     {"set", Sequence{ Literal("{"), Ref(optionalWhitespace), Ref{ elements }, Ref{ optionalWhitespace }, Literal("}") } };
    // Operations 

    // Knowledge rules
    Rule simplePredicate{"predicate", Ref(id) };
    Rule argumentedPredicate{"predicate()", Sequence{Ref(id),Ref(sequence)} };
    Rule predicate{"predicate[()]", Disjunction{Ref(simplePredicate),Ref(argumentedPredicate) } };  
    Rule clause{"clause", Ref{predicate} };  // TODO:  <consequent> ":" <antecedent>
    Rule clauses{"clauses", Loop{Ref(clause)} };
    Rule space{"namespace", Sequence{ Ref(id), Ref(optionalWhitespace), Literal{":"}, Ref(optionalWhitespace), Literal{"{"}, Ref(optionalWhitespace),Ref(clauses), Ref(optionalWhitespace),Literal{"}"}} };

    Rule knowledge{"knowledge", Disjunction{Ref{optionalWhitespace}, Ref{space}, Ref{clauses}} };
}