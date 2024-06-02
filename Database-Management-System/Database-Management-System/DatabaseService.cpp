#include <fstream>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cstdio>
#include "StringUtils.h"
#include "IntColumn.h"
#include "DoubleColumn.h"
#include "DatabaseService.h"
#include <iomanip>

void DatabaseService::saveTo(const std::string& file_name)
{
    std::ofstream os
    {
        file_name,
        std::ios::out 
    };

    if (!os)
    {
        throw std::runtime_error("Couldn't save file!");
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

DatabaseService& DatabaseService::getInstance()
{
    static DatabaseService instance;
    return instance;
}

DatabaseService::DatabaseService() : is_opened(false) {}

void DatabaseService::open(const std::string& file_name)
{
    std::ifstream recovery("recovery.txt", std::ios::in);
    bool recovery_file_exists = recovery.is_open();
    recovery.close();

    if (recovery_file_exists)
    {
        std::cout << "It seems the program didn't close properly last time."
            << "There is a recovery file containing all the changes made before.\n"
            << "Would you like to open the recovery file instead ? (y / n) : ";
        char user_response;
        std::cin >> user_response;
        std::cin.ignore();

        if (user_response == 'y' || user_response == 'Y')
        {
            std::ifstream recovery("recovery.txt", std::ios::in);
            database.deserialize(recovery);
            recovery.close();

            is_opened = true;
            file = "recovery.txt"; 
            std::cout << "Successfully opened the recovery file. New changes will be saved in: " << file << '\n';
            return;
        }
    }

    std::ifstream is(file_name, std::ios::in);
    if (!is.is_open())
    {
        std::cout << "Error: File " << file_name << " does not exist.\n";
        return;
    }

    database.deserialize(is);
    is.close();
    is_opened = true;
    file = file_name;
    std::cout << "Successfully opened " << file_name << '\n';

    for (size_t i = 0; i < database.getTableCount(); i++)
    {
        database[i].serialize(true);
    }
}

void DatabaseService::help() const noexcept 
{
    const std::string RESET = "\033[0m";
    const std::string YELLOW = "\u001b[33m";
    const std::string GREEN = "\u001b[32m";

    std::cout << "=================================================== Database Service Help ==================================================\n";
    std::cout << "Welcome to the Database Service! Below are the commands you can use:\n";
    std::cout << "============================================================================================================================\n\n";

    std::cout << std::setw(50) << std::left << "Command" << "Description\n";
    std::cout << "----------------------------------------------------------------------------------------------------------------------------\n";

    printCommand("open <file>", "Open a file and make it active. If the file does not exist, a new one will be created.");
    printCommand("close", "Close the currently active file without saving any changes.");
    printCommand("save", "Save changes to the active file.");
    printCommand("saveas <file>", "Save the active file with a new name.");
    printCommand("help", "Display this list of commands.");
    printCommand("exit", "Exit the program.");
    printCommand("import <file name>", "Import a table from a file into the current database.");
    printCommand("showtables", "Show all tables in the current database.");
    printCommand("describe <name>", "Display the schema of the specified table.");
    printCommand("print <name>", "Print all rows of the specified table.");
    printCommand("export <name> <file name>", "Export a table to a file.");
    printCommand("select <column-n> <value> <table name>", "Print rows containing a specific value in a column of a table.");
    printCommand("addcolumn <table name> <column name> <column type>", "Add a new column to the specified table.");
    printCommand("update <table name> <search column n> <search value> <target column n> <target value>", "Update values in a table based on a condition.");
    printCommand("delete <table name> <search column n> <search value>", "Delete rows based on a condition.");
    printCommand("insert <table name> <column1> ... <column n>", "Insert a new row into the specified table.");
    printCommand("innerjoin <table 1> <column n1> <table 2> <column n2>", "Join two tables based on a common column.");
    printCommand("rename <old name> <new name>", "Rename a table.");
    printCommand("count <table name> <search column n> <search value>", "Count rows based on a condition.");
    printCommand("aggregate <table name> <search column n> <search value> <target column n> <operation>", "Perform an operation on column values based on a condition.");
}

void DatabaseService::printCommand(const std::string& command, const std::string& description) const 
{
    const std::string RESET = "\033[0m";
    const std::string YELLOW = "\u001b[33m";
    const std::string GREEN = "\u001b[32m";

    std::cout << YELLOW << std::setw(50) << std::left << command << RESET;
    std::cout << "|";
    std::cout << GREEN << description << RESET << "\n";
    std::cout << "----------------------------------------------------------------------------------------------------------------------------\n";
}

void DatabaseService::save() 
{
    saveTo(file);
    std::remove("recovery.txt");

    for (size_t i = 0; i < database.getTableCount(); ++i) 
    {
        std::string recovery_file_name = "recovery-" + database[i].getFileName();
        std::remove(recovery_file_name.c_str());
    }

    is_opened = false;
    std::cout << "Successfully saved " << file << '\n';
    file = "";
    database = Database();
}


void DatabaseService::saveas(const std::string& file_name)
{
    saveTo(file_name);
    std::remove("recovery.txt");
    for (size_t i = 0; i < database.getTableCount(); i++)
    {
        std::string s = ("recovery-" + database[i].getFileName());
        std::remove(s.c_str());
    }

    is_opened = false;
    std::cout << "Successfully saved another " << file_name << '\n';
    file = "";
    database = Database();
}

void DatabaseService::close()
{
    std::remove("recovery.txt");
    for (size_t i = 0; i < database.getTableCount(); i++)
    {
        std::string s = ("recovery-" + database[i].getFileName());
        std::remove(s.c_str());
    }

    is_opened = false;
    std::cout << "Successfully closed " << file << '\n';
    file = "";
    database = Database();
}

void DatabaseService::showtables() const
{
    std::cout << "List of all tables: \n";
    for (size_t i = 0; i < database.getTableCount(); i++)
    {
        std::cout << i + 1 << ") " << database[i].getTableName() << '\n';
    }
}

void DatabaseService::describe(const std::string& table_name)
{
    Table* table = database.find(table_name);
    if (table != nullptr)
    {
        std::cout << "Types of table " << table_name << ":\n";
        for (size_t i = 0; i < table->getColumnCount(); i++)
        {
            std::cout << i + 1 << ") " << (*table)[i]->getColumnType() << '\n';
        }
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::print(const std::string& table_name)
{
    const std::string RESET = "\033[0m";
    const std::string YELLOW = "\u001b[33m";
    const std::string GREEN = "\u001b[32m";

    Table* table = database.find(table_name);
    if (table != nullptr) 
    {
        std::cout << GREEN << "\nYou are currently in visualization mode! Available commands:\n" << RESET;
        std::cout << YELLOW << "next" << GREEN << " - Go to the next page\n" << RESET;
        std::cout << YELLOW << "previous" << GREEN << " - Go to the previous page\n" << RESET;
        std::cout << YELLOW << "exit" << GREEN << " - Exit the visualization mode\n" << RESET;
        std::cout << '\n';

        size_t totalPages = (table->getRowCount() + 9) / 10;
        size_t current_page = 1;

        auto print_page = [&](size_t page) 
            {
            std::cout << YELLOW << "\nCurrent page: " << page << "\n\n" << RESET;
            for (size_t i = 0; i < table->getColumnCount(); ++i) 
            {
                std::cout << std::setw(20) << std::left << (*table)[i]->getColumnName() << ' ';
            }
            std::cout << '\n';

            size_t startRow = (page - 1) * 10;
            size_t endRow = std::min(table->getRowCount(), page * 10);
            for (size_t i = startRow; i < endRow; ++i) {
                for (size_t j = 0; j < table->getColumnCount(); ++j) 
                {
                    std::cout << std::setw(20) << std::left << (*(*table)[j])[i] << ' ';
                }
                std::cout << '\n';
            }
            std::cout << '\n';
            };

        print_page(current_page);

        std::string command;
        do 
        {
            std::cout << "> ";
            std::getline(std::cin, command);
            command = StringUtils::removeTrailingSpaces(StringUtils::removeLeadingSpaces(command));
            StringUtils::toLowerCase(command);

            if (command == "next" || command == "previous") 
            {
                if (command == "next" && current_page < totalPages) 
                {
                    ++current_page;
                }
                else if (command == "previous" && current_page > 1) 
                {
                    --current_page;
                }
                else 
                {
                    std::cout << GREEN << "No more pages!\n" << RESET;
                    continue;
                }
                print_page(current_page);
            }
            else if (command == "exit") 
            {
                std::cout << GREEN << "Exiting visualization mode...\n" << RESET;
            }
            else 
            {
                std::cout << GREEN << "Invalid command! Available commands:\n" << RESET;
                std::cout << YELLOW << "next" << GREEN << " - Go to the next page\n" << RESET;
                std::cout << YELLOW << "previous" << GREEN << " - Go to the previous page\n" << RESET;
                std::cout << YELLOW << "exit" << GREEN << " - Exit the visualization mode\n" << RESET;
            }
        } while (command != "exit");
    }
    else 
    {
        std::cout << GREEN << "Couldn't find table with specified name!\n" << RESET;
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
        std::cout << "Table exported successfully!\n";
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::select(size_t column, const std::string& value, const std::string& table)
{
    Table* search_table = database.find(table);
    if (search_table != nullptr)
    {
        std::vector<size_t> indexes;
        for (size_t i = 0; i < (*search_table)[column - 1]->getCellCount(); i++)
        {
            if ((*(*search_table)[column - 1])[i] == value)
            {
                indexes.push_back(i);
            }
        }

        std::cout << "\nYou are currently in visualization mode! Available commands: \n";
        visualizationInfo();

        size_t pages = indexes.size() / 10;
        pages += (indexes.size() % 10 != 0);
        size_t current_page = 1;

        std::cout << "\nCurrent page: 1\n";

        for (size_t i = 0; i < search_table->getColumnCount(); i++)
        {
            std::cout << std::setw(20) << std::left << (*search_table)[i]->getColumnName() << ' ';
        }
        std::cout << '\n';
        for (size_t i = 0; i < std::min(indexes.size(), current_page * 10); i++)
        {
            for (size_t j = 0; j < search_table->getColumnCount(); j++)
            {
                std::cout << std::setw(20) << std::left << (*(*search_table)[j])[indexes[i]] << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';

        std::string command;
        do
        {
            std::cout << "> ";
            std::getline(std::cin, command);
            command = StringUtils::removeTrailingSpaces(StringUtils::removeLeadingSpaces(command));
            StringUtils::toLowerCase(command);

            if (command == "next" || command == "previous")
            {
                bool isValid = true;
                if (command == "next" && pages > current_page)
                {
                    current_page++;
                }
                else if (command == "previous" && current_page > 1)
                {
                    current_page--;
                }
                else
                {
                    isValid = false;
                    std::cout << "No more pages!\n";
                }

                if (isValid)
                {
                    std::cout << "\nCurrent page: " << current_page << "\n";

                    for (size_t i = 0; i < search_table->getColumnCount(); i++)
                    {
                        std::cout << std::setw(20) << std::left << (*search_table)[i]->getColumnName() << ' ';
                    }
                    std::cout << '\n';
                    for (size_t i = (current_page - 1) * 10; i < std::min(indexes.size(), current_page * 10); i++)
                    {
                        for (size_t j = 0; j < search_table->getColumnCount(); j++)
                        {
                            std::cout << std::setw(20) << std::left << (*(*search_table)[j])[indexes[i]] << ' ';
                        }
                        std::cout << '\n';
                    }
                    std::cout << '\n';
                }
            }
            else if (command == "exit")
            {
                std::cout << "Exiting visualization mode...\n";
            }
            else
            {
                std::cout << "Invalid command! Available commands: \n";
                visualizationInfo();
            }
        } while (command != "exit");
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::addcolumn(const std::string& table_name, const std::string& column_name, const std::string& column_type)
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
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::update(const std::string& table_name, size_t search_column_index, const std::string& search_value, size_t target_column_index, const std::string& target_value)
{
    Table* search_table = database.find(table_name);
    if (search_table != nullptr)
    {
        for (size_t i = 0; i < (*search_table)[search_column_index - 1]->getCellCount(); i++)
        {
            if ((*(*search_table)[search_column_index - 1])[i] == search_value)
            {
                (*search_table)[target_column_index - 1]->updateValue(i, target_value);
            }
        }
        search_table->serialize(true);
        saveTo("recovery.txt");
        std::cout << "Table " << table_name << " successfully updated!\n";
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::deleteRows(const std::string& table_name, size_t search_column, const std::string& search_value)
{
    Table* search_table = database.find(table_name);
    if (search_table != nullptr)
    {
        for (size_t i = 0; i < (*search_table)[search_column - 1]->getCellCount(); i++)
        {
            if ((*(*search_table)[search_column - 1])[i] == search_value)
            {
                search_table->remove(i);
                i--;
            }
        }

        search_table->serialize(true);
        saveTo("recovery.txt");
        std::cout << "Rows deleted successfully!\n";
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::insert(const std::vector<std::string>& values_to_insert)
{
    Table* table = database.find(values_to_insert[0]);
    if (table != nullptr)
    {
        if (values_to_insert.size() - 1 > table->getColumnCount())
        {
            throw std::runtime_error("Invalid number of parameters!");
        }
        std::vector<std::string> values(values_to_insert);
        values.erase(std::begin(values));
        table->insertRow(values);

        table->serialize(true);
        saveTo("recovery.txt");
        std::cout << "Row inserted successfully!\n";
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::innerjoin(const std::string& table1, size_t column1, const std::string& table2, size_t column2)
{
    Table* first_table = database.find(table1);
    Table* second_table = database.find(table2);
    if (first_table != nullptr && second_table != nullptr)
    {
        Table result = innerJoin(*first_table, column1 - 1, *second_table, column2 - 1);
        result.serialize();
        database.import(result.getFileName());

        std::string first_name = first_table->getTableName();
        std::string second_name = second_table->getTableName();

        result.serialize(true);
        saveTo("recovery.txt");
        std::cout << "Successfully joined tables " << first_name << " and " << second_name << " into " << result.getTableName() << "!\n";
    }
    else
    {
        std::cout << "Couldn't find one of the tables!\n";
    }
}

void DatabaseService::rename(const std::string& old_name, const std::string& new_name)
{
    Table* table = database.find(old_name);
    if (table != nullptr)
    {
        if (database.find(new_name) != nullptr)
        {
            std::cout << "A table with this name already exists!\n";
        }
        else
        {
            table->rename(new_name);

            table->serialize(true);
            saveTo("recovery.txt");
            std::cout << "Table "
                << "\"" << old_name << "\" "
                << "renamed to "
                << "\"" << new_name << "\"!\n";
        }
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::count(const std::string& table_name, size_t search_column, const std::string& search_value)
{
    Table* search_table = database.find(table_name);
    if (search_table != nullptr)
    {
        size_t counter = 0;
        for (size_t i = 0; i < (*search_table)[search_column - 1]->getCellCount(); i++)
        {
            if ((*(*search_table)[search_column - 1])[i] == search_value)
            {
                counter++;
            }
        }
        std::cout << counter << " rows in column " << (*search_table)[search_column - 1]->getColumnName() << " contain value " << search_value << "!\n";
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

void DatabaseService::aggregate(const std::string& table_name, size_t search_column, const std::string& search_value, size_t target_column, const std::string& operation)
{
    Table* search_table = database.find(table_name);
    if (search_table != nullptr)
    {
        if ((*search_table)[target_column - 1]->getColumnType() == "string")
        {
            std::cout << "Target column is not of numeric type!\n";
        }
        else
        {
            if (operation == "sum")
            {
                double result = sum(*search_table, search_value, search_column, target_column);
                std::cout << "The sum is: " << result << '\n';
            }
            else if (operation == "product")
            {
                double result = product(*search_table, search_value, search_column, target_column);
                std::cout << "The product is: " << result << '\n';
            }
            else if (operation == "maximum")
            {
                double result = maximum(*search_table, search_value, search_column, target_column);
                std::cout << "The maximum value is: " << result << '\n';
            }
            else if (operation == "minimum")
            {
                double result = minimum(*search_table, search_value, search_column, target_column);
                std::cout << "The minimum value is: " << result << '\n';
            }
            else
            {
                std::cout << "Unrecognized operation!\n";
            }
        }
    }
    else
    {
        std::cout << "Couldn't find table with specified name!\n";
    }
}

double DatabaseService::sum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const
{
    double sum = 0;

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); i++)
    {
        if ((*(table[search_column_index - 1]))[i] == search_value)
        {

            if (table[target_column_index - 1]->getColumnType() == "integer")
            {
                const IntColumn* col = dynamic_cast<const IntColumn*>(table[target_column_index - 1]);
                sum += col->parseValue(i);
            }
            else if (table[target_column_index - 1]->getColumnType() == "double")
            {
                const DoubleColumn* col = dynamic_cast<const DoubleColumn*>(table[target_column_index - 1]);
                sum += col->parseValue(i);
            }
        }
    }

    return sum;
}

double DatabaseService::product(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const
{
    double product = 1;

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); i++)
    {
        if ((*(table[search_column_index - 1]))[i] == search_value && (*(table[target_column_index - 1]))[i] != "NULL")
        {

            if (table[target_column_index - 1]->getColumnType() == "integer")
            {
                const IntColumn* col = dynamic_cast<const IntColumn*>(table[target_column_index - 1]);
                product *= col->parseValue(i);
            }
            else if (table[target_column_index - 1]->getColumnType() == "double")
            {
                const DoubleColumn* col = dynamic_cast<const DoubleColumn*>(table[target_column_index - 1]);
                product *= col->parseValue(i);
            }
        }
    }

    return product;
}

double DatabaseService::maximum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const
{
    double maximum = std::numeric_limits<double>::lowest();

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); i++)
    {
        if ((*(table[search_column_index - 1]))[i] == search_value && (*(table[target_column_index - 1]))[i] != "NULL")
        {

            if (table[target_column_index - 1]->getColumnType() == "integer")
            {
                const IntColumn* col = dynamic_cast<const IntColumn*>(table[target_column_index - 1]);
                if (col->parseValue(i) > maximum)
                {
                    maximum = col->parseValue(i);
                }
            }
            else if (table[target_column_index - 1]->getColumnType() == "double")
            {
                const DoubleColumn* col = dynamic_cast<const DoubleColumn*>(table[target_column_index - 1]);
                if (col->parseValue(i) > maximum)
                {
                    maximum = col->parseValue(i);
                }
            }
        }
    }

    return maximum;
}

double DatabaseService::minimum(const Table& table, const std::string& search_value, size_t search_column_index, size_t target_column_index) const
{
    double minimum = std::numeric_limits<double>::max();

    for (size_t i = 0; i < table[search_column_index - 1]->getCellCount(); i++)
    {
        if ((*(table[search_column_index - 1]))[i] == search_value && (*(table[target_column_index - 1]))[i] != "NULL")
        {

            if (table[target_column_index - 1]->getColumnType() == "integer")
            {
                const IntColumn* col = dynamic_cast<const IntColumn*>(table[target_column_index - 1]);
                if (col->parseValue(i) < minimum)
                {
                    minimum = col->parseValue(i);
                }
            }
            else if (table[target_column_index - 1]->getColumnType() == "double")
            {
                const DoubleColumn* col = dynamic_cast<const DoubleColumn*>(table[target_column_index - 1]);
                if (col->parseValue(i) < minimum)
                {
                    minimum = col->parseValue(i);
                }
            }
        }
    }

    return minimum;
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

void DatabaseService::run()
{
    std::string command;
    do
    {
        std::cout << "> ";
        std::getline(std::cin, command);
        std::string space_delimiter = " ";
        std::vector<std::string> parameters{};
        command = StringUtils::removeTrailingSpaces(StringUtils::removeLeadingSpaces(command));

        size_t pos = command.find(space_delimiter);
        while (pos != std::string::npos)
        {
            parameters.push_back(command.substr(0, pos));
            command.erase(0, pos + space_delimiter.length());
            command = StringUtils::removeLeadingSpaces(command);
            pos = command.find(space_delimiter);
        }
        parameters.push_back(command);
        command = parameters[0];
        StringUtils::toLowerCase(command);
        parameters.erase(std::begin(parameters));

        if (command == "open" && !is_opened)
        {
            try
            {
                if (parameters.size() != 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                open(parameters[0]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "open" && is_opened)
        {
            std::cout << "There is a file currently opened!\n";
        }
        else if (command == "help")
        {
            try
            {
                if (parameters.size() != 0)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                help();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "save" && is_opened)
        {
            try
            {
                if (parameters.size() != 0)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                save();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "saveas" && is_opened)
        {
            try
            {
                if (parameters.size() != 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                saveas(parameters[0]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "close" && is_opened)
        {
            if (parameters.size() != 0)
            {
                throw std::runtime_error("Invalid number of parameters!");
            }
            close();
        }
        else if (command == "exit" && is_opened)
        {
            std::cout << "You have an open file with unsaved changes, please select close or save first.\n";
            command = "";
        }
        else if (command == "import" && is_opened)
        {
            try
            {
                if (parameters.size() != 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                database.import(parameters[0]);
                std::cout << "Table successfully imported to database!\n";
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "showtables" && is_opened)
        {
            try
            {
                if (parameters.size() != 0)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                showtables();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "describe" && is_opened)
        {
            try
            {
                if (parameters.size() != 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                describe(parameters[0]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "print" && is_opened)
        {
            try
            {
                if (parameters.size() != 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                print(parameters[0]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "export" && is_opened)
        {
            try
            {
                if (parameters.size() != 2)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                exportTable(parameters[0], parameters[1]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "select" && is_opened)
        {
            try
            {
                if (parameters.size() != 3)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t column = StringUtils::parseToInt(parameters[0]);

                select(column, parameters[1], parameters[2]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "addcolumn" && is_opened)
        {
            try
            {
                if (parameters.size() != 3)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                addcolumn(parameters[0], parameters[1], parameters[2]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "update" && is_opened)
        {
            try
            {
                if (parameters.size() != 5)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t searchColumn = StringUtils::parseToInt(parameters[1]);
                size_t targetColumn = StringUtils::parseToInt(parameters[3]);
                update(parameters[0], searchColumn, parameters[2], targetColumn, parameters[4]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "delete" && is_opened)
        {
            try
            {
                if (parameters.size() != 3)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t column = StringUtils::parseToInt(parameters[1]);
                deleteRows(parameters[0], column, parameters[2]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "insert" && is_opened)
        {
            try
            {
                if (parameters.size() < 1)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                insert(parameters);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "innerjoin" && is_opened)
        {
            try
            {
                if (parameters.size() != 4)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t column1 = StringUtils::parseToInt(parameters[1]);
                size_t column2 = StringUtils::parseToInt(parameters[3]);
                innerjoin(parameters[0], column1, parameters[2], column2);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "rename" && is_opened)
        {
            try
            {
                if (parameters.size() != 2)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                rename(parameters[0], parameters[1]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "count" && is_opened)
        {
            try
            {
                if (parameters.size() != 3)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t column = StringUtils::parseToInt(parameters[1]);
                count(parameters[0], column, parameters[2]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "aggregate" && is_opened)
        {
            try
            {
                if (parameters.size() != 5)
                {
                    throw std::runtime_error("Invalid number of parameters!");
                }
                size_t searchColumn = StringUtils::parseToInt(parameters[1]);
                size_t targetColumn = StringUtils::parseToInt(parameters[3]);
                aggregate(parameters[0], searchColumn, parameters[2], targetColumn, parameters[4]);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (command == "count" || command == "rename" || command == "innerjoin" || command == "insert" || command == "delete" || command == "update" || command == "addcolumn" || command == "select" || command == "export" || command == "print" || command == "describe" || command == "showtables" || command == "import" || command == "close" || command == "saveas" || command == "save" || command == "aggregate" && !is_opened)
        {
            std::cout << "There is not a file currently opened!\n";
        }
        else
        {
            if (command != "exit")
            {
                std::cout << "Invalid command! Type \"help\" to see list of commands!\n";
            }
            else
            {
                std::cout << "Exiting program...\n";
            }
        }
    } while (command != "exit");
}