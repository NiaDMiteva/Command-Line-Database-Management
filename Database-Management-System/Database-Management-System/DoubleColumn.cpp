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
    while (position < value.length() && value[position] != '.') 
    {
        result = result * 10 + (value[position] - '0');
        ++position;
    }

    if (position < value.length() && value[position] == '.') 
    {
        ++position;
    }

    return result;
}

double DoubleColumn::parseFractionalPart(const std::string& value, size_t position) const
{
    double result = 0;
    double decimalPlace = 10;

    while (position < value.length()) 
    {
        double digit = static_cast<double>(value[position] - '0');
        result += digit / decimalPlace;
        decimalPlace *= 10;
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
    std::string value = (*this)[index];
    if (value == "NULL") 
    {
        return 0;
    }

    double result = 0;
    size_t position = 0;

    bool isNegative = (value[0] == '-');
    if (isNegative || value[0] == '+') 
    {
        position = 1;
    }

    result = parseIntegerPart(value, position);
    result += parseFractionalPart(value, position);

    return isNegative ? -result : result;
}