#pragma once

#include <string>

namespace Engine
{
class Font
{
public:
    // NB for stroke fonts, height and width are in model space, for bitmap fonts they are in pixels
    Font(std::string_view name, float height);
    void Render(int character) const;
    float Width(std::string_view text) const;
    float Height(std::string_view text="") const;
    bool IsStroke() const;
private:
    struct FontDescriptor* font;    // platform/implementation dependent font descriptor
    float scale;
};

}
