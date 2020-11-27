#pragma once
namespace Game
{

    constexpr int HorizontalEl = 3; // 1 square is 3x3l
    constexpr int VerticalEl = 1; // 1 block is 3x3x1 l
    constexpr double MeterPerEl = 1.0 / 3.0;
    constexpr double LiterPerVoxel = (HorizontalEl * MeterPerEl) * (HorizontalEl * MeterPerEl)* (VerticalEl * MeterPerEl) * 1000.0;

}