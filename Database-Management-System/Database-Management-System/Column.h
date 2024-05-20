#pragma once
#include <string>
#include <vector>

class Column
{
private:
    std::string column_name;
    std::vector<std::string> cell_values;
    virtual bool validateValue(const std::string& value) const = 0;

public:
    Column(const std::string& name = "", size_t cell_count = 0);
    virtual ~Column() = default;
    virtual Column* clone() const = 0;
    void appendValue(const std::string& value);
    virtual const std::string getColumnType() const = 0;
    const std::string& getColumnName() const
    {
        return column_name;
    }
    size_t getCellCount() const
    {
        return cell_values.size();
    }
    std::string& operator[](size_t index)
    {
        return cell_values[index];
    }
    const std::string& operator[](size_t index) const;
    void removeValue(size_t index);
    static Column* createColumn(const std::string& columnType, const std::string& columnName = "", size_t cellCount = 0);
    void updateValue(size_t index, const std::string& value);
};
