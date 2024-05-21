#include "DoubleColumn.h"

bool DoubleColumn::validateValue(const std::string& value) const
{
    if (value[0] == '+' || value[0] == '-' || (value[0] >= '0' && value[0] <= '9'))
    {
        int decimal_point_count = 0; 

        for (size_t i = 1; i < value.length(); i++)
        {
            if (value[i] == '.')
            {
                decimal_point_count++;

                if (decimal_point_count > 1)
                {
                    return false;
                }
            }
            else if (value[i] < '0' || value[i] > '9')
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

double DoubleColumn::parseIntegerPart(const std::string& value, size_t position) const
{
    double result = 0;
    while (position < value.size() && value[position] != '.') 
    {
        result *= 10;
        result += value[position] - '0';
        ++position;
    }
    return result;
}

double DoubleColumn::parseFractionalPart(const std::string& value, size_t position) const
{
    double result = 0.0;
    double multiplier = 0.1;
    while (position < value.size()) 
    {
        result += (value[position] - '0') * multiplier;
        multiplier *= 0.1;
        ++position;
    }
    return result;
}

DoubleColumn::DoubleColumn(const std::string& name, size_t cell_count) : Column(name, cell_count) {}

DoubleColumn* DoubleColumn::clone() const
{
    return new DoubleColumn(*this);
}

double DoubleColumn::parseValue(size_t index) const
{
    const std::string& string_value = (*this)[index];
    if (string_value == "NULL") 
    {
        return 0.0;
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

    double integerPart = parseIntegerPart(string_value, position);

    double fractionalPart = 0.0;
    if (position < string_value.size() && string_value[position] == '.') 
    {
        ++position;
        fractionalPart = parseFractionalPart(string_value, position);
    }

    double result = integerPart + fractionalPart;
    if (is_negative) 
    {
        result = -result;
    }

    return result;
}