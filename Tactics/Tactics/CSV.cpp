#include "stdafx.h"
#include "CSV.h"

namespace Engine
{
    namespace Adapter
    {
        Interface::Interface(unsigned offset) : offset(offset) {}

        String::String(unsigned offset) : Interface(offset) {}
        Integer::Integer(unsigned offset) : Interface(offset) {}

        Enumeration::Enumeration(unsigned offset, const std::map<std::wstring, int>& values) : Interface(offset) {}
        Optional::Optional(unsigned offset, Interface& type) : Interface(offset) {}
    }
}