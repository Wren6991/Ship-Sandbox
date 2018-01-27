#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Vectors.h"

#include <json/json.h>
#include <string>

struct material
{
    std::string name;
    float strength;
    float mass;
    vec3f colour;
    bool isHull;
    material(Json::Value);
};

#endif // _MATERIAL_H_
