#pragma once
#include "Column.h"

class IntColumn : public Column
{ 
private:
	bool validateValue(const std::string& value) const final;

public:
	IntColumn(const std::string& name = "", size_t cell_count = 0);
	IntColumn* clone() const final;
	const std::string getColumnType() const final 
	{
		return "integer"; 
	}
	int parseValue(size_t index) const;
};