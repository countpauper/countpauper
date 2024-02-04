#include "Xml.h"
#include "Utils.h"
#include <string>

namespace Engine
{
namespace Xml
{

std::string str(const xmlChar* xmlString)
{
	return std::string((const char*)xmlString);
}

std::string text(const xmlNode* node)
{
	if (node->type != XML_TEXT_NODE)
		throw std::runtime_error("Expected a text node");
	if (node->children != nullptr)
		throw std::runtime_error("Expected a leaf text node");
	return str(node->content);
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
	return (content.find_first_not_of(whitespace) != std::string::npos);
}

}
}