#pragma once
#include "Column.h"

class IntColumn : public Column
{ 
private:
	bool validateValue(const std::string& value) const final;
	int parseIntegerPart(const std::string& value, size_t position) const;

public:
	IntColumn(const std::string& name = "", size_t cell_count = 0);
	IntColumn* clone() const final;
	const std::string getColumnType() const final { return "int"; }
	int parseValue(size_t index) const;
};