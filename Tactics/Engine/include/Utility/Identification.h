#pragma once

#include <map>

namespace Engine
{
    class Identification
    {
    public:
        Identification();
        Identification(const Identification& other);
        Identification(Identification&& other);
        virtual ~Identification();
        unsigned Id() const;
        static Identification* FindId(unsigned id);
    private:
        static unsigned FindFreeId();
        unsigned m_id;
        static std::map<unsigned, Identification*> s_idRegistry;
    };
}
