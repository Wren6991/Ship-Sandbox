/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <memory>

struct ImageData
{
public:

    int const Width;
    int const Height;
    std::unique_ptr<unsigned char const[]> Data;

    ImageData(
        int width,
        int height,
        std::unique_ptr<unsigned char const[]> data)
        : Width(width)
        , Height(height)
        , Data(std::move(data))
    {
    }
};