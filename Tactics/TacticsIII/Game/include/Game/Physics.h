#pragma once

namespace Game 
{
class Stack;
class Cell;

class Physics 
{   // Place holder base class, later can be used as computation agent in multi threaded setup 

protected:
    float PressureForce(Cell& up, Cell& down, float area, Orientation axis, float dt) const;
private:
    float gravity;
};

class Vertical : public Physics 
{
public:
    explicit Vertical(float gravity=9.80665f);
    void operator()(float dt, Stack& Stack) const;

};

class Viscosity : public Physics 
{
public:
    Viscosity();
    void operator()(float dt, Stack& Stack) const;
};

}