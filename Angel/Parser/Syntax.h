#pragma once
#include "BNF.h"
#include "LogicParser.h"
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
    Rule id( "id", RegularExpression{"[a-z_\\u0080-\\uDB7Fa][a-z0-9_\\u0080-\\uDB7Fa]*"}, ParseId );
    Rule integer( "integer", RegularExpression{"-*[0-9]+"}, ParseInteger );
    Rule boolean("boolean", Disjunction{ Literal{"true"},Literal{"false"} }, ParseBoolean);

    // expressions
    Rule element( "element", Disjunction{ Ref(boolean), Ref(integer), Ref(id) } );
    Recursive naked_expression( "naked expression", Disjunction{ Ref(collection), Ref(predicate), Ref(element) } );
    Rule braced_expression("braced expression", Sequence{ Merge, Literal("("), Whitespace(0), Ref(expression), Whitespace(0), Literal(")") }, static_cast<Rule::ConstructFn>(ConstructBracedExpression));
    Recursive expression_("expression", Disjunction{ Ref(braced_expression), Ref(naked_expression) });

    // Collections
    //Rule sequence_elements("sequence elements", Sequence{Ref(expression), Loop(Sequence{ Whitespace(0), Literal(","), Whitespace(0), Ref(expression) })});
    Rule empty_sequence{ "empty sequence", Sequence{Merge, Literal("("), Whitespace(0), Literal(")") } };
    Declare comma_sequence("comma sequence");
    Rule comma_sequence_("comma sequence", Sequence{Merge, Ref(expression), Whitespace(0), Literal(","), Whitespace(0), 
        Disjunction{ Ref(comma_sequence), Ref(expression)}});
    Rule braced_sequence("braced sequence", Sequence{Merge, Literal("("), Whitespace(0), Ref(expression),
        Loop(Sequence{Merge, Whitespace(0), Literal(","), Whitespace(0), Ref(expression) }, Merge), Whitespace(0), Literal(")") }, static_cast<Rule::ConstructFn>(ConstructBracedSequence));

    Rule sequence("sequence", Disjunction{
        Ref(empty_sequence),
        Ref(braced_sequence),
        Ref(comma_sequence) }, static_cast<Rule::ConstructFn>(ConstructSequence));

    Rule set_elements("set elements", Sequence{Merge, Ref(expression), Loop(Sequence{Merge, Whitespace(0), Literal(","), Whitespace(0), Ref(expression) }, Merge) });
    Rule set     {"set", Sequence{Merge, Literal("{"), Whitespace(0), Ref{ set_elements }, Whitespace(0), Literal("}")} };
    Recursive collection_ { "collection", Disjunction{ Ref{set}, Ref{sequence}} };

    // Operations 

    // Knowledge rules
    Rule arguments{ "arguments", Disjunction{ Ref(empty_sequence), Ref(braced_sequence)} };
    Rule predicate_("predicate", Sequence{Merge, Ref(id),Ref(arguments)}, static_cast<Rule::ConstructFn>(ConstructPredicate));
    Rule clause{"clause", Ref{predicate} };  // TODO:  <consequent> ":" <antecedent>
    Rule clauses{ "clauses", Sequence{Merge, Ref(clause), Loop(Sequence{Merge, Whitespace(1), Ref(clause)}, Merge ) }};
    Rule space{ "namespace", Sequence{Merge, Ref(id), Whitespace(0), Literal{":"}, Whitespace(0), Literal{"{"}, Whitespace(0),Ref(clauses), Whitespace(0),Literal{"}"}} };

    Rule knowledge( "knowledge", Sequence{Merge, Whitespace(0), Disjunction{Ref{space}, Ref{clauses}, Whitespace(0) } }, static_cast<Rule::ConstructFn>(ConstructKnowledge));
}