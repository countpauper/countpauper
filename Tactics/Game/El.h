#pragma once
namespace Game
{
    constexpr int HorizontalEl = 3; // 1 square is 3x3l
    constexpr int VerticalEl = 1; // 1 voxel is 3x3x1 l
    constexpr double MeterPerEl = 1.0 / 3.0;
    constexpr double HorizontalGrid = HorizontalEl * MeterPerEl;    // in meter per grid
    constexpr double VerticalGrid = VerticalEl * MeterPerEl;        // in meter per grid
    constexpr double LiterPerVoxel = HorizontalGrid * HorizontalGrid * VerticalGrid * 1000.0;
}