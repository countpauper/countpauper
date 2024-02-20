#pragma once

#include "Engine/File/Image.h"
namespace Engine
{

class Texture final
{
public:
    Texture();
    Texture(const Image& image);
    ~Texture();

    class Binding
    {
    public:
        Binding() = default;
        Binding(const Texture& texture);
        Binding(Binding&& other);
        Binding& operator=(Binding&& other);
        ~Binding();
    private:
        void Unbind();
        unsigned id = 0;
    };
    Binding Bind() const;

private:
    unsigned id = 0;
};

}
