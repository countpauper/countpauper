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
    explicit Image(const std::wstring& fn);
    void Load(const std::wstring& fn);

private:
    int w;
    int h;
    int channels;
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

    static void Write(const std::wstring& filename, unsigned width, unsigned height, RGBA data[]);
    static void Write(const std::wstring& filename, unsigned width, unsigned height, float data[]);
};
}//::Engine