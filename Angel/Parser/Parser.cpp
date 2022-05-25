#include "stdafx.h"
#include <sstream>
#include <optional>
#include <map>
#include <ctype.h>
#include "Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Sequence.h"
#include "Logic/Conjunction.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"
#include "Logic/Object.h"

namespace Angel
{
namespace Parser
{

static std::wstring operators(L"~!@#$%^&|=+-*/,:()[]{}<>.?!");
static std::wstring whitespace(L" \t\r\n");

enum class Operator 
{	// in order of precedence low to high
	None=0,
	Colon,
	Comma,
	SequenceBegin,	
	SequenceEnd,	// closing operators return using IsClosing
	SetBegin,
	SetEnd,
};

static const std::map<wchar_t, Operator> translateOperator = {
	{L',', Operator::Comma},
	{L'(', Operator::SequenceBegin},
	{L')', Operator::SequenceEnd},
	{L'{', Operator::SetBegin},
	{L'}', Operator::SetEnd},
	{L':', Operator::Colon}
};

// TODO: operator properties should be OO

std::string Description(Operator op)
{
	static const std::map<Operator, std::string> description = {
    { Operator::None, "<None>" },
    { Operator::Comma, "," },
	{ Operator::SequenceBegin, "(" },
	{ Operator::SequenceEnd, "(" },
	{ Operator::SetBegin, "{" },
	{ Operator::SetEnd, "}" },
	{ Operator::Colon, ":" }
	};
	return description.at(op);
}

bool Opens(Operator op)
{
	return (op == Operator::SequenceBegin) ||
		   (op == Operator::SetBegin);
}

bool Closes(Operator open, Operator close)
{
	return ((open == Operator::SequenceBegin) && (close == Operator::SequenceEnd)) ||
		((open == Operator::SetBegin) && (close == Operator::SetEnd));
}

bool ClosesSomethingElse(Operator open, Operator close)
{
	return ((open != Operator::SequenceBegin) && (close == Operator::SequenceEnd)) ||
		((open != Operator::SetBegin) && (close == Operator::SetEnd));
}

bool IsUnary(Operator o)
{
	return false;
}

void SkipWhitespace(std::wistream& s)
{
	while (s.good())
	{
		wchar_t c = s.peek();
		if (whitespace.find(c) == std::wstring::npos)
			return;
		s.ignore();
	}
}

Operator PeekOperator(std::wistream& stream)
{
	SkipWhitespace(stream);
	if (!stream.good())
		return Operator::None;
	wchar_t c = stream.peek();
	if (operators.find(c) == std::wstring::npos)
		return Operator::None;
	// TODO: double character operators and ignore() them later
	return translateOperator.at(c);
}

std::wstring ReadTag(std::wistream& s)
{
	std::wstring result;
	while (s.good())
	{
		wchar_t c = s.peek();
		if (iswalnum(c) ||
			((c == L'-') && (result.empty())))
		{
			result.append(1, wchar_t(s.get()));
		}
		else
		{
			return result;
		}
	}
	return result;
}

// TODO: can be constructor of element
Logic::Object MakeExpression(Logic::Object&& left, Operator op, Logic::Object&& right)
{
	auto id0 = left.As<Logic::Id>();
	auto pred1 = right.As<Logic::Predicate>();
    auto seq0 = left.As<Logic::Sequence>();
    auto set0 = left.As<Logic::Set>();
    auto pred0 = left.As<Logic::Predicate>();
	auto seq1 = right.As<Logic::Sequence>();

    if ((seq0) && (op == Operator::Comma))
    {
        return Logic::sequence(std::move(*seq0), std::move(right));
    }
    else if ((set0) && (op == Operator::Comma))
    {
        return Logic::set(std::move(*set0), std::move(right));
    }
	if ((id0) && (op == Operator::SequenceBegin))
	{	// predicate = <id> ( <seq> ) , also works if right is void
		return Logic::predicate(*id0, Logic::Sequence(std::move(right)));
	}
	else if ((pred0) && (seq1) && (op == Operator::Colon))
	{	// clause = <pred> : <array>
		return Logic::clause(std::move(*pred0), Logic::conjunction(std::move(*seq1)));
	}
	else if ((pred0) && (op == Operator::Colon))
	{	// clause = <pred> : <whatever>
		return Logic::clause(std::move(*pred0), std::move(right));
	}
	else if ((op == Operator::SequenceBegin) && (!left))
	{	// also works for right being an array or void
        if (right.As<Logic::Sequence>())
            return std::move(right);
        else
		    return Logic::sequence(std::move(right));
	}
	else if ((op == Operator::SetBegin) && (!left))
	{	// also works for right being an array or void
        auto seq = right.As<Logic::Sequence>();
        if (seq)
            return Logic::set(std::move(*seq));
        else
            return Logic::set(std::move(right));
	}
	else
	{
		throw SyntaxError((std::string("Unexpected operator ")+Description(op)).c_str());
	}
}


Logic::Object ParseElement(std::wistream& stream)
{
	// TODO Object::operator<< ?
	SkipWhitespace(stream);
	auto tag = ReadTag(stream);	
	return Logic::Object(tag);
}

Logic::Object ParseExpression(std::wistream& stream, Operator previousOperator, Operator openOperator);

Logic::Object ParseCollection(std::wistream& stream, Operator openOperator)
{
    Logic::Object left;
    Logic::Sequence seq;
    while (!stream.bad())
	{
		auto op = PeekOperator(stream);
        if (ClosesSomethingElse(openOperator, op))
		{
			throw std::runtime_error((std::string("Unexpected ") + Description(op)).c_str());
		}
		stream.ignore();	// skip operator
		if (Closes(openOperator, op))
		{
			break;
		}
		else if (Opens(op))
		{
			left = MakeExpression(std::move(left), op, ParseCollection(stream, op));
		}
		else
		{
			left = MakeExpression(std::move(left), op, ParseExpression(stream, op, openOperator));
		}
	}
	return left;
}

Logic::Object ParseExpression(std::wistream& stream, Operator previousOperator, Operator openOperator)
{
	auto left = ParseElement(stream);
	while (!stream.bad())
	{
		auto op = PeekOperator(stream);
		if (op == Operator::None)
		{
            break; // done
		}
        else if (op == Operator::Comma)
        {
            break;
        }
        else if (op <= previousOperator)
		{	// equal, because chains of , or other equivalent operators should be iterative
			break;
		}
		else if (ClosesSomethingElse(openOperator, op))
		{
			throw std::runtime_error(std::string("Unexpected ") + Description(op));
		}
		else if (Closes(openOperator, op))
		{	// end of collection is end of expression
			break;
		}
		stream.ignore();	// skip operator
		if (Opens(op))
		{	
			left = MakeExpression(std::move(left), op, ParseCollection(stream, op));
		}
		else
		{	
			left = MakeExpression(std::move(left), op, ParseExpression(stream, op, openOperator));
		}
	}
	return left;
}

Logic::Knowledge Parse(const std::wstring& text)
{
	Logic::Knowledge result;
	std::wstringstream stream(text);
	stream.exceptions(std::wistream::failbit | std::wistream::badbit);
	while (stream.good())
	{
		Logic::Object e = ParseExpression(stream, Operator::None, Operator::None);
		if (e)
		{
			result.Know(std::move(e));
		}
	}
	return result;
}

std::wostream& operator<<(std::wostream& s, const Logic::Object& o)
{
    return s;
}

std::wistream& operator>>(std::wistream& s, Logic::Object& o)
{
    o = ParseExpression(s, Operator::None, Operator::None);
    return s;
}

}
}
