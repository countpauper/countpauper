#pragma once

#include <string>
#include <memory>
namespace Engine
{

struct RGBA;

class Image final
{
public:
    Image() = default;
    Image(unsigned width, unsigned height, unsigned channels);
    Image(unsigned width, unsigned height, float* floatData);
    ~Image();
    explicit Image(std::string_view fn);
    unsigned Width() const;
    unsigned Height() const;
    unsigned Channels() const;
    unsigned Pixels() const;
    unsigned SizeBytes() const;
    
    void Write(std::string_view filename) const;
    const uint8_t* operator*() const;
    uint8_t* operator*();
    const uint8_t& operator[](unsigned offset) const;
    uint8_t& operator[](unsigned offset);
    operator bool() const;
private:
    void Read(std::string_view fn);

    void Release();
    unsigned width = 0;
    unsigned height = 0;
    unsigned channels = 0;
    uint8_t* data = nullptr;
};
}//::Engine