#pragma once

namespace Game 
{
class Slice;


class Physics 
{   // Place holder base class, later can be used as computation agent in multi threaded setup 
};

class Gravity : public Physics 
{
public:
    Gravity(float gravity);
    void operator()(float dt, Slice& slice) const;
private:
    float gravity;
};

class Viscosity : public Physics 
{
public:
    Viscosity();
    void operator()(float dt, Slice& slice) const;
};

}