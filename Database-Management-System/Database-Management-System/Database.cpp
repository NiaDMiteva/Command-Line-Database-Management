#include "Database.h"

void Database::import(const std::string & file_name)
{
    Table table(file_name);
    table.deserialize();

    for (size_t i = 0; i < tables.size(); i++)
    {
        if (tables[i].getTableName() == table.getTableName())
        {
            throw std::runtime_error("Table with the same name already exists!");
        }
    }

    tables.push_back(table);
}

std::ostream& Database::serialize(std::ostream& os) const
{
    os << tables.size() << '\n';

	for (size_t i = 0; i < tables.size(); i++)
	{
		os << tables[i].getTableName() << '\n'
            << tables[i].getFileName() << '\n';
        tables[i].serialize();
	}

	return os;
}

std::istream& Database::deserialize(std::istream& is)
{
    size_t count = 0;
    is >> count;

    is.ignore();
    for (size_t i = 0; i < count; i++)
    {
        std::string table_name;
        std::getline(is, table_name);

        std::string file_path;
        std::getline(is, file_path);

        Table table(file_path, table_name);
        tables.push_back(table);
        tables[i].deserialize();

        if (tables[i].getFileName().find("recovery-") != std::string::npos)
        {
            tables[i].changeFileName(file_path.erase(0, 9));
        }
    }

    return is;
}

std::ostream& Database::serializeWithRecovery(std::ostream& os) const
{
	os << tables.size() << '\n';

	for (size_t i = 0; i < tables.size(); i++)
	{
		os << tables[i].getTableName() << '\n'
			<< "recovery-" << tables[i].getFileName() << '\n';
		tables[i].serialize(true);
	}

	return os;
}

const Table& Database::operator[](size_t index) const
{
    if (index < 0 || index > tables.size() - 1)
    {
        throw std::runtime_error("Invalid index!");
    }

	return tables[index];
}

Table* Database::find(const std::string& name)
{
    std::vector<Table>::iterator found_table =
        std::find_if(tables.begin(), tables.end(),
            [&name](const Table& table)
            {
                return table.getTableName() == name; 
            });

    if (found_table != std::end(tables))
    {
        return &(*found_table);
	}
	else
	{
		return nullptr;
    }
}