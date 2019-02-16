#include "stdafx.h"
#include <sstream>
#include <optional>
#include <map>
#include <ctype.h>
#include "Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Sequence.h"
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
	SequenceEnd,
};

static const std::map<wchar_t, Operator> translateOperator = {
	{L',', Operator::Comma},
	{L'(', Operator::SequenceBegin},
	{L')', Operator::SequenceEnd},
	{L':', Operator::Colon}
};

bool IsPostfix(Operator o)
{
	return o == Operator::SequenceEnd;
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

// TODO: can be constructor of element
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
Logic::Element MakeExpression(Logic::Element&& first, Operator op, Logic::Element&& second)
{
	if (op == Operator::Comma)
	{
		return Logic::sequence(std::move(first), std::move(second));
	}

	auto id = first.Cast<Logic::Id>();
	auto pred0 = first.Cast<Logic::Predicate>();
	auto pred1 = second.Cast<Logic::Predicate>();
	auto seq = second.Cast<Logic::Sequence>();
	
	if ((id) && (seq) && (op == Operator::SequenceBegin))
	{	// predicate = <id> ( <seq> )
		return Logic::predicate(*id, std::move(*seq));
	}
	else if ((pred0) && (seq) && (op == Operator::Colon))
	{	// clause = <pred> : <seq>
		return Logic::clause(std::move(*pred0), std::move(*seq));
	}
	else if ((pred0) && (pred1) && (op == Operator::Colon))
	{	// clause = <pred> : <pred>
		return Logic::clause(std::move(*pred0), Logic::Sequence(std::move(second)));
	}
	else if ((op == Operator::SequenceEnd) && (first))
	{	// sequence = <element> )
		if (second)
			throw std::runtime_error("Orphaned element after sequence");
		return Logic::sequence(std::move(first));
	}
	else if ((op == Operator::SequenceEnd) && (!first))
	{	// null sequence = )
		if (second)
			throw std::runtime_error("Orphaned element after empty sequence");
		return Logic::sequence();
	}
	else
	{
		throw std::runtime_error("Unexpected operator");
	}

}

Logic::Element ParseExpression(std::wistream& stream, Operator previousOperator)
{
	SkipWhitespace(stream);
	auto tag = ReadTag(stream);	// TODO tag type and operator<<
	auto left = ParseElement(tag);
	while (stream.good())
	{
		SkipWhitespace(stream);
		auto op = PeekOperator(stream);	// TODO operator type and operator <<
		if (op <= previousOperator)
		{
			break;
		}
		stream.ignore();	// skip operator
		if (IsPostfix(op))
		{
			left = MakeExpression(std::move(left), op, Logic::Element());
		}
		else
		{
			left =MakeExpression(std::move(left), op, ParseExpression(stream, op));
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
		Logic::Element e = ParseExpression(stream, Operator::None);
		if (e)
		{
			result.Know(std::move(e));
		}
	}
	return result;
}

}
}
