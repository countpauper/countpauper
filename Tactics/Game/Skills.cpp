#include "stdafx.h"
#include "Engine/Xml.h"
#include "Skills.h"

namespace Game
{

Skills::Skills(const std::vector<Skill>& skills):
    vector<Skill>(skills)
{
}

const Skill* Skills::Find(const std::wstring& name) const
{
	for (auto& skill : *this)
		if (skill.name == name)
			return &skill;
	return nullptr;
}
Skills::Skills(const std::string& fname)
{
	Load(fname);
}

void Skills::Parse(const xmlNode* node)
{
	for (const xmlNode *curNode = node; curNode; curNode = curNode->next)
	{
		if (curNode->type == XML_COMMENT_NODE)
		{
			continue;
		}
		if (curNode->type == XML_ELEMENT_NODE && Engine::Xml::hasTag(curNode, "skill"))
		{
			emplace_back(Skill(curNode));
		}
		else
		{

		}
	}
}

void Skills::Load(const std::string& fname)
{
	// initialize the library and verify dlll version 
	LIBXML_TEST_VERSION

	xmlDoc *doc = xmlReadFile(fname.c_str(), NULL, 0);
	if (doc == NULL)
		throw std::runtime_error("could not xml parse skills file");

	xmlNode *root_element = xmlDocGetRootElement(doc);
	for (xmlNode *curNode = root_element; curNode; curNode = curNode->next)
	{
		if (curNode->type == XML_COMMENT_NODE)
		{
			continue;
		}
		if (curNode->type == XML_ELEMENT_NODE && Engine::Xml::hasTag(curNode, "skills"))
		{
			Parse(curNode->children);
		}
		else
		{
			throw std::runtime_error("Invalid root node");
		}
	}
	xmlFreeDoc(doc);
	// cleanup global variables used
	xmlCleanupParser();
}




}