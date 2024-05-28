#include "StringUtils.h"

size_t StringUtils::parseToInt(const std::string& string)
{
	size_t result = 0;

	for (size_t i = 0; i < string.size(); ++i)
	{
		if (string[i] < '0' || string[i] > '9')
		{
			// String contains characters that are not digits
		}
		else
		{
			result = result * 10 + (string[i] - '0');
		}
	}

	return result;
}

std::string StringUtils::removeLeadingSpaces(const std::string& string)
{
	size_t start = string.find_first_not_of(' ');
	return (start == std::string::npos) ? "" : string.substr(start);
}

std::string StringUtils::removeTrailingSpaces(const std::string& string)
{
	size_t end = string.find_last_not_of(' ');
	return (end == std::string::npos) ? "" : string.substr(0, end + 1);
}

void StringUtils::toLowerCase(std::string& string)
{
	for (size_t i = 0; i < string.size(); ++i)
	{
		if (string[i] >= 'A' && string[i] <= 'Z')
		{
			string[i] = string[i] - 'A' + 'a';
		}
	}
}
