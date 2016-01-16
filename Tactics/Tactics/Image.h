#include <string>
#include <png.h>

class Image
{
public:
    void Load(const std::string& fn);
private:
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;
};