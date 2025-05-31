#pragma once 
namespace Angel::Logic
{
class Trace 
{
public:
    Trace() = default;
    Trace(const Trace& parent, const class Predicate& predicate);
    void SetVerbosity(unsigned level);
private:
    const Trace* parent = nullptr;
    unsigned verbosity = 0;

};

}