#pragma once

#include <string>

namespace Engine
{
class Font
{
public:
    Font(std::string_view name, unsigned height);
    void Render(int character) const;
    unsigned Width(std::string_view text) const;
    unsigned Height(std::string_view text="") const;
private:
    struct FontDescriptor* font;    // platform/implementation dependent font descriptor
    unsigned height;
};

}
