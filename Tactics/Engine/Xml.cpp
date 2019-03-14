#include "stdafx.h"
#include "Xml.h"
#include "Utils.h"

namespace Engine
{
namespace Xml
{

std::string str(const xmlChar* xmlString)
{
	return std::string((const char*)xmlString);
}

std::wstring wstr(const xmlChar* xmlString)
{
	if (!xmlString)
		throw std::invalid_argument("Null pointer xml string");
	int len = xmlStrlen(xmlString);
	if (len == 0)
		return std::wstring();

	int requiredCharacters = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)xmlString, len, 0, 0);
	if (requiredCharacters == 0)
	{
		int err = GetLastError();
		throw std::system_error(err, std::generic_category(), "Failed to convert XML string");
	}
	std::wstring result(requiredCharacters, L'\x0');
	int processedCharacters = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)xmlString, len, result.data(), requiredCharacters);
	if (processedCharacters != requiredCharacters)
		throw std::runtime_error("Unexpected number of wide characters in XML string");
	return result;
}

std::wstring text(const xmlNode* node)
{
	if (node->type != XML_TEXT_NODE)
		throw std::runtime_error("Expected a text node");
	if (node->children != nullptr)
		throw std::runtime_error("Expected a leaf text node");
	return wstr(node->content);
}

bool hasTag(const xmlChar* name, const char* tag)
{
	return xmlStrEqual(name, xmlCharStrdup(tag)) == 1;
}

bool hasTag(const xmlNode* node, const char* tag)
{
	return hasTag(node->name, tag);
}

bool hasTag(const xmlAttr* attr, const char* tag)
{
	return hasTag(attr->name, tag);
}

bool hasText(const xmlNode* node)
{
	if (node->type != XML_TEXT_NODE)
		return true;
	auto content = text(node);
	return (content.find_first_not_of(whitespace) != std::wstring::npos);
}

}
}