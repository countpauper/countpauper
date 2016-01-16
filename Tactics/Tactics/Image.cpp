#include "stdafx.h"
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image() :
    w(0),
    h(0),
    channels(0),
    data(nullptr)
{

}

Image::~Image()
{
    free(data);
}

void Image::Load(const std::string& fn)
{
    data = stbi_load(fn.c_str(), &w, &h, &channels, STBI_rgb_alpha);
}

/*
#define PNG_DEBUG 3
#include <png.h>
#include <stdlib.h>
#include <stdio.h>
void Image::Load(const std::string& fn)
{
    png_byte header[8];    // 8 is the maximum size that can be checked

    // open file and test for it being a png 
    FILE *fp;
    errno_t err = fopen_s(&fp, fn.c_str(), "rb");
    if (!fp || err)
        throw std::runtime_error("[read_png_file] File %s could not be opened for reading"+fn);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        throw std::runtime_error("[read_png_file] File %s is not recognized as a PNG file"+fn);


    // initialize stuff 
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        throw std::runtime_error("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw std::runtime_error("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        throw std::runtime_error("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);


    // read file
    if (setjmp(png_jmpbuf(png_ptr)))
        throw std::runtime_error("[read_png_file] Error during read_image");

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y<height; y++)
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

    png_read_image(png_ptr, row_pointers);

    fclose(fp);

}
*/