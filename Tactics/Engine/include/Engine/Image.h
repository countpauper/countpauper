#pragma once

#include <string>
#include <memory>
namespace Engine
{

struct RGBA;

class Image
{
public:
    Image();
    explicit Image(const std::string_view fn);
    void Load(const std::string_view fn);
private:
    class Texture
    {
    public:
        Texture();
        ~Texture();
        void Bind() const;
        void Unbind() const;
    private:
        unsigned id;
    };
    std::shared_ptr<Texture> texture;
public:
    class Bind
    {
    public:
        Bind(const Image& image);
        Bind(const Bind& other) = delete;
        Bind& operator=(const Bind& other) = delete;
        Bind(Bind&& other);
        ~Bind();
    private:
        Bind(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> texture;
    };

    struct Data
    {
        Data();
        explicit Data(const std::string_view fileName);
        Data(unsigned width, unsigned height, unsigned channels = 1);
        Data(unsigned width, unsigned height, float* floaData);
        ~Data();

        void Write(std::string_view filename) const;
        void Read(std::string_view filename);
        void Release();

        unsigned size() const;
        unsigned Pixels() const;

        unsigned width;
        unsigned height;
        unsigned channels;
        uint8_t* data;
    };
};
}//::Engine