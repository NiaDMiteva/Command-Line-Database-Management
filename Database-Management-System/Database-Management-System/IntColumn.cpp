#include "IntColumn.h"

bool IntColumn::validateValue(const std::string& value) const
{
    if (value[0] == '+' || value[0] == '-' || (value[0] >= '0' && value[0] <= '9'))
    {
        for (size_t i = 1; i < value.length(); i++)
        {
            if (value[i] < '0' || value[i] > '9')
            {
                return false;
            }
        }
    }
    return true;
}

IntColumn::IntColumn(const std::string& name, size_t cell_count) : Column(name, cell_count){}

IntColumn* IntColumn::clone() const
{
    return new IntColumn(*this);
}

int IntColumn::parseIntegerPart(const std::string& value, size_t position) const {
    int result = 0;
    for (size_t i = position; i < value.length(); ++i) {
        if (value[i] < '0' || value[i] > '9') 
        {
            throw std::runtime_error("Invalid integer value!");
        }
        result *= 10;
        result += value[i] - '0';
    }
    return result;
}
int IntColumn::parseValue(size_t index) const
{
    const std::string& string_value = (*this)[index];
    if (string_value == "NULL") 
    {
        return 0;
    }

    size_t position = 0;
    bool is_negative = false;

    if (string_value[position] == '-') 
    {
        is_negative = true;
        ++position;
    }
    else if (string_value[position] == '+') 
    {
        ++position;
    }

    int result = parseIntegerPart(string_value, position);

    if (is_negative) 
    {
        result = -result;
    }

    return result;
}
