/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-11
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Physics.h"

#include "Vectors.h"

namespace Physics
{	

class Cloud
{
public:

    float OffsetX;
    float SpeedX1;
    float AmpX;
    float SpeedX2;

    float OffsetY;
    float AmpY;
    float SpeedY;

    float OffsetScale;
    float AmpScale;
    float SpeedScale;

    Cloud()
    {}

    Cloud(
        float offsetX,
        float speedX1,
        float ampX,
        float speedX2,
        float offsetY,
        float ampY,
        float speedY,
        float offsetScale,
        float ampScale,
        float speedScale)
        : OffsetX(offsetX)
        , SpeedX1(speedX1)
        , AmpX(ampX)
        , SpeedX2(speedX2)
        , OffsetY(offsetY)
        , AmpY(ampY)
        , SpeedY(speedY)
        , OffsetScale(offsetScale)
        , AmpScale(ampScale)
        , SpeedScale(speedScale)
    {
    }

    inline vec3f CalculatePosAndScale(float t, float windSpeed) const
    {
        float x = OffsetX + (t * SpeedX1 * windSpeed) + (AmpX  * windSpeed * sinf(SpeedX2 * t));
        float y = OffsetY + (AmpY * sinf(SpeedY * t));
        float scale = OffsetScale + (AmpScale * sinf(SpeedScale * t));

        return vec3f(x, y, scale);
    }

private:
};

}
