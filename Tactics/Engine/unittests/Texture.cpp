#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include "Engine/File/Image.h"
#include "Engine/Utility/Utils.h"

namespace Engine::Test
{
    TEST(Texture, DISABLED_ConvertFolder)
    {
        // Not really a test but a tool to convert a grayscale PNG files to lumin & alpha particle textures
        std::string folder = "..\\Tactics\\Data\\Particles";
        for (const auto & entry : std::filesystem::directory_iterator(folder))
        {
            if (!entry.is_regular_file())
                continue;
            if (UpperCase(entry.path().extension().string()) != ".PNG")
                continue;
            Image data(entry.path().string());
            Image luminAlpha(data.Width(), data.Height(), 2);
            for (unsigned p = 0; p < data.Pixels(); ++p)
            {
                luminAlpha[p * 2] = 0xFF;
                auto pixel = data[p * data.Channels()];
                luminAlpha[p * 2 + 1] = pixel;
            }
            auto newPath = std::filesystem::path(folder) / "LuminAlpha" / entry.path().filename();
            luminAlpha.Write(newPath.string());
        }
    }
}
