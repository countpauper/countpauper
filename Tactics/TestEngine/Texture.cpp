#include "pch.h"
#include <iostream>
#include <filesystem>
#include "Image.h"
#include "Utils.h"

namespace Engine::Test
{
    TEST(Image, DISABLED_ConvertFolder)
    {
        std::wstring folder = L"..\\Tactics\\Data\\Particles";
        for (const auto & entry : std::filesystem::directory_iterator(folder))
        {
            if (!entry.is_regular_file())
                continue;
            if (UpperCase(entry.path().extension().c_str()) != L".PNG")
                continue;
            Image::Data data(entry.path().c_str());
            Image::Data luminAlpha(data.width, data.height, 2);
            for (unsigned p = 0; p < data.Pixels(); ++p)
            {
                luminAlpha.data[p * 2] = 0xFF;
                auto pixel = data.data[p*data.channels];
                luminAlpha.data[p * 2 + 1] = pixel;
            }
            auto newPath = std::filesystem::path(folder) / L"LuminAlpha" / entry.path().filename();
            luminAlpha.Write(newPath.c_str());
        }
    }
}