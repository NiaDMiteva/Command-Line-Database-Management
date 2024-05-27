#include "DatabaseService.h"
#include <fstream>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cstdio>
#include "IntColumn.h"
#include "DoubleColumn.h"

DatabaseService::DatabaseService() : is_opened(false) {}

void DatabaseService::addcolumn(const std::string& table_name, const std::string& column_name, const std::string& column_type)
{
}

void DatabaseService::aggregate(const std::string& table_name, size_t search_column, const std::string& search_value, size_t target_column, const std::string& operation)
{
}

void DatabaseService::close()
{
}

void DatabaseService::count(const std::string& table_name, size_t search_column, const std::string& search_value)
{
}

void DatabaseService::deleteRows(const std::string& table_name, size_t search_column, const std::string& search_value)
{
}

void DatabaseService::describe(const std::string& name)
{
}

void DatabaseService::exportTable(const std::string& name, const std::string& file_name)
{
}

void DatabaseService::help() const noexcept 
{
    std::cout << "Hello! Do you need help?\n";
    std::cout << "Here is a list of commands you can use:\n\n";

    std::cout << std::setw(30) << std::left << "open <file>"
        << "Open the database file named <file>.\n";

    std::cout << std::setw(30) << "close"
        << "Close the currently open database file.\n";

    std::cout << std::setw(30) << "save"
        << "Save the currently open database file.\n";

    std::cout << std::setw(30) << "saveas <file>"
        << "Save the current database as <file>.\n";

    std::cout << std::setw(30) << "help"
        << "Show this help information.\n";

    std::cout << std::setw(30) << "exit"
        << "Exit the program.\n";

    std::cout << std::setw(30) << "import <file name>"
        << "Import a table from <file name> into the database.\n";

    std::cout << std::setw(30) << "showtables"
        << "Display a list of all table names in the database.\n";

    std::cout << std::setw(30) << "describe <name>"
        << "Show the column types of the table <name>.\n";

    std::cout << std::setw(30) << "print <name>"
        << "Print all rows of the table <name>.\n";

    std::cout << std::setw(30) << "export <name> <file name>"
        << "Export the table <name> to <file name>.\n";

    std::cout << std::setw(30) << "select <column-n> <value> <table name>"
        << "Print rows from <table name> where column <column-n> has value <value>.\n";

    std::cout << std::setw(30) << "addcolumn <table name> <column name> <column type>"
        << "Add a column named <column name> of type <column type> to <table name>.\n";

    std::cout << std::setw(30) << "update <table name> <search column n> <search value> <target column n> <target value>"
        << "Update rows in <table name> where <search column n> is <search value>, setting <target column n> to <target value>.\n";

    std::cout << std::setw(30) << "delete <table name> <search column n> <search value>"
        << "Delete rows from <table name> where <search column n> is <search value>.\n";

    std::cout << std::setw(30) << "insert <table name> <column1> ... <column n>"
        << "Insert a new row into <table name> with values for <column1> ... <column n>.\n";

    std::cout << std::setw(30) << "innerjoin <table 1> <column n1> <table 2> <column n2>"
        << "Perform an inner join between <table 1> and <table 2> on columns <column n1> and <column n2>.\n";

    std::cout << std::setw(30) << "rename <old name> <new name>"
        << "Rename table <old name> to <new name>.\n";

    std::cout << std::setw(30) << "count <table name> <search column n> <search value>"
        << "Count rows in <table name> where <search column n> is <search value>.\n";

    std::cout << std::setw(30) << "aggregate <table name> <search column n> <search value> <target column n> <operation>"
        << "Perform <operation> on <target column n> values where <search column n> is <search value> in <table name>.\n";

    std::cout << "\nEnjoy managing your database!\n";
}


void DatabaseService::innerjoin(const std::string& table1, size_t column1, const std::string& table2, size_t column2)
{
}

void DatabaseService::insert(const std::vector<std::string>& values_to_insert)
{
}

void DatabaseService::open(const std::string& file_name)
{
}

void DatabaseService::print(const std::string& name)
{
}

void DatabaseService::rename(const std::string& old_name, const std::string& new_name)
{
}

void DatabaseService::save()
{
}

void DatabaseService::saveas(const std::string& file_name)
{
}

void DatabaseService::saveTo(const std::string& file_name)
{
}

void DatabaseService::select(size_t column, const std::string& value, const std::string& table)
{
}

void DatabaseService::showtables() const
{
}

void DatabaseService::update(const std::string& table, size_t search_column, const std::string& search_value, size_t target_column, const std::string& target_value)
{
}

double DatabaseService::sum(const Table& table, const std::string& value, size_t searchColumn, size_t targetColumn) const
{
	return 0.0;
}

double DatabaseService::product(const Table& table, const std::string& value, size_t searchColumn, size_t targetColumn) const
{
	return 0.0;
}

double DatabaseService::maximum(const Table& table, const std::string& value, size_t searchColumn, size_t targetColumn) const
{
	return 0.0;
}

double DatabaseService::minimum(const Table& table, const std::string& value, size_t searchColumn, size_t targetColumn) const
{
	return 0.0;
}

void DatabaseService::visualizationInfo() const
{
}

DatabaseService& DatabaseService::getInstance()
{
	static DatabaseService instance;
	return instance;
}

void DatabaseService::run()
{
}