#include "util.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

// TODO: see if can have jsoncpp in unicode
Json::Value jsonParseFile(std::string const & filename)
{
    std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Error: could not open file " << filename << "\n";
        return Json::Value();
    }

    file.seekg(0, std::ios::end);
    size_t length = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    char *buffer = new char[length + 1];
    file.read(buffer, length);

    Json::CharReaderBuilder readerBuilder;
	std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
    Json::Value root;
	JSONCPP_STRING errors;
    bool parsingSuccessful = reader->parse(buffer, buffer + length, &root, &errors);
    if (parsingSuccessful)
    {
        return root;
    }
    else
    {
		std::cout << "In file " << filename << ": Parsing error(s):\n" << errors;
        return Json::Value();
    }
}

charbuffer getFileContents(std::string const & filename)
{
    std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << filename << " for reading.\n";
    }
    file.seekg(0, std::ios::end);
    size_t length = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    char *buffer = new char[length];
    file.read(buffer, length);
    charbuffer result;
    result.buffer = buffer;
    result.length = length;
    return result;
}

template <typename T> std::string tostring(T x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}
