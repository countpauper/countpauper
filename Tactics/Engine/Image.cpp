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
    channels(0),
    id(0)
{
    glGenTextures(1, &id);
}

Image::Image(Image&& other) :
    id(other.id)
{
    other.id = 0;
}
Image::~Image()
{
    if (id==0)
        glDeleteTextures(1, &id);
}

void Image::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}
void Image::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::Load(const std::wstring& fn)
{
    std::string fns(fn.begin(), fn.end());
    unsigned char* data = stbi_load(fns.c_str(), &w, &h, &channels, STBI_default);
    Bind();
    if (channels==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        assert(false && "Unsupported image format");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
    Unbind();
    CheckGLError();
    stbi_image_free(data);
}

}