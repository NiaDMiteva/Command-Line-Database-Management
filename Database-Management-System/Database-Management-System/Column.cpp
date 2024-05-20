#include "Column.h"

Column::Column(const std::string& name, size_t cell_count) : cell_values(cell_count), column_name(name)
{
	for (size_t i = 0; i < cell_count; i++)
	{
		cell_values[i] = "NULL";
	}
}

void Column::appendValue(const std::string& value)
{
	if (validateValue(value))
	{
		cell_values.push_back(value);
	}
	else
	{
		cell_values.push_back("NULL");
	}
}

const std::string& Column::operator[](size_t index) const
{
	if (index < 0 || index > cell_values.size() - 1)
	{
		// Invalid index
	}

	return cell_values[index];
}

void Column::removeValue(size_t index)
{
	cell_values.erase(cell_values.begin() + index);
}

Column* Column::createColumn(const std::string& columnType, const std::string& columnName, size_t cellCount)
{
	if (columnType == "integer")
	{
		// int column
	}
	else if (columnType == "double")
	{
		// double column
	}
	else if (columnType == "string")
	{
		// string column
	}
	else
	{
		return nullptr;
	}
}

void Column::updateValue(size_t index, const std::string& value)
{
	if (index < 0 || index > cell_values.size() - 1)
	{
		// Invalid index!
	}

	if (validateValue(value)) 
	{
		cell_values[index] = value;
	}
	else 
	{
		// Invalid value!
	}
}