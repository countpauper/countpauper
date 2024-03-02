#pragma once
#include <vector>
#include <string>
#include "Geometry/Coordinate.h"
#include "Geometry/Matrix.h"

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

class HUD : public Controls
{
public:
    HUD();
    void Render() const override;
    Control* Click(Coordinate pos) const override;
private:
    Matrix projection;
};

class Label : public Control
{
public:
    Label(std::string_view name, std::string_view text="");
    void SetText(std::string_view text);
    // TODO set font, color, use default font
    void Render() const override;
    Control* Click(Coordinate pos) const override { return nullptr; }
private:
    std::string text;
};

}
