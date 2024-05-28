#pragma once
#include <vector>
#include "Column.h"

class Table
{
private:
	std::vector<Column*> columns;
	std::string file_name;
	std::string table_name;
	size_t columns_count;
	size_t rows_count;
public:
	Table(const std::string& file_name = "", const std::string& table_name = "");
	Table(const Table& other);
	~Table();
	Table& operator=(const Table& other);
	void swap(Table& other);
	void addColumn(const std::string& column_name, const std::string& column_type);
	void insertRow(const std::vector<std::string>& row);
	void rename(const std::string& name);
	void serialize(bool recovery = false) const;
	void deserialize(bool recovery = false);
	const std::string& getFileName() const;
	const std::string& getTableName() const;
	size_t getColumnCount() const;
	size_t getRowCount() const;
	Column* operator[](size_t index);
	const Column* operator[](size_t index) const;
	void changeFileName(const std::string& new_file_name);
	void remove(size_t row_index);
	friend Table innerJoin(const Table& first, size_t first_column, const Table& second, size_t second_column);
};