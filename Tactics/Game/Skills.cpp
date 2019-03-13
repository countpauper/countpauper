#include "stdafx.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <codecvt>
#include "Engine/CSV.h"
#include "Engine/from_string.h"
#include "Engine/utils.h"
#include "Engine/Image.h"
#include "Skills.h"
#include "Attack.h"
#include "Affect.h"
#include "Move.h"
#include "Actor.h"
#include "Target.h"
#include "Item.h"

namespace Game
{
    Skill::Skill() :
        mp(0),
        range(0),
        trigger(Trigger::None),
        attribute(Attribute::None),
        resist(Attribute::None),
        offset(0),
        weapon(Type::Weapon::Style::All)
    {
    }

    Skill::Skill(const std::wstring name, Trigger trigger, Type::Weapon::Style weapon, Trajectory singleTrajector, int damage) :
        mp(2),
        range(0),
        trigger(trigger),
        name(name),
        attribute(Attribute::Strength),
        resist(Attribute::None),
        offset(0),
        weapon(weapon),
        type(std::make_shared<Melee>())
    {
        static_cast<Melee&>(*type).damage = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
        trajectory.insert(singleTrajector);
        target.insert(Targeting::Swing);
    }


    Skill::~Skill() = default;

    Action* Skill::CreateAction(const Actor& actor, const Target& target, Trajectory trajectory) const
    {
        if (type)
            return type->CreateAction(*this, actor, target, trajectory);
        else
            return nullptr;
    }

    Bonus Skill::GetChance(const Score& level) const
    {
        if (chance.empty())
            return Bonus(100);
        else
        {
            auto index = level.Value();
            if (index == 0)
            {
                return Bonus();
            }
            else if (index > chance.size())
            {
                return Bonus(level.Description(), chance.back());
            }
            else
            {
                return Bonus(level.Description(), chance.at(index-1));
            }
        }
    }

    Action* Skill::Move::CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const
    {
        return new ::Game::Move(actor, target.GetPosition(), skill, trajectory);
    }

    Skill::Melee::Melee() :
        attribute(Attribute::Strength)
    {
    }

    Action* Skill::Melee::CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const
    {
        return new Attack(actor, dynamic_cast<const Actor&>(target), skill, trajectory);
    }

    Bonus Skill::Melee::DamageBonus(const Score& skillScore)
    {
        if (damage.empty())
            return Bonus();
        auto index = skillScore.Value();
        if (index == 0)
            return Bonus();
        else if (index > damage.size())
            return Bonus(skillScore.Description(), damage.back());
        else
            return Bonus(skillScore.Description(), damage.at(index-1));
    }

    Action* Skill::Affect::CreateAction(const Skill& skill, const Actor& actor, const Target& target, Trajectory trajectory) const
    {
        return new ::Game::Affect(actor, dynamic_cast<const Actor&>(target), skill, trajectory);
    }

    bool Skill::Match(const std::wstring& category) const
    {
        return categories.count(category) > 0;
    }

    bool Skill::Require(const Weapon* item) const
    {
        if (weapon == ::Game::Type::Weapon::Style::None)
            return item == nullptr;
        else
            return item && item->Match(weapon);
    }

    bool Skill::Combo(const Skill& previous) const
    {
        if (trigger != Trigger::Combo)
            return false;
        return Follows(previous);
    }

    bool Skill::Follows(const Skill& previous) const
    {
        for (const auto& follow : follows)
        {
            if (previous.categories.count(follow))
                return true;
        }
        return false;
    }

    bool Skill::Counter(const Skill& previous) const
    {
        if ((trigger != Trigger::Defend) && (trigger != Trigger::React))
            return false;
        return Follows(previous);
    }

    unsigned Skill::Id() const
    {
        static_assert(sizeof(unsigned) == sizeof(this), "Failed to use skill pointer as id");
        return unsigned(this);
    }

    Engine::Image Skill::Icon() const
    {
        return *icon;
    }

    void Skill::LoadIcon(const std::wstring& fname)
    {
        // TODO: unique pointer, but need move constructor for skills then
        icon = std::make_shared<Engine::Image>(fname);
    }

    bool Skill::IsActive() const
    {
        return (trigger == Trigger::Act);
    }

    bool Skill::IsAttack() const
    {
        return IsType<Melee>();
    }

    bool Skill::IsMove() const
    {
        return IsType<Move>();
    }

    bool Skill::IsWait() const
    {
        return type == nullptr;
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

    std::wstring xmlWStr(const xmlChar* xmlString)
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

    std::string xmlStr(const xmlChar* xmlString)
    {
        return std::string((const char*)xmlString);
    }

    std::wstring xmlText(const xmlNode* node)
    {
        if (node->type != XML_TEXT_NODE)
            throw std::runtime_error("Expected a text node");
        if (node->children != nullptr)
            throw std::runtime_error("Expected a leaf text node");
        return xmlWStr(node->content);
    }

    bool xmlTagCompare(const xmlChar* name, const char* tag)
    {
        return xmlStrEqual(name, xmlCharStrdup(tag)) == 1;
    }

    bool xmlTagCompare(const xmlNode* node, const char* tag)
    {
        return xmlTagCompare(node->name, tag);
    }

    bool xmlTagCompare(const xmlAttr* attr, const char* tag)
    {
        return xmlTagCompare(attr->name, tag);
    }

    bool IsEmptyText(const xmlNode* node)
    {
        if (node->type != XML_TEXT_NODE)
            return false;
        auto content = xmlText(node);
        return (content.find_first_not_of(Engine::whitespace) == std::wstring::npos);
    }


void Parse(Skill::Move& o, const xmlNode* node)
{
}

void Parse(Skill::Melee& o, const xmlNode* node)
{
    for (auto prop = node->properties; prop; prop = prop->next)
    {
        if (xmlTagCompare(prop, "damage"))
        {
            o.damage = Engine::from_strings<unsigned>(xmlText(prop->children), L',');
        }
        else if (xmlTagCompare(prop, "attribute"))
        {
            // TODO
        }
        else
            throw std::runtime_error("Unknown Melee skill property");
    }
}


void Parse(Skill::Affect& o, const xmlNode* node)
{
    for (auto prop = node->properties; prop; prop = prop->next)
    {
        throw std::runtime_error("Unknown Affect skill property");
    }

}


template<class T>
std::shared_ptr<T> MakeType(const xmlNode* node)
{
    auto o = std::make_shared<T>();
    Parse(*o, node);
    return o;
}

void Parse(Skill& o, const xmlNode* node)
{
    for (auto prop = node->properties; prop; prop = prop->next)
    {
        if (xmlTagCompare(prop, "name"))
        {
            o.name = xmlText(prop->children);
            o.LoadIcon(std::wstring(L"Data/") + o.name + std::wstring(L".png"));
        }
        else if (xmlTagCompare(prop, "mp"))
        {
            auto mpStr = xmlText(prop->children);
            o.mp = Engine::from_string<unsigned>(mpStr);
        }
        else if (xmlTagCompare(prop, "range"))
        {
            auto rangeStr= xmlText(prop->children);
            o.range = Engine::from_string<unsigned>(rangeStr);
        }
        else if (xmlTagCompare(prop, "trajectory"))
        {
            o.trajectory = Engine::from_strings<Trajectory>(xmlText(prop->children), L'|',
            {
                { L"straight", Trajectory::Straight },
                { L"reverse", Trajectory::Reverse },
                { L"forehand", Trajectory::Forehand },
                { L"backhand", Trajectory::Backhand },
                { L"up",Trajectory::Up },
                { L"down", Trajectory::Down },
            });
        }
        else if (xmlTagCompare(prop, "target"))
        {
            o.target = Engine::from_strings<Targeting>(xmlText(prop->children), L'|',
            {
                { L"self", Targeting::Self },
                { L"swing", Targeting::Swing },
                { L"center", Targeting::Center },
                { L"intercept", Targeting::Intercept },
                { L"aim", Targeting::Intercept },
                { L"seek", Targeting::Seek},
                { L"parabola", Targeting::Intercept },
            });
        }
        else if (xmlTagCompare(prop, "trigger"))
        {
            auto triggerStr = xmlText(prop->children);
            o.trigger = Engine::from_string<Skill::Trigger>(triggerStr, {
                { L"", Skill::Trigger::None },
                { L"Act", Skill::Trigger::Act },
                { L"Combo", Skill::Trigger::Combo },
                { L"Prepare", Skill::Trigger::Prepare },
                { L"React", Skill::Trigger::React },
                { L"Defend", Skill::Trigger::Defend }
            });
        }
        else if (xmlTagCompare(prop, "effect"))
        {
            o.effects = Engine::from_strings<Skill::Effect>(xmlText(prop->children), L'|', {
                { L"", Skill::Effect::None },
                { L"miss", Skill::Effect::Miss },
                { L"interrupt", Skill::Effect::Interrupt },
                { L"deflect", Skill::Effect::Deflect },
                { L"reflect", Skill::Effect::Reflect },
                { L"engage", Skill::Effect::Engage },
                { L"disarm", Skill::Effect::Disarm },
                { L"steal", Skill::Effect::Steal },
                { L"drop", Skill::Effect::Drop },
                { L"control", Skill::Effect::Control }
            });
        }
        else if (xmlTagCompare(prop, "category"))
        {
            auto str= xmlWStr(prop->children->content);
            o.categories = Engine::SplitSet(str, L'|');
        }
        else if (xmlTagCompare(prop, "follows"))
        {
            auto str = xmlWStr(prop->children->content);
            o.follows = Engine::SplitSet(str, L'|');
        }
        else if (xmlTagCompare(prop, "attribute"))
        {
            auto str = xmlWStr(prop->children->content);
            o.attribute = Engine::from_string<Attribute>(str, Attributes::map);
        }
        else if (xmlTagCompare(prop, "resist"))
        {
            auto str = xmlWStr(prop->children->content);
            o.resist = Engine::from_string<Attribute>(str, Attributes::map);
        }
        else if (xmlTagCompare(prop, "offset"))
        {
            auto str = xmlWStr(prop->children->content);
            o.offset = Engine::from_string<int>(str);
        }
        else if (xmlTagCompare(prop, "weapon"))
        {
            auto str = xmlWStr(prop->children->content);
            o.weapon = Engine::from_string<Type::Weapon::Style>(str, Type::Weapon::styleMap);
        }
        else if (xmlTagCompare(prop, "chance"))
        {
            o.chance = Engine::from_strings<int>(xmlText(prop->children), L',');
        }
        else
        {
            throw std::runtime_error("Unknown skill property");
        }
    }
    for (xmlNode *curNode = node->children; curNode; curNode = curNode->next)
    {
        if (curNode->type == XML_COMMENT_NODE)
        {
            continue;
        }
        else if (curNode->type == XML_TEXT_NODE)
        {
            auto str = Engine::Strip(xmlText(curNode),L"\r\n\t ");
            if (!str.empty())
                o.description = str;
        }
        else if (curNode->type == XML_ELEMENT_NODE)
        {
            if (xmlTagCompare(curNode, "move"))
            {
                o.type = MakeType<Skill::Move>(curNode);
            }
            else if (xmlTagCompare(curNode, "melee"))
            {
                o.type = MakeType<Skill::Melee>(curNode);
            }
            else if (xmlTagCompare(curNode, "affect"))
            {
                o.type = MakeType<Skill::Affect>(curNode);
            }
            else if (xmlTagCompare(curNode, "description"))
            {
            }
            else
            {
                throw std::runtime_error("Unknown skill action");
            }
        }
        else
        {
            throw std::runtime_error("Unexpected child node of skill");
        }

    }
}

void Parse(Skills& o, xmlNode* node)
{
    for (xmlNode *curNode = node; curNode; curNode = curNode->next)
    {
        if (curNode->type == XML_COMMENT_NODE)
        {
            continue;
        }
        if (curNode->type == XML_ELEMENT_NODE  && xmlTagCompare(curNode, "skill"))
        {
            o.emplace_back(Skill());
            Parse(o.back(), curNode);
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
        if (curNode->type == XML_ELEMENT_NODE  && xmlTagCompare(curNode, "skills"))
        {
            Parse(*this, curNode->children);
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


static void
print_element_names(xmlNode * a_node)
{
    xmlNode *curNode = NULL;

    for (curNode = a_node; curNode; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) {
            std::cout << "node type: Element, name: " << curNode->name << std::endl;
        }

        print_element_names(curNode->children);
    }
}


Skills::Skills(const std::vector<Skill>& skills):
    vector<Skill>(skills)
{
}

}