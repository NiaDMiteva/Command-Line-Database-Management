#pragma once
#include "Database.h"

class DatabaseService
{
public:
	Database database;
	std::string file;
	bool is_opened;

	DatabaseService();

    void addcolumn(const std::string& table_name, const std::string& column_name, const std::string& column_type);
    void aggregate(const std::string& table_name, size_t search_column, const std::string& search_value, size_t target_column, const std::string& operation);
    void close();
    void count(const std::string& table_name, size_t search_column, const std::string& search_value);
    void deleteRows(const std::string& table_name, size_t search_column, const std::string& search_value);
    void describe(const std::string& table_name);
    void exportTable(const std::string& table_name, const std::string& file_name);
    void help() const noexcept;
    void innerjoin(const std::string& table1, size_t column1, const std::string& table2, size_t column2);
    void insert(const std::vector<std::string>& values_to_insert);
    void open(const std::string& file_name);
    void print(const std::string& name);
    void rename(const std::string& old_name, const std::string& new_name);
    void save();
    void saveas(const std::string& file_name);
    void saveTo(const std::string& file_name);
    void select(size_t column, const std::string& value, const std::string& table);
    void showtables() const;
    void update(const std::string& table_name, size_t search_column_index,const std::string& search_value, size_t target_column_index, const std::string& target_value);

    double sum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const;
    double product(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const;
    double maximum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const;
    double minimum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const;

    void visualizationInfo() const;

public:
    static DatabaseService& getInstance();
    void run();

    DatabaseService(DatabaseService const&) = delete;
    DatabaseService& operator=(DatabaseService const&) = delete;
};