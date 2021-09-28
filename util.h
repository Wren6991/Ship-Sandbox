#ifndef _UTIL_H_
#define _UTIL_H_

#include "ogl.h"

#include <json/json.h>
#include <string>


struct charbuffer
{
    char *buffer;
    int length;
};

Json::Value jsonParseFile(std::string filename);
charbuffer getFileContents(std::string filename);
template <typename T> std::string tostring(T x);

#endif // _UTIL_H_
