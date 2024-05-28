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
    std::ofstream os;
    if (recovery)
    {
        os = std::ofstream
        {
            "recovery-" + file_name,
            std::ios::out 
        };
    }
    else
    {
        os = std::ofstream
        {
            file_name,
            std::ios::out 
        };
    }

    os << table_name << '\n'
        << columns_count << '\n'
        << rows_count << '\n';
    for (size_t i = 0; i < columns_count; i++)
    {
        columns[i]->serialize(os);
    }
    os.close();
}

void Table::deserialize(bool recovery) 
{
    std::ifstream is;
    if (recovery)
    {
        is = std::ifstream
        {
            "recovery-" + file_name,
            std::ios::in 
        };
    }
    else
    {
        is = std::ifstream
        {
            file_name,
            std::ios::in 
        };
    }

    std::string name;
    std::getline(is, name);
    table_name = name;
    size_t _columns_count = 0;
    size_t _rows_count = 0;
    is >> _columns_count >> _rows_count;
    columns_count = _columns_count;
    rows_count = _rows_count;
    is.ignore();
    for (size_t i = 0; i < _columns_count; i++)
    {
        std::string type;
        std::getline(is, type);
        columns.push_back(Column::createColumn(type));
        columns[i]->deserialize(is);
    }
    if (name == "") 
    {
        rename("empty");
    }
    is.close();
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
        throw std::runtime_error("Invalid index!");
    }
    return columns[index];
}

const Column* Table::operator[](size_t index) const 
{
    if (index < 0 || index > columns.size() - 1)
	{
        throw std::runtime_error("Invalid index!");
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
    Table result(first.getTableName() + '-' + second.getTableName() + ".txt", first.getTableName() + '-' + second.getTableName());
    for (size_t i = 0; i < first.getColumnCount(); i++)
    {
        if (i == first_column)
        {
            result.addColumn(first.getTableName() + '-' + first.columns[i]->getColumnName(), first.columns[i]->getColumnType());
        }
        else
        {
            result.addColumn(first.columns[i]->getColumnName(), first.columns[i]->getColumnType());
        }
    }
    for (size_t i = 0; i < second.getColumnCount(); i++)
    {
        if (i == second_column)
        {
            result.addColumn(second.getTableName() + '-' + second.columns[i]->getColumnName(), second.columns[i]->getColumnType());
        }
        else
        {
            result.addColumn(second.columns[i]->getColumnName(), second.columns[i]->getColumnType());
        }
    }

    for (size_t i = 0; i < std::min(first.getRowCount(), second.getRowCount()); i++)
    {
        for (size_t j = 0; j < std::max(first.getRowCount(), second.getRowCount()); j++)
        {
            if ((*first.columns[first_column])[i] == (*second.columns[second_column])[j])
            {
                std::vector<std::string> v;
                for (size_t k = 0; k < first.getColumnCount(); k++)
                {
                    v.push_back((*first.columns[k])[i]);
                }
                for (size_t k = 0; k < second.getColumnCount(); k++)
                {
                    v.push_back((*second.columns[k])[j]);
                }

                result.insertRow(v);
            }
        }
    }

    return result;
}