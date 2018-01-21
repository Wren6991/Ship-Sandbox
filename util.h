#ifndef _UTIL_H_
#define _UTIL_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <json/json.h>
#include <string>


struct charbuffer
{
    char *buffer;
    int length;
};

Json::Value jsonParseFile(std::wstring const & filename);
charbuffer getFileContents(std::wstring const & filename);
template <typename T> std::string tostring(T x);

#endif // _UTIL_H_
