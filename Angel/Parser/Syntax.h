#pragma once
#include "BNF.h"

namespace Angel::Parser
{
    // literals
    Rule id{ RegularExpression{"a-z+"} };
    Rule optionalWhitespace{ Disjunction{Whitespace(), Nothing() } };
    Rule integer{ RegularExpression{"0-9+"} };
    Rule boolean{ Disjunction{Literal{"true"},Literal{"false"}} };

    // expressions
    Rule element{ Disjunction{RuleRef{id}, RuleRef{integer}, RuleRef{boolean} } };
    Rule expression{ Disjunction{RuleRef{element}} };
    
    // Collections
    Rule elements{ Disjunction{Nothing(), Sequence{RuleRef{expression}, Loop{ RuleRef{ optionalWhitespace }, Sequence{Literal(","), RuleRef{ optionalWhitespace }, RuleRef{expression}}}} } };
    Rule sequence{ Sequence{ Literal("("), RuleRef(optionalWhitespace), RuleRef{ elements }, RuleRef{ optionalWhitespace }, Literal(")") } };
    Rule set     { Sequence{ Literal("{"), RuleRef(optionalWhitespace), RuleRef{ elements }, RuleRef{ optionalWhitespace }, Literal("}") } };
    // Operations 

    // Knowledge rules
    Rule simplePredicate{ RuleRef(id) };
    Rule argumentedPredicate{ Sequence{RuleRef(id),RuleRef(sequence)} };
    Rule predicate{ Disjunction{RuleRef(simplePredicate),RuleRef(argumentedPredicate) } };  
    Rule clause{ RuleRef{predicate} };  // TODO:  <consequent> ":" <antecedent>
    Rule clauses{ Loop{RuleRef(clause)} };
    Rule space{ Sequence{ RuleRef(id), RuleRef(optionalWhitespace), Literal{":"}, RuleRef(optionalWhitespace), Literal{"{"}, RuleRef(optionalWhitespace),RuleRef(clauses), RuleRef(optionalWhitespace),Literal{"}"}} };

    Rule root{ Disjunction{RuleRef{optionalWhitespace}, RuleRef{space}, RuleRef{clauses}} };
}