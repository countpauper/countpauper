#pragma once 
namespace Angel::Logic
{
class Trace 
{
public:
    Trace() = default;
    void SetVerbosity(unsigned level);
    void operator()(const class Predicate& predicate);
private:
    unsigned verbosity = 0;

};

}