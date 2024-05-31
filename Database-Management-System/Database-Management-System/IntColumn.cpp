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

int IntColumn::parseValue(size_t index) const
{
    if ((*this)[index] == "NULL")
	{
		return 0;
	}
	int result = 0;
	if ((*this)[index][0] == '-' || (*this)[index][0] == '+')
	{
		unsigned i = 1;
		while ((*this)[index][i])
		{
			result *= 10;
			result += (*this)[index][i++] - '0';
		}
		if ((*this)[index][0] == '-')
		{
			result = -result;
		}
	}
	else
	{
		unsigned i = 0;
		while ((*this)[index][i])
		{
			result *= 10;
			result += (*this)[index][i++] - '0';
		}
	}
	return result;
}
