#include "DoubleColumn.h"

bool DoubleColumn::validateValue(const std::string& value) const
{
    if (value[0] == '+' || value[0] == '-' || (value[0] >= '0' && value[0] <= '9'))
    {
        int separator_count = 0;
        for (size_t i = 1; i < value.length(); i++)
        {
            if (value[i] == '.')
            {
                separator_count++;
                if (separator_count > 1)
                {
                    return false;
                }
            }
            else if (value[i] < '0' || value[i] > '9')
            {
                return false;
            }
        }
    }
    return true;
}

DoubleColumn::DoubleColumn(const std::string& name, size_t cell_count) : Column(name, cell_count){}

DoubleColumn* DoubleColumn::clone() const
{
    return new DoubleColumn(*this);
}

double DoubleColumn::parseValue(size_t index) const
{
    if ((*this)[index] == "NULL")
    {
        return 0;
    }
    double result = 0;
    if ((*this)[index][0] == '-' || (*this)[index][0] == '+')
    {
        long multiplier = 10;
        unsigned i = 1;
        while ((*this)[index][i] && (*this)[index][i] != '.')
        {
            result *= 10;
            result += (*this)[index][i++] - '0';
        }
        i++;
        while ((*this)[index][i])
        {
            double digit = static_cast<double>((*this)[index][i++] - '0');
            result += digit / multiplier;
            multiplier *= 10;
        }

        if ((*this)[index][0] == '-')
        {
            result = -result;
        }
    }
    else
    {
        long multiplier = 10;
        unsigned i = 0;
        while ((*this)[index][i] && (*this)[index][i] != '.')
        {
            result *= 10;
            result += (*this)[index][i++] - '0';
        }
        i++;
        while ((*this)[index][i])
        {
            double digit = static_cast<double>((*this)[index][i++] - '0');
            result += digit / multiplier;
            multiplier *= 10;
        }
    }
    return result;
}