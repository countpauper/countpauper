#include <GL/gl.h>
#include "Engine/Image.h"
#include "Engine/Color.h"
#include "Engine/Error.h"
#include "Engine/Utils.h"
#include "Engine/from_string.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

using errno_t = int;

namespace Engine
{

Image::Image() = default;

Image::Texture::Texture()
{
    glGenTextures(1, &id);
}

Image::Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Image::Image(const std::string_view fn) :
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

void Image::Load(const std::string_view fn)
{
    texture = std::make_shared<Texture>();

    Data data(fn);
    std::string fns = from_string<std::string>(fn);
    Bind bind(*this);
    if (data.channels==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);
    else if (data.channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, data.width, data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data);
    else if (data.channels == 2)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8_ALPHA8, data.width, data.height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data.data);
    else if (data.channels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, data.width, data.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data.data);
    else
        throw std::runtime_error("Unsupported image format");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
    CheckGLError();
}

Image::Data::Data() :
    data(nullptr),
    width(0),
    height(0),
    channels(0)
{
}

Image::Data::Data(const std::string_view fileName) :
    Data()
{
    Read(fileName);
}

Image::Data::Data(unsigned width, unsigned height, unsigned channels) :
    width(width),
    height(height),
    channels(channels),
    data(static_cast<uint8_t*>(stbi__malloc(width*height*channels)))
{
}

Image::Data::Data(unsigned width, unsigned height, float* floatData) :
    Data(width, height, 1)
{
    for (unsigned i = 0; i < width*height; ++i)
    {
        data[i] = static_cast<unsigned char>(std::min(255.0f, floatData[i] * 255.0f));
    }
}

Image::Data::~Data()
{
    Release();
}

void Image::Data::Release()
{
    if (data)
        stbi_image_free(data);
    data = nullptr;
    width = 0;
    height = 0;
    channels = 0;
}

unsigned Image::Data::size() const
{
    return width * height*channels;
}

unsigned Image::Data::Pixels() const
{
    return width * height;
}

void Image::Data::Read(std::string_view filename)
{
    Release();

    int w, h, c;
    Data result;
    std::string fns = from_string<std::string>(filename);
    data = stbi_load(fns.c_str(), &w, &h, &c, STBI_default);
    if (!data)
    {
        throw std::runtime_error("Image file " + fns + " not found.");
    }
    width = w;
    height = h;
    channels = c;
}

void Image::Data::Write(std::string_view filename) const
{
    std::string extension = UpperCase(filename.substr(filename.find_last_of('.'), std::string::npos));
    if (extension == ".PNG")
    {
        std::string fns = from_string<std::string>(filename);

        if (!stbi_write_png(fns.c_str(), width, height, channels, data, width * channels))
        {
            errno_t err = errno;
            throw std::runtime_error("Failed to write image " + fns);
        }
    }
    else
    {
        throw std::runtime_error("Unsupported image format");
    }
}

/*
void Image::Write(const std::string& filename, unsigned width, unsigned height, Engine::RGBA data[])
{
    std::string extension = UpperCase(filename.substr(filename.find_last_of('.'), std::string::npos));
    if (extension == ".PNG")
    {
        stbi_write_png(from_string<std::string>(filename).c_str(), width, height, 4, data, width * sizeof(RGBA));
    }
    else
    {
        throw std::runtime_error("Unsupported image format");
    }
}

void Image::Write(const std::string& filename, unsigned width, unsigned height, float data[])
{
    std::unique_ptr<RGBA[]> recode = std::make_unique<RGBA[]>(width*height);
    for (unsigned i = 0; i < width*height; ++i)
    {
        recode[i].a = 255;
        recode[i].r = unsigned char(std::min(255.0f, data[i] * 255.0f));
        recode[i].g = recode[i].b = recode[i].r;
    }
    Write(filename, width, height, recode.get());
}
*/

}