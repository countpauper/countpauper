#include <GL/gl.h>
#include <stdexcept>
#include "Rendering/Texture.h"
#include "Utility/Error.h"

namespace Engine
{

Texture::Texture()
{
    glGenTextures(1, &id);
}

Texture::Texture(const Image& image)
    : Texture()
{
    Binding bind(*this);

    if (image.Channels()==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, *image);
    else if (image.Channels() == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.Width(), image.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, *image);
    else if (image.Channels() == 2)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8_ALPHA8, image.Width(), image.Height(), 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, *image);
    else if (image.Channels() == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, image.Width(), image.Height(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, *image);
    else
        throw std::runtime_error("Unsupported image format");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
    CheckGLError();
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Texture::Binding Texture::Bind() const
{
    return Binding(*this);
}

Texture::Binding::Binding(const Texture& texture) :
    id(texture.id)
{
    if (id)
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }
}

Texture::Binding::Binding(Binding&& other) :
    id(0)
{
    std::swap(id, other.id);
}

Texture::Binding& Texture::Binding::operator=(Binding&& other)
{
    Unbind();
    std::swap(id, other.id);
    return *this;
}

Texture::Binding::~Binding()
{
    Unbind();
}

void Texture::Binding::Unbind()
{
    if (id)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        id = 0;
    }
}

}
