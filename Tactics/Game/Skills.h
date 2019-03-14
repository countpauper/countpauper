#pragma once
#include "Skill.h"

namespace Game
{

class Skills : public std::vector<Skill>
{
public:
    Skills() = default;
    Skills(const std::string& filename);
    const Skill* Find(const std::wstring& name) const;
private:
    Skills(const std::vector<Skill>& skills);
    void Load(const std::string& fn);
	void Parse(const xmlNode* node);
};

}