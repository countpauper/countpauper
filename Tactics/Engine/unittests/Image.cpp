#include <gtest/gtest.h>
#include "Engine/File/Image.h"
#include <filesystem>

namespace Engine::Test
{

    TEST(Image, Empty)
    {
        Image notAnImage;
        EXPECT_FALSE(notAnImage);
        EXPECT_EQ(notAnImage.Channels(), 0);
        EXPECT_EQ(*notAnImage, nullptr);
    }


    TEST(Image, NewImage)
    {
        Image newImage(15,10,1);
        EXPECT_TRUE(newImage);
        EXPECT_EQ(newImage.Width(), 15);
        EXPECT_EQ(newImage.Channels(), 1);
        memset(*newImage, 0, newImage.SizeBytes());
        EXPECT_EQ(newImage[20], 0);
    }


    TEST(Image, ReadImage)
    {
        Image newImage("../../../data/test.png");
        EXPECT_TRUE(newImage);
        EXPECT_EQ(newImage.Width(), 32);
        EXPECT_EQ(newImage.Channels(), 3);
        EXPECT_EQ(newImage[(newImage.Width()+1)*newImage.Channels()], 23);
    }


}
