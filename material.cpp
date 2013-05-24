#include "material.h"

#include <sstream>


int hex2int(std::string str)
{
    std::stringstream ss;
    ss << std::hex << str;
    int x;
    ss >> x;
    return x;
}

vec3f hex2Colour(std::string str)    //  e.g. "#00FF00";
{
    if (str[0] == '#')
        str = str.substr(1);
    if (str.length() != 6)
        std::cout << "Error: badly formed hex colour value!\n";
    return vec3f(hex2int(str.substr(0, 2)) / 255.f,
                 hex2int(str.substr(2, 2)) / 255.f,
                 hex2int(str.substr(4, 2)) / 255.f);
}

material::material(Json::Value root)
{
    name = root.get("name", "Unspecified").asString();
    mass = root.get("mass", 1).asDouble();
    strength = root.get("strength", 1).asDouble() / mass * 1000;
    colour = hex2Colour((root.isMember("colour") ? root["colour"] : root["color"]).asString());  // may as well account for American spelling...
    isHull = root["isHull"].asBool();
    std::cout << "Adding new material: \"" << name << "\" " << colour.toString() << "\n";
}
