#pragma once
#include "Column.h"

class DoubleColumn : public Column
{
private:
	bool validateValue(const std::string& value) const final;
    double parseIntegerPart(const std::string& value, size_t position) const;
    double parseFractionalPart(const std::string& value, size_t position) const;

public:
    DoubleColumn(const std::string& name = "", size_t cell_count = 0);
    DoubleColumn* clone() const final;
    const std::string getColumnType() const final { return "double"; }
    double parseValue(size_t index) const;
};