#include "Table.h"
#include <fstream>
#include "IntColumn.h"
#include "DoubleColumn.h"
#include "StringColumn.h"

Table::Table(const std::string& file_name, const std::string& table_name) : 
    table_name(table_name), file_name(file_name), columns_count(0), rows_count(0){}

Table::Table(const Table& other) : table_name(other.table_name), file_name(other.file_name), columns_count(other.columns_count), rows_count(other.rows_count)
{
    for (size_t i = 0; i < columns_count; ++i)
    {
        columns.push_back(other.columns[i]->clone());
    }
}

Table::~Table()
{
    for (size_t i = 0; i < columns_count; ++i)
	{
		delete columns[i];
	}
}

Table& Table::operator=(const Table& other)
{
	Table copy(other);
    swap(copy);
    return *this;
}

void Table::swap(Table& other)
{
	std::swap(columns, other.columns);
	std::swap(file_name, other.file_name);
	std::swap(table_name, other.table_name);
	std::swap(columns_count, other.columns_count);
	std::swap(rows_count, other.rows_count);
}

void Table::addColumn(const std::string& column_name, const std::string& column_type)
{
    columns.push_back(Column::createColumn(column_type, column_name, rows_count));
}

void Table::insertRow(const std::vector<std::string>& row)
{
    for (size_t i = 0; i < getColumnCount(); ++i)
    {
        if (row.size() >= i + 1)
        {
            columns[i]->appendValue(row[i]);
        }
        else
        {
            columns[i]->appendValue("NULL");
        }
    }
    ++rows_count;
}

void Table::rename(const std::string& name) 
{
    table_name = name;
}

void Table::serialize(bool recovery) const 
{
    //TODO: Implement this method
}

void Table::deserialize(bool recovery) 
{
    // TODO: Implement this method
}

const std::string& Table::getFileName() const 
{
    return file_name; 
}

const std::string& Table::getTableName() const 
{
    return table_name;
}

size_t Table::getColumnCount() const 
{
    return columns_count;
}

size_t Table::getRowCount() const 
{
    return rows_count;
}

Column* Table::operator[](size_t index) 
{
    if (index < 0 || index > columns.size() - 1)
    {
        // invalid index
    }
    return columns[index];
}

const Column* Table::operator[](size_t index) const 
{
    if (index < 0 || index > columns.size() - 1)
	{
		// invalid index
	}
	return columns[index];
}

void Table::changeFileName(const std::string& new_file_name) 
{
    file_name = new_file_name;
}

void Table::remove(size_t row_index) 
{
    size_t column_count = getColumnCount();
    for (size_t i = 0; i < column_count; ++i)
	{
		columns[i]->removeValue(row_index);
	}
    --rows_count;
}

Table innerJoin(const Table& first, size_t first_column, const Table& second, size_t second_column)
{
    //TODO : Implement this method
}