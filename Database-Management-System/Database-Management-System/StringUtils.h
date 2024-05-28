#pragma once
#include <string>

class StringUtils
{
private:
    StringUtils() = default;

public:
    static size_t parseToInt(const std::string& string);
    static std::string removeLeadingSpaces(const std::string& string);
    static std::string removeTrailingSpaces(const std::string& string);
    static void toLowerCase(std::string& string);
};