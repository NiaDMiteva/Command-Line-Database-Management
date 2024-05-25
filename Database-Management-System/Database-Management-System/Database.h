#pragma once
#include "Table.h"
#include "Serializable.h"

class Database : public Serializable
{
private:
	std::vector<Table> tables;

public:
	Database() : tables(9) {}
	void import(const std::string& file_name);

	size_t getTableCount() const
	{
		return tables.size();
	}

	const Table& operator[](size_t index) const;
	Table* find(const std::string& name);

	std::ostream& serialize(std::ostream& os) const override;
	std::istream& deserialize(std::istream& is) override;
	std::ostream& serializeWithRecovery(std::ostream& os = std::cout) const;

};