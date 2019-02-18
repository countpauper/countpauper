#include "stdafx.h"
#include <sstream>
#include <optional>
#include <map>
#include <ctype.h>
#include "Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Array.h"
#include "Logic/Sequence.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"

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

Operator PeekOperator(std::wistream& s)
{
	if (!s.good())
		return Operator::None;
	wchar_t c = s.peek();
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
		if (!iswalnum(c))
			return result;
		result.append(1, wchar_t(s.get()));
	}
	return result;
}

std::optional<bool> ParseBoolean(const std::wstring& tag)
{
	if (tag == L"true")
		return true;
	else if (tag == L"false")
		return false;
	else
		return std::optional<bool>();
}

// TODO: can be constructor of element after element/collection/expression split
Logic::Element ParseElement(const std::wstring& tag)
{
	if (tag.empty())
	{
		return Logic::Element();
	}
	else if (auto boolean = ParseBoolean(tag))
	{
		return Logic::boolean(*boolean);
	}
	else
	{
		return Logic::id(tag);
	}
}


// TODO: can be constructor of element
Logic::Element MakeExpression(Logic::Element&& left, Operator op, Logic::Element&& right)
{
	if (op == Operator::Comma)
	{
		return Logic::array(std::move(left), std::move(right));
	}
	auto id0 = left.Cast<Logic::Id>();
	auto pred1 = right.Cast<Logic::Predicate>();
	auto seq0 = left.Cast<Logic::Sequence>();
	auto pred0 = left.Cast<Logic::Predicate>();
	auto array1 = right.Cast<Logic::Array>();

	if ((id0) && (op == Operator::SequenceBegin))
	{	// predicate = <id> ( <seq> ) , also works if right is void
		return Logic::predicate(*id0, Logic::Sequence(std::move(right)));
	}
	else if ((pred0) && (array1) && (op == Operator::Colon))
	{	// clause = <pred> : <array>
		return Logic::clause(std::move(*pred0), std::move(*array1));
	}
	else if ((pred0) && (op == Operator::Colon))
	{	// clause = <pred> : <whatever>
		return Logic::clause(std::move(*pred0), Logic::Array(std::move(right)));
	}
	else if ((op == Operator::SequenceBegin) && (!left))
	{	// also works for right being an array or void
		return Logic::sequence(std::move(right));
	}
	else if ((op == Operator::SetBegin) && (!left))
	{	// also works for right being an array or void
		return Logic::set(std::move(right));
	}
	else
	{
		throw std::runtime_error("Unexpected operator");
	}

}

Logic::Element ParseExpression(std::wistream& stream, Operator openOperator, Operator previousOperator)
{
	SkipWhitespace(stream);
	auto tag = ReadTag(stream);	// TODO Element::operator<< , but sequence & expression not elements
	auto left = ParseElement(tag);
	while (stream.good())
	{
		SkipWhitespace(stream);
		auto op = PeekOperator(stream);
		if (op == Operator::None)
		{
			break;
		}
		else if (op <= previousOperator)
		{	// equal, because chains of , or other equivalent operators should be iterative

			break;
		}
		else if (ClosesSomethingElse(openOperator, op))
		{
			break;
		}
		stream.ignore();	// skip operator
		if (Closes(openOperator, op))
		{
			break;
		}
		if (Opens(op))
		{	
			left = MakeExpression(std::move(left), op, ParseExpression(stream, op, Operator::None));
		}
		else
		{	// TODO: different function altogether, so closes something else can be jus Closes(op)
			//	or more explicit about complete sack
			left = MakeExpression(std::move(left), op, ParseExpression(stream, Operator::None, op));
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
		Logic::Element e = ParseExpression(stream, Operator::None, Operator::None);
		if (e)
		{
			result.Know(std::move(e));
		}
	}
	return result;
}

}
}
