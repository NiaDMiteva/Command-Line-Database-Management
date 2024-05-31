#pragma once
#include "Column.h"

class DoubleColumn : public Column
{
private:
	bool validateValue(const std::string& value) const final;

public:
    DoubleColumn(const std::string& name = "", size_t cell_count = 0);
    DoubleColumn* clone() const final;
    const std::string getColumnType() const final 
    {
        return "double"; 
    }
    double parseValue(size_t index) const;
};