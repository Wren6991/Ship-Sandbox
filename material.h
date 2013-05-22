#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <json/json.h>
#include <string>
#include "vec.h"


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
