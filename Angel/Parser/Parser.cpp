#include "stdafx.h"
#include <sstream>
#include <optional>
#include <ctype.h>
#include "Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Sequence.h"

namespace Angel
{
namespace Parser
{

static std::wstring operators(L"~!@#$%^&|=+-*/,:()[]{}<>.?!");
static std::wstring whitespace(L" \t\r\n");

enum class Operator : wchar_t
{
	None=0,
	SequenceBegin=L'(',
	SequenceEnd=L')'
};

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

Operator ReadOperator(std::wistream& s)
{
	if (!s.good())
		return Operator::None;
	wchar_t c = s.peek();
	if (operators.find(c) == std::wstring::npos)
		return Operator::None;
	s.ignore();
	return static_cast<Operator>(c);
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

Logic::Element ReadSequence(std::wistream& stream)
{
	Logic::Element result;
	while (stream.good())
	{
		SkipWhitespace(stream);
		auto op = ReadOperator(stream);
		if (op == Operator::SequenceEnd)
			return result;
		stream.ignore();	// TODO: read expressions in sequence
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

Logic::Element MakeExpression(Logic::Element&& first, Operator op, Logic::Element&& second)
{
	auto id = first.Cast<Logic::Id>();
	auto seq = second.Cast<Logic::Sequence>();
	if ((id) && (seq) && (op == Operator::SequenceBegin))
	{
		return Logic::predicate(*id, std::move(second));
	}
	else if (op == Operator::SequenceEnd)
	{
		return Logic::sequence();
	}
	else
	{
		throw std::runtime_error("Unexpected operator");
	}

}

Logic::Element ParseExpression(std::wistream& stream)
{
	SkipWhitespace(stream);
	auto tag = ReadTag(stream);	// TODO tag type and operator<<
	SkipWhitespace(stream);
	auto op = ReadOperator(stream);	// TODO operator type and operator <<
	if (op == Operator::None)
	{
		return ParseElement(tag);
	}
	else 
	{
		return MakeExpression(ParseElement(tag), op, ParseExpression(stream));
	}
}

Logic::Knowledge Parse(const std::wstring& text)
{
	Logic::Knowledge result;
	std::wstringstream stream(text);
	stream.exceptions(std::wistream::failbit | std::wistream::badbit);
	while (stream.good())
	{
		Logic::Element e = ParseExpression(stream);
		if (e)
		{
			result.Know(std::move(e));
		}
	}
	return result;
}

}
}
