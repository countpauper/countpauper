#pragma once
#include "stdafx.h"
#include "Element.h"
#include "Knowledge.h"
#include <assert.h>

namespace Angel::Logic
{

Object Element::Copy() const
{
    // Elements can be copied by casting them to themselves with no knowledge;
    Knowledge none;
    return Cast(typeid(*this), none);
}


bool Element::Match(const Expression& other, const Knowledge& knowledge) const
{
    return (*this) == other;    // TODO: might need to Cast or does == do this?
}

}

