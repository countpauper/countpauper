#include "stdafx.h"
#include "Identification.h"

namespace Engine
{

std::map<unsigned, Identification*> Identification::s_idRegistry;

Identification::Identification() :
    m_id(FindFreeId())
{
    s_idRegistry[m_id] = this;
}

Identification::~Identification()
{
    s_idRegistry.erase(m_id);
}

unsigned Identification::FindFreeId()
{
    unsigned last = 0;
    for (const auto& it : s_idRegistry)
    {
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
    return s_idRegistry.at(id);
}

};