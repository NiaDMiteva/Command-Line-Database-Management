# Database Management System (DBMS)

This repository hosts an Object-Oriented Programming (OOP) project developed for an FMI course. The project deals with basic database operations. Databases consist of tables stored in separate files, managed by a master file. Supported data types include integers, floating-point numbers, and strings. 

Operations Supported:
- **close, save, saveas, help**: Typical.
- **import**: Adds a new table to the database from a file.
- **showtables**: Displays a list of all loaded tables.
- **describe**: Shows information about the column types of a specific table.
- **print**: Displays all rows of a given table, allowing navigation through pages with commands like next page, previous page, and exit.
- **export**: Writes a table to a file.
- **select**: Displays all rows from a table containing the specified value in the designated column, with pagination support.
- **addcolumn**: Adds a new column with the highest number to a given table.
- **update**: Updates values in a table based on specified conditions, supporting NULL values.
- **delete**: Deletes rows from a table based on specified conditions.
- **insert**: Inserts a new row into a table with corresponding values.
- **innerjoin**: Performs an Inner Join operation on two tables, creating a new table and displaying its identifier.
- **rename**: Renames a table, printing an error if the new name is not unique.
- **count**: Counts the number of rows in a table matching specified criteria.
- **aggregate**: Performs an aggregate operation on values in a column based on specified conditions, with supported operations.
