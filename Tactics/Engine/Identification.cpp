#include "stdafx.h"
#include "Identification.h"

namespace Engine
{

std::map<unsigned, Identification*> Identification::s_idRegistry;

Identification::Identification() :
    m_id(FindFreeId())
{
    if (m_id)
        s_idRegistry[m_id] = this;
}

Identification::Identification(const Identification&) :
    Identification()
{
}

Identification::Identification(Identification&& other) :
    m_id(other.m_id)
{
    other.m_id = 0;
    s_idRegistry[m_id] = this;
}

Identification::~Identification()
{
    s_idRegistry[m_id] = nullptr;
}

unsigned Identification::FindFreeId()
{
    unsigned last = 1;
    for (const auto& it : s_idRegistry)
    {
        if (it.first != last)
            break;
        if (it.first == std::numeric_limits<unsigned>::max())
            throw std::range_error("Identity crisis");
        last = it.first + 1;
    }
    return last;
}

unsigned Identification::Id() const
{
    return m_id;
}


Identification* Identification::FindId(unsigned id)
{
    auto it = s_idRegistry.find(id);
    if (it == s_idRegistry.end())
        return nullptr;
    else
        return it->second;
}

};