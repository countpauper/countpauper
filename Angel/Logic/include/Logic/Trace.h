#pragma once 
namespace Angel::Logic
{
class Trace 
{
public:
    Trace() = default;
    Trace(Trace& parent, const class Predicate& predicate);
    void SetVerbosity(unsigned level);
private:
    unsigned Depth() const;
    Trace* parent = nullptr;
    unsigned verbosity = 0;

};

}