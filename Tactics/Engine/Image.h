#pragma once

#include <string>

namespace Engine
{
class Image
{
public:
    Image();
    Image(const Image&) = delete;
    Image(Image&& other);
    ~Image();
    void Load(const std::wstring& fn);
    void Bind() const;
    void Unbind() const;
private:
    int w;
    int h;
    int channels;
    unsigned id;
};
}//::Engine