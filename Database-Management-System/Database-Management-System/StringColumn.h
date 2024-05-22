#pragma once
#include "Column.h"

class StringColumn : public Column
{
private:
	bool validateValue(const std::string& value) const final;

public:
	StringColumn(const std::string& name = "", size_t cell_count = 0);
	StringColumn* clone() const final;
	const std::string getColumnType() const final { return "string"; }
};