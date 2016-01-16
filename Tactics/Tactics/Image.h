#include <string>

class Image
{
public:
    Image();
    ~Image();
    void Load(const std::string& fn);
private:
    int w;
    int h;
    int channels;
    unsigned char* data;
};