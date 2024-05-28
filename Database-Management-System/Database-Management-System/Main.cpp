#include <iostream>
#include <fstream>
#include "IntColumn.h"
#include "DoubleColumn.h"
#include "StringColumn.h"
#include "DatabaseService.h"
#include "Table.h"
#include "Database.h"

int main()
{
    DatabaseService::getInstance().run();

    return 0;
}