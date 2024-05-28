#include "Column.h"
#include "IntColumn.h"
#include "DoubleColumn.h"
#include "StringColumn.h"

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
		throw std::runtime_error("Invalid index!");
	}

	return cell_values[index];
}

void Column::removeValue(size_t index)
{
	cell_values.erase(cell_values.begin() + index);
}

Column* Column::createColumn(const std::string& column_type, const std::string& column_name, size_t cell_count)
{
	if (column_type == "integer")
	{
		return new IntColumn(column_name, cell_count);
	}
	else if (column_type == "double")
	{
		return new DoubleColumn(column_name, cell_count);
	}
	else if (column_type == "string")
	{
		return new StringColumn(column_name, cell_count);
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
		throw std::runtime_error("Invalid index!");
	}

	if (validateValue(value)) 
	{
		cell_values[index] = value;
	}
	else 
	{
		cell_values[index] = "NULL";
	}

}

std::ostream& Column::serialize(std::ostream& os) const
{
	os << getColumnType() << '\n'
		<< column_name << '\n'
		<< getCellCount() << '\n';

	for (size_t i = 0; i < getCellCount(); i++)
	{
		os << (*this)[i] << '\n';
	}

	return os;
}

std::istream& Column::deserialize(std::istream& is)
{
	std::string name;
	std::getline(is, name);
	column_name = name;

	size_t data_size;
	is >> data_size;
	is.ignore();

	for (size_t index = 0; index < data_size; ++index)
	{
		std::string string_to_add;
		std::getline(is, string_to_add);
		appendValue(string_to_add);
	}

	return is;
}