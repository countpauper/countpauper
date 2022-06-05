#pragma once
#include "BNF.h"

namespace Angel::Parser::BNF
{
    // literals
    Rule id( "id", RegularExpression{"[a-z][a-z0-9_]*"} );
    Rule optionalWhitespace( "whitespace", Disjunction(Whitespace(), Nothing()) );
    Rule integer( "integer", RegularExpression{"-*[0-9]+"} );
    Rule boolean("boolean", Disjunction{ Literal{"true"},Literal{"false"} });

    // expressions
    Declare collection("collection");
    Rule element( "element", Disjunction{ Ref(boolean), Ref(integer), Ref(id) } );
    Rule expression( "expression", Disjunction{ Ref(collection), Ref(element) } );

    // Collections
    Rule sequence_elements("sequence elements", Sequence{ Ref(expression), Ref(optionalWhitespace), Literal(","), Ref(optionalWhitespace), Ref(expression),
        Loop(Sequence{ Ref(optionalWhitespace), Literal(","), Ref(optionalWhitespace), Ref(expression) }) });

    Rule empty_sequence{ "empty sequence", Sequence{ Literal("("), Ref(optionalWhitespace), Literal(")") } };
    Rule single_sequence("single sequence", Sequence{ Literal("("), Ref(optionalWhitespace), Ref(expression), Ref(optionalWhitespace), Literal(")") });
    Rule braces_sequence("braces sequence", Sequence{ Literal("("), Ref(optionalWhitespace), Ref(sequence_elements), Ref(optionalWhitespace), Literal(")") });
    Rule sequence( "sequence", Disjunction{ Ref(empty_sequence), Ref(single_sequence), Ref(braces_sequence)});

    Rule set     {"set", Sequence{ Literal("{"), Ref(optionalWhitespace), Ref{ sequence_elements }, Ref{ optionalWhitespace }, Literal("}") } };
    Rule collection_ { "collection", Disjunction{ Ref{set}, Ref{sequence}} };

    // Operations 

    // Knowledge rules
    Rule simplePredicate{"predicate", Ref(id) };
    Rule argumentedPredicate{"predicate()", Sequence{Ref(id),Ref(sequence)} };
    Rule predicate{"predicate[()]", Disjunction{Ref(simplePredicate),Ref(argumentedPredicate) } };  
    Rule clause{"clause", Ref{predicate} };  // TODO:  <consequent> ":" <antecedent>
    Rule clauses{"clauses", Loop{Ref(clause)} };
    Rule space{"namespace", Sequence{ Ref(id), Ref(optionalWhitespace), Literal{":"}, Ref(optionalWhitespace), Literal{"{"}, Ref(optionalWhitespace),Ref(clauses), Ref(optionalWhitespace),Literal{"}"}} };

    Rule knowledge{"knowledge", Disjunction{Ref{space}, Ref{clauses}, Ref{ optionalWhitespace } } };
}