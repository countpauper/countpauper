#include <GL/gl.h>
#include "Engine/Image.h"
#include "Engine/Color.h"
#include "Engine/Error.h"
#include "Engine/Utils.h"
#include "Engine/Position.h"
#include "Engine/from_string.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

using errno_t = int;

namespace Engine
{

Image::Image(std::string_view fn) :
    Image()
{
    Read(fn);
}


Image::Image(unsigned width, unsigned height, unsigned channels) :
    width(width),
    height(height),
    channels(channels),
    data(static_cast<uint8_t*>(stbi__malloc(width*height*channels)))
{
}

Image::Image(unsigned width, unsigned height, float* floatData) :
    Image(width, height, 1)
{
    for (unsigned i = 0; i < width*height; ++i)
    {
        data[i] = static_cast<unsigned char>(std::min(255.0f, floatData[i] * 255.0f));
    }
}

Image::~Image()
{
    Release();
}


unsigned Image::Width() const
{
    return width;
}

unsigned Image::Height() const
{
    return height;
}

unsigned Image::Channels() const
{
    return channels;
}


unsigned Image::Pixels() const
{
    return width * height;
}

unsigned Image::SizeBytes() const
{
    return Pixels() * channels;
}

void Image::Release()
{
    if (data)
        stbi_image_free(data);
    data = nullptr;
    width = 0;
    height = 0;
    channels = 0;
}

void Image::Read(std::string_view filename)
{
    Release();

    int w, h, c;
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

Image::operator bool() const
{
    return data != nullptr;
}

const uint8_t* Image::operator*() const
{
    return data;
}

uint8_t* Image::operator*()
{
    return data;
}

const uint8_t& Image::operator[](unsigned offset) const
{
    return data[offset];
}

uint8_t& Image::operator[](unsigned offset)
{
    return data[offset];
}

RGBA Image::operator[](const Position& position) const
{
    unsigned offset = (position.x + position.y * width) * channels;
    switch(channels)
    {
    case 0:
        return RGBA::transparent;
    case 1:
        return RGBA(data[offset], data[offset], data[offset]);  // grayscale
    case 2:
        return RGBA(data[offset], data[offset], data[offset], data[offset+1]);  // lumin alpha
    case 3:
        return RGBA(data[offset], data[offset+1], data[offset+2]); // RGB
    case 4:
        return RGBA(data[offset], data[offset+1], data[offset+2], data[offset+3]); // RGBA
    default:
        assert(false); // unimplemented channel to RGBA conversion
        return RGBA::transparent;
    }
}

void Image::Write(std::string_view filename) const
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