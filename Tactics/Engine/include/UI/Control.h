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

    bool IsShown() const;
    bool IsEnabled() const;
    void Show();
    void Hide();
    void Enable();
    void Disable();
protected:
    friend class Controls;
    const Control* parent=nullptr;
    std::string name;
    bool shown = true;
    bool enabled = true;
};

class Controls :
    public Control
{
public:
    using Control::Control;
    void Add(Control& control);
    void Render() const;
    Control* Click(Coordinate pos) const;
    virtual Control* FindControl(std::string_view path);
private:
    static constexpr char pathSeparator = '.';
    std::vector<Control*> children;
};

}
