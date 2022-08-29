#pragma once
#include "BNF.h"

/*
elements: 
id -> lowercase lowercase digits and underscore, starting with not a digit 
integer -> optional - numbers 
boolean -> true or false 
variable -> uppercase lowrcase|digits|underscore
float -> [-] digits e|E [-] digits | "inf"
element -> boolean | id | integer | float | predicate | variable
expression -> element | computation | collection
collection -> sequence | range 
expression -> ( expression ) 
computation -> summation | conjunction | disjunction | sequence | multiplication | division | comparison | negation | indexation
division -> expresion / expression 
summation -> expression + expression [...]
conjunction -> expression & expression [...]
disjunction -> expression | expression [...]
multiplication -> expression * expression [...]
indexation -> expression "[" expression "]"
negation -> - expression 
comparison -> equality | greater | lesser | greater_or_equal | lesser_or_equal | element 
equality -> expression = expression
element -> expression @ expression 
range -> open_range | closed_range | half_open_range_start | half_open_range_end
closed_range -> integer...integer
half_open_range_start -> ...integer
half_open_range_end -> integer...
predicate -> id ["(" sequence ")]
clause -> predicate : expression
*/
namespace Angel::Parser::BNF
{
    Declare predicate("predicate");
    Declare expression("expression");
    Declare collection("collection");

    // literals
    Rule id( "id", RegularExpression{"[a-z_\\u0080-\\uDB7Fa][a-z0-9_\\u0080-\\uDB7Fa]*"} );
    Rule integer( "integer", RegularExpression{"-*[0-9]+"} );
    Rule boolean("boolean", Disjunction{ Literal{"true"},Literal{"false"} });

    // expressions
    Rule element( "element", Disjunction{ Ref(boolean), Ref(integer), Ref(id) } );
    Recursive naked_expression( "naked expression", Disjunction{ Ref(collection), Ref(predicate), Ref(element) } );
    Rule braced_expression("braced expression", Sequence{ Literal("("), Whitespace(0), Ref(expression), Whitespace(0), Literal(")") });
    Recursive expression_("expression", Disjunction{ Ref(braced_expression), Ref(naked_expression) });

    // Collections
    //Rule sequence_elements("sequence elements", Sequence{Ref(expression), Loop(Sequence{ Whitespace(0), Literal(","), Whitespace(0), Ref(expression) })});
    Rule empty_sequence{ "empty sequence", Sequence{ Literal("("), Whitespace(0), Literal(")") } };
    Declare comma_sequence("comma sequence");
    Rule comma_sequence_("comma sequence", Sequence{ Ref(expression), Whitespace(0), Literal(","), Whitespace(0), 
        Disjunction{ Ref(comma_sequence), Ref(expression)} });
    Rule braced_sequence("braced sequence", Sequence{ Literal("("), Whitespace(0), Ref(expression),
        Loop { Sequence{ Whitespace(0), Literal(","), Whitespace(0), Ref(expression) }}, Whitespace(0), Literal(")") });

    Rule sequence("sequence", Disjunction{
        Ref(empty_sequence),
        Ref(braced_sequence),
        Ref(comma_sequence) });

    Rule set_elements("set elements", Sequence{ Ref(expression), Loop(Sequence{ Whitespace(0), Literal(","), Whitespace(0), Ref(expression) }) });
    Rule set     {"set", Sequence{ Literal("{"), Whitespace(0), Ref{ set_elements }, Whitespace(0), Literal("}") } };
    Recursive collection_ { "collection", Disjunction{ Ref{set}, Ref{sequence}} };

    // Operations 

    // Knowledge rules
    Rule arguments{ "arguments", Disjunction{ Ref(empty_sequence), Ref(braced_sequence)} };
    Rule predicate_{"predicate", Sequence{Ref(id),Ref(arguments)} };
    Rule clause{"clause", Ref{predicate} };  // TODO:  <consequent> ":" <antecedent>
    Rule clauses{ "clauses", Sequence{ Ref(clause), Loop{ Sequence{ Whitespace(1), Ref(clause) } } } };
    Rule space{"namespace", Sequence{ Ref(id), Whitespace(0), Literal{":"}, Whitespace(0), Literal{"{"}, Whitespace(0),Ref(clauses), Whitespace(0),Literal{"}"}} };

    Rule knowledge{"knowledge", Sequence{ Whitespace(0), Disjunction{Ref{space}, Ref{clauses}, Whitespace(0) } }};
}