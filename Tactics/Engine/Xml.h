#pragma once

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string>

namespace Engine
{

	namespace Xml
	{
		std::string str(const xmlChar* xmlString);
		std::string text(const xmlNode* node);
		bool hasTag(const xmlChar* name, const char* tag);
		bool hasTag(const xmlNode* node, const char* tag);
		bool hasTag(const xmlAttr* attr, const char* tag);
		bool hasText(const xmlNode* node);
	}
}