#include "util.h"

#include <fstream>
#include <iostream>
#include <sstream>

Json::Value jsonParseFile(std::string filename)
{
    std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Error: could not open file " << filename << "\n";
        return Json::Value();
    }

    file.seekg(0, std::ios::end);
    int length = file.tellg();
    file.seekg(0, std::ios::beg);
    char *buffer = new char[length + 1];
    file.read(buffer, length);

    Json::Reader reader;
    Json::Value root;
    bool parsingSuccessful = reader.parse(buffer, root);
    if (parsingSuccessful)
    {
        return root;
    }
    else
    {
        std::cout << "In file " << filename << ": Parsing error(s):\n" << reader.getFormatedErrorMessages();
        return Json::Value();
    }
}

charbuffer getFileContents(std::string filename)
{
    std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << filename << " for reading.\n";
    }
    file.seekg(0, std::ios::end);
    int length = file.tellg();
    file.seekg(0, std::ios::beg);
    char *buffer = new char[length];
    file.read(buffer, length);
    charbuffer result;
    result.buffer = buffer;
    result.length = length;
    return result;
}
