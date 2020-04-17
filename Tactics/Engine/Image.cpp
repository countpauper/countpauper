#include "stdafx.h"
#include <gl/GL.h>
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Error.h"

namespace Engine
{

Image::Image() :
    w(0),
    h(0),
    channels(0)
{
}

Image::Texture::Texture()
{
    glGenTextures(1, &id);
}

Image::Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Image::Image(const std::wstring& fn) :
    Image()
{
    Load(fn);
}


void Image::Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}
void Image::Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

Image::Bind::Bind(const Image& image) :
    Bind(image.texture)
{
}

Image::Bind::Bind(std::shared_ptr<Texture> texture) :
    texture(texture)
{
    if (texture)
        texture->Bind();
}

Image::Bind::Bind(Bind&& other) :
    texture(other.texture)
{
    other.texture.reset();
}
Image::Bind::~Bind()
{
    if (texture)
        texture->Unbind();
}

void Image::Load(const std::wstring& fn)
{
    texture = std::make_shared<Texture>();

    std::string fns(fn.begin(), fn.end());
    unsigned char* data = stbi_load(fns.c_str(), &w, &h, &channels, STBI_default);
    if (!data)
        throw std::runtime_error("Image file not found");
    Bind bind(*this);
    if (channels==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        throw std::runtime_error("Unsupported image format");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
    CheckGLError();
    stbi_image_free(data);
}

}