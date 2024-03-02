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
    virtual void Render() const = 0;
    virtual Control* Click(Coordinate pos) const = 0;
    bool shown = true;
    bool enabled = true;
};

class Controls :
    public Control,
    public std::vector<Control*>
{
public:
    using std::vector<Control*>::vector;
    void Render() const override;
    Control* Click(Coordinate pos) const override;
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
    explicit Label(std::string_view text="");
    void SetText(std::string_view text);
    // TODO set font, color, use default font
    void Render() const override;
    Control* Click(Coordinate pos) const override { return nullptr; }
private:
    std::string text;
};

}
