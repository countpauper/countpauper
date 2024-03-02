#pragma once
#include <vector>
#include "Geometry/Coordinate.h"

namespace Engine
{

class Control
{
public:
    explicit Control(std::string_view name);
    virtual void Render() const = 0;
    virtual Control* Click(Coordinate pos) const = 0;
    template<class Type>
    Type* Find(std::string_view path)
    {
        return dynamic_cast<Type*>(FindControl(path));
    }
    virtual Control* FindControl(std::string_view path);

    bool shown = true;
    bool enabled = true;
protected:
    std::string name;
};

class Controls :
    public Control,
    public std::vector<Control*>
{
public:
    using Control::Control;
    using std::vector<Control*>::vector;
    void Render() const;
    Control* Click(Coordinate pos) const;
    virtual Control* FindControl(std::string_view path);
private:
    static constexpr char pathSeparator = '.';
};

}
