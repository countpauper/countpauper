#include "stdafx.h"
#include <sstream>
#include <ctype.h>
#include "Parser.h"
#include "Logic/Predicate.h"

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

std::wistream& operator>>(std::wistream& stream, Logic::Element& e)
{
	SkipWhitespace(stream);
	auto tag = ReadTag(stream);	// TODO tag type and operator<<
	SkipWhitespace(stream);
	auto op = ReadOperator(stream);	// TODO operator type and operator <<
	if (op == Operator::None)
	{
		if (!tag.empty())
		{
			e = Logic::id(tag);
		}
	}
	else if (op == Operator::SequenceBegin)
	{
		auto s = ReadSequence(stream);
		e = Logic::predicate(tag, std::move(s));
	}
	return stream;
}

Logic::Knowledge Parse(const std::wstring& text)
{
	Logic::Knowledge result;
	std::wstringstream stream(text);
	stream.exceptions(std::wistream::failbit | std::wistream::badbit);
	while (stream.good())
	{
		Logic::Element e;
		stream >> e;
		if (e)
		{
			result.Know(std::move(e));
		}
	}
	return result;
}

}
}
