#pragma once

#include "Syntax.h"

// This file defines the Backus Naur Form using the C++ Syntax 
// it is as this example of BNF itself using the BNF: https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form#BNF_representation_of_itself
// with three exception: 
//  1 single quotes strings are regular expression tokens
//  2 rule names and literal strings can contain any unicode character over 128 
//  3 To help the lexer, literal strings, regexes and rule names are one token including opening and closing quotes or brackets

namespace Interpreter 
{

Syntax BNF
{
    //   <syntax> ::= <rule> | <rule> <syntax>
    Rule("syntax", {Symbol("rule")}),
    Rule("syntax", {Symbol("rule"), Symbol("Syntax")}),
    //   <rule> ::= <opt-whitespace> "<" <rule-name> ">" <opt-whitespace> "::=" <opt-whitespace> <expression> <line-end>
    Rule("rule",   {Symbol("opt-whitespace"), Symbol("rule-name"), Symbol("opt-whitespace"), Literal("::="), Symbol("opt-whitespace"), Symbol("expression"), Symbol("line-end") }),
    //   <opt-whitespace> ::= " " <opt-whitespace> | ""
    Rule("opt-whitespace", {Regex("[ \\t]+")}),
    Rule("opt-whitespace", {epsilon}),
    // <expression> ::= <list> | <list> <opt-whitespace> "|" <opt-whitespace> <expression>
    Rule("expression", {Symbol("list")}),
    Rule("expression", {Symbol("list"), Symbol("opt-whitespace"), Literal("|"), Symbol("opt-whitespace"), Symbol("expression")}),
    // <line-end> ::= <opt-whitespace> <EOL> | <line-end> <line-end>
    Rule("line-end", {Regex("(\\s*\\n)+")}),    // NB avoiding self recursion here as well 
    // <list> ::= <term> | <term> <opt-whitespace> <list>
    Rule("list", {Symbol("term")}),
    Rule("list", {Symbol("term"), Symbol("opt-whitespace"), Symbol("list")}),
    // <term> ::= <literal> | "<" <rule-name> ">"
    Rule("term", {Symbol("literal")}),
    Rule("term", {Symbol("rule-name")}),
    // <literal> ::= '"' <text1> '"' | "'" <text2> "'"
    Rule("literal", {Symbol("string")}),
    Rule("literal", {Symbol("regex")}),
    // <text1> ::= "" | <character1> <text1>
    // <character> ::= <letter> | <digit> | <symbol>
    // <letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
    // <digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
    // <symbol> ::= "|" | " " | "!" | "#" | "$" | "%" | "&" | "(" | ")" | "*" | "+" | "," | "-" | "." | "/" | ":" | ";" | ">" | "=" | "<" | "?" | "@" | "[" | "\" | "]" | "^" | "_" | "`" | "{" | "}" | "~"
    // <character1> := <character> | "'"
    Rule("string", {Regex(R"regex("(\\"|[^"\n])*")regex")}),
    // <text2> ::= "" | <character2> <text2>
    // <character2> ::= <character> | '"'
    Rule("regex", {Regex(R"regex('(\\'|[^'\n])*')regex")}),
    // <rule-name> ::= <letter> | <rule-name> <rule-char>
    // <rule-char> ::= <letter> | <digit> | "-"
    Rule("rule-name", {Regex(R"regex(<([^\x00-\x7F]|[A-Za-z])([^\x00-\x7F]|\w)*>)regex")})
};

}
