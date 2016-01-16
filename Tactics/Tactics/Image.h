#include <string>

namespace Engine
{
class Image
{
public:
    Image();
    ~Image();
    void Load(const std::string& fn);
    void Bind();
    void Unbind();
private:
    int w;
    int h;
    int channels;
    unsigned id;
};
}//::Engine