/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Vectors.h"

#include <sstream>

// V     V  EEEEEEE    CCC      222    FFFFFFF
// V     V  E         CC CC    2   22  F
// V     V  E        CC    C        2  F
//  V   V   E        C             2   F
//  V   V   EEEE     C            2    FFFF
//  V   V   E        C           2     F
//   V V    E        CC    C    2      F
//   VVV    E         CC CC    2       F
//    V     EEEEEEE    CCC    2222222  F

std::wstring vec2f::toString()
{
    std::wstringstream ss;
    ss << L"(" << x << L", " << y << L")";
    return ss.str();
}

// V     V  EEEEEEE    CCC      333    FFFFFFF
// V     V  E         CC CC   33   33  F
// V     V  E        CC    C  3     3  F
//  V   V   E        C             33  F
//  V   V   EEEE     C           33    FFFF
//  V   V   E        C             33  F
//   V V    E        CC    C  3     3  F
//   VVV    E         CC CC   33   33  F
//    V     EEEEEEE    CCC      333    F

std::wstring vec3f::toString()
{
    std::wstringstream ss;
    ss << L"(" << x << L", " << y << L", " << z << L")";
    return ss.str();
}

