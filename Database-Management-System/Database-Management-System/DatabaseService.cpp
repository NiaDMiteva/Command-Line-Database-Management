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

void DatabaseService::addcolumn(const std::string& table_name,
                                const std::string& column_name,
                                const std::string& column_type)
{
    Table* search_table = database.find(table_name);
    if (search_table != nullptr)
    {
        search_table->addColumn(column_name, column_type);
        search_table->serialize(true);
        saveTo("recovery.txt");
        std::cout << "Column " << column_name << " added successfully to table " << table_name << '\n';
    }
    else
    {
        std::cout << "Table with the specified name was not found!\n";
    }
}

void DatabaseService::aggregate(const std::string& table_name,
    size_t search_column, const std::string& search_value,
    size_t target_column, const std::string& operation)
{
    Table* search_table = database.find(table_name);

    if (search_table == nullptr)
    {
        std::cout << "Table with the specified name was not found!\n";
        return;
    }

    if ((*search_table)[target_column - 1]->getColumnType() == "string")
    {
        std::cout << "Target column is not of numeric type!\n";
        return;
    }

    double result = 0.0;

    if (operation == "sum")
    {
        result = sum(*search_table, search_value, search_column, target_column);
        std::cout << "The sum is: " << result << '\n';
    }
    else if (operation == "product")
    {
        result = product(*search_table, search_value, search_column, target_column);
        std::cout << "The product is: " << result << '\n';
    }
    else if (operation == "maximum")
    {
        result = maximum(*search_table, search_value, search_column, target_column);
        std::cout << "The maximum value is: " << result << '\n';
    }
    else if (operation == "minimum")
    {
        result = minimum(*search_table, search_value, search_column, target_column);
        std::cout << "The minimum value is: " << result << '\n';
    }
    else
    {
        std::cout << "Unrecognized operation: " << operation << '\n';
    }
}

void DatabaseService::close()
{
    if (std::remove("recovery.txt") != 0)
    {
        std::cout << "Error removing recovery file: recovery.txt\n";
    }

    for (size_t i = 0; i < database.getTableCount(); ++i)
    {
        std::string recovery_file_name = "recovery-" + database[i].getFileName();

        if (std::remove(recovery_file_name.c_str()) != 0)
        {
            std::cout << "Error removing recovery file: " << recovery_file_name << '\n';
        }
    }

    is_opened = false;
    std::cout << "Successfully closed " << file << '\n';

    file.clear();
    database = Database();
}

void DatabaseService::count(const std::string& table_name, size_t search_column,
                            const std::string& search_value)
{
    Table* search_table = database.find(table_name);

    if (search_table == nullptr)
    {
        std::cout << "Table with the specified name was not found!\n";
        return;
    }

    if (search_column == 0 || search_column > search_table->getColumnCount())
    {
        std::cout << "Invalid column index!\n";
        return;
    }

    Column* column = (*search_table)[search_column - 1];

    size_t counter = 0;
    size_t cell_count = column->getCellCount();
    for (size_t i = 0; i < cell_count; ++i)
    {
        if ((*column)[i] == search_value)
        {
            ++counter;
        }
    }

    std::cout << counter << " rows in column " << column->getColumnName()
        << " contain the value \"" << search_value << "\".\n";
}

void DatabaseService::deleteRows(const std::string& table_name, size_t search_column,
                                const std::string& search_value)
{
    Table* search_table = database.find(table_name);

    if (search_table == nullptr)
    {
        std::cout << "Table with the specified name was not found!\n";
        return;
    }

    if (search_column == 0 || search_column > search_table->getColumnCount())
    {
        std::cout << "Invalid column index!\n";
        return;
    }

    Column* column = (*search_table)[search_column - 1];

    size_t cell_count = column->getCellCount();
    for (size_t i = 0; i < cell_count; )
    {
        if ((*column)[i] == search_value)
        {
            search_table->remove(i);
            cell_count--;
        }
        else
        {
            ++i;
        }
    }

    search_table->serialize(true);
    saveTo("recovery.txt");

    std::cout << "Rows deleted successfully!\n";
}

void DatabaseService::describe(const std::string& table_name)
{
    Table* table = database.find(table_name);

    if (table != nullptr)
    {
        std::cout << "Description of table: " << table_name << ":\n";

        for (size_t i = 0; i < table->getColumnCount(); ++i)
        {
            std::cout << i + 1 << ". " << (*table)[i]->getColumnType() << '\n';
        }
    }
    else
    {
        std::cout << "Table \"" << table_name << "\" not found!\n";
    }
}

void DatabaseService::exportTable(const std::string& table_name, const std::string& file_name)
{
    Table* search_table = database.find(table_name);

    if (search_table != nullptr)
    {
        Table target_table(*search_table);
        target_table.changeFileName(file_name);
        target_table.serialize();

        std::cout << "Table \"" << table_name << "\" exported successfully!\n";
    }
    else
    {
        std::cout << "Table \"" << table_name << "\" not found!\n";
    }
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


void DatabaseService::innerjoin(const std::string& table1, size_t column1,
                                const std::string& table2, size_t column2)
{
    Table* firstTable = database.find(table1);
    Table* secondTable = database.find(table2);

    if (firstTable != nullptr && secondTable != nullptr)
    {
        Table result = innerJoin(*firstTable, column1 - 1, *secondTable, column2 - 1);

        result.serialize();

        database.import(result.getFileName());

        std::string first_name = firstTable->getTableName();
        std::string second_name = secondTable->getTableName();
        std::string joined_name = result.getTableName();

        result.serialize(true);
        saveTo("recovery.txt");

        std::cout << "Successfully joined tables " << first_name << " and " << second_name << " into " << joined_name << "!\n";
    }
    else
    {
        std::cout << "Couldn't find one of the tables!\n";
    }
}

void DatabaseService::insert(const std::vector<std::string>& values_to_insert)
{
    const std::string& table_name = values_to_insert[0];
    Table* table = database.find(table_name);

    if (table != nullptr)
    {
        if (values_to_insert.size() - 1 != table->getColumnCount())
        {
            throw std::runtime_error("Invalid number of parameters!");
        }

        std::vector<std::string> values(values_to_insert.begin() + 1, values_to_insert.end());

        table->insertRow(values);
        table->serialize(true);

        saveTo("recovery.txt");
        std::cout << "Row inserted successfully into table \"" << table_name << "\"!\n";
    }
    else
    {
        std::cout << "Table \"" << table_name << "\" not found!\n";
    }
}

void DatabaseService::open(const std::string& file_name)
{
    std::ifstream recovery("recovery.txt", std::ios::in);

    if (recovery)
    {
        std::cout << "It seems the program didn't close properly last time."
             << "There is a recovery file containing all the changes made before.\n"
             << "Would you like to open the recovery file instead ? (y / n) : ";
        char user_response;
        std::cin >> user_response;
        std::cin.ignore();

        if (user_response == 'y' || user_response == 'Y')
        {
            database.deserialize(recovery);
            recovery.close();

            is_opened = true;
            file = file_name;

            std::cout << "Successfully opened the recovery file. New changes will be saved in: " << file_name << '\n';
            return;
        }
    }

    std::ifstream input_file(file_name, std::ios::in);

    database.deserialize(input_file);
    input_file.close();

    is_opened = true;
    file = file_name;

    std::cout << "Successfully opened: " << file_name << '\n';

    for (size_t i = 0; i < database.getTableCount(); ++i)
    {
        database[i].serialize(true);
    }
}

void DatabaseService::print(const std::string& name)
{
    // TODO: Implement print function
}

void DatabaseService::rename(const std::string& old_name, const std::string& new_name)
{
    Table* table = database.find(old_name);

    if (table != nullptr)
    {
        if (database.find(new_name) != nullptr)
        {
            std::cout << "A table with the new name \"" << new_name << "\" already exists!\n";
        }
        else
        {
            table->rename(new_name);
            table->serialize(true);
            saveTo("recovery.txt");
            std::cout << "Table \"" << old_name << "\" renamed to \"" << new_name << "\"!\n";
        }
    }
    else
    {
        std::cout << "Table \"" << old_name << "\" not found!\n";
    }
}

void DatabaseService::save()
{
    saveTo(file);
    std::remove("recovery.txt");

    for (size_t i = 0; i < database.getTableCount(); ++i)
    {
        std::string recoveryFileName = "recovery-" + database[i].getFileName();
        std::remove(recoveryFileName.c_str());
    }

    is_opened = false;
    file = "";
    database = Database();

    std::cout << "Successfully saved database state.\n";
}


void DatabaseService::saveas(const std::string& file_name)
{
    saveTo(file_name);
    std::remove("recovery.txt");

    for (size_t i = 0; i < database.getTableCount(); ++i)
    {
        std::string recoveryFileName = "recovery-" + database[i].getFileName();
        std::remove(recoveryFileName.c_str());
    }

    is_opened = false;
    file = "";
    database = Database();

    std::cout << "Successfully saved as: " << file_name << '\n';
}


void DatabaseService::saveTo(const std::string& file_name)
{
    std::ofstream os(file_name, std::ios::out);

    if (!os)
    {
        throw std::runtime_error("Couldn't save file: " + file_name);
    }
    if (file_name == "recovery.txt")
    {
        database.serializeWithRecovery(os);
    }
    else
    {
        database.serialize(os);
    }

    os.close();
}


void DatabaseService::select(size_t column, const std::string& value, const std::string& table)
{
    // TODO: Implement select function
}

void DatabaseService::showtables() const
{
    std::cout << "List of tables in the database:\n";
    for (size_t i = 0; i < database.getTableCount(); ++i)
    {
        std::cout << i + 1 << ". " << database[i].getTableName() << '\n';
    }
}

void DatabaseService::update(const std::string& table_name, size_t search_column_index,
                             const std::string& search_value, size_t target_column_index,
                             const std::string& target_value)
{
    Table* target_table = database.find(table_name);

    if (target_table != nullptr)
    {
        Column* search_column = (*target_table)[search_column_index - 1];

        for (size_t i = 0; i < search_column->getCellCount(); ++i)
        {
            if ((*search_column)[i] == search_value)
            {
                (*target_table)[target_column_index - 1]->updateValue(i, target_value);
            }
        }

        target_table->serialize(true);
        saveTo("recovery.txt");
        std::cout << "Table \"" << table_name << "\" successfully updated!\n";
    }
    else
    {
        std::cout << "Table \"" << table_name << "\" not found!\n";
    }
}


double DatabaseService::sum(const Table& table, const std::string& search_value,
                            size_t search_column_index, size_t target_column_index) const
{
    double total_sum = 0;

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); ++i)
    {
        if ((*table[search_column_index - 1])[i] == search_value)
        {
            const Column* target_column = table[target_column_index - 1];
            std::string column_type = target_column->getColumnType();

            if (column_type == "integer")
            {
                const IntColumn* int_col = dynamic_cast<const IntColumn*>(target_column);
                total_sum += int_col->parseValue(i);
            }
            else if (column_type == "double")
            {
                const DoubleColumn* double_col = dynamic_cast<const DoubleColumn*>(target_column);
                total_sum += double_col->parseValue(i);
            }
        }
    }

    return total_sum;
}


double DatabaseService::product(const Table& table, const std::string& search_value,
                                size_t search_column_index, size_t target_column_index) const
{
    double total_product = 1;

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); ++i)
    {
        if ((*table[search_column_index - 1])[i] == search_value && (*table[target_column_index - 1])[i] != "NULL")
        {
            const Column* target_column = table[target_column_index - 1];
            std::string column_type = target_column->getColumnType();

            if (column_type == "integer")
            {
                const IntColumn* int_col = dynamic_cast<const IntColumn*>(target_column);
                total_product *= int_col->parseValue(i);
            }
            else if (column_type == "double")
            {
                const DoubleColumn* double_col = dynamic_cast<const DoubleColumn*>(target_column);
                total_product *= double_col->parseValue(i);
            }
        }
    }

    return total_product;
}


double DatabaseService::maximum(const Table& table, const std::string& search_value,
                                size_t search_column_index, size_t target_column_index) const
{
    double max_value = std::numeric_limits<double>::lowest();

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); ++i)
    {
        if ((*table[search_column_index - 1])[i] == search_value && (*table[target_column_index - 1])[i] != "NULL")
        {
            const Column* target_column = table[target_column_index - 1];
            std::string column_type = target_column->getColumnType();

            if (column_type == "integer")
            {
                const IntColumn* int_col = dynamic_cast<const IntColumn*>(target_column);
                double value = int_col->parseValue(i);
                if (value > max_value)
                {
                    max_value = value;
                }
            }
            else if (column_type == "double")
            {
                const DoubleColumn* double_col = dynamic_cast<const DoubleColumn*>(target_column);
                double value = double_col->parseValue(i);
                if (value > max_value)
                {
                    max_value = value;
                }
            }
        }
    }

    return max_value;
}

double DatabaseService::minimum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const
{
    double min_value = std::numeric_limits<double>::max();

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); ++i)
    {
        if ((*table[search_column_index - 1])[i] == search_value && (*table[target_column_index - 1])[i] != "NULL")
        {
            const Column* target_column = table[target_column_index - 1];
            std::string column_type = target_column->getColumnType();

            if (column_type == "integer")
            {
                const IntColumn* int_col = dynamic_cast<const IntColumn*>(target_column);
                double value = int_col->parseValue(i);
                if (value < min_value)
                {
                    min_value = value;
                }
            }
            else if (column_type == "double")
            {
                const DoubleColumn* double_col = dynamic_cast<const DoubleColumn*>(target_column);
                double value = double_col->parseValue(i);
                if (value < min_value)
                {
                    min_value = value;
                }
            }
        }
    }

    return min_value;
}

void DatabaseService::visualizationInfo() const
{
    std::cout << std::setw(20) << std::left << "next"
        << "Shows the next 10 rows of the table" << std::endl;
    std::cout << std::setw(20) << std::left << "previous"
        << "Shows the previous 10 rows of the table" << std::endl;
    std::cout << std::setw(20) << std::left << "exit"
        << "Exits the visualization mode" << std::endl;
}

DatabaseService& DatabaseService::getInstance()
{
	static DatabaseService instance;
	return instance;
}

void DatabaseService::run()
{
}