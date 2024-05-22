#include "StringColumn.h"

bool StringColumn::validateValue(const std::string& value) const
{
    if (value.length() < 2) 
    {
        return false;
    }

    if (value.front() != '"' || value.back() != '"') 
    {
        return false;
    }

    for (size_t i = 1; i < value.length() - 1; ++i) 
    {
        if (value[i] == '"' && value[i - 1] != '\\') 
        {
            return false;
        }

        if (value[i] == '\\') 
        {
            if (i + 1 >= value.length() - 1 || (value[i + 1] != '\\' && value[i + 1] != '"')) 
            {
                return false;
            }
            ++i;
        }
    }

    return true;
}

StringColumn::StringColumn(const std::string& name, size_t cell_count) : Column(name, cell_count){}

StringColumn* StringColumn::clone() const
{
    return new StringColumn(*this);
}
