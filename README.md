# SimpleRA-Extended
## Description
This project improves upon the [SimpleRA](https://github.com/SimpleRA/SimpleRA) RDBMS built in C++ and introduces several key features such as JOIN , GROUP BY and secondary indexing with additional support for matrix operations such as cross-transpose and checking for anti-symmetry with space optimized implementation.
## Key Features
- **In-place operations** using minimal temporary storage of upto 10 blocks.
- Supports matrices up to **\(10^4 X 10^4\) elements.**
- **Matrix Operations**:  
  - Load, Print, Export  
  - Cross Transpose  
  - Anti-symmetry Check   
- **SORT & ORDER BY** performed using block-wise external sort.
- **GROUP BY**:  
  - Supports `SUM`, `COUNT`, `AVG`, `MIN`, `MAX`    
  - HAVING clause support with filtering after aggregation  
- **JOIN (Partitioned Hash Join)**:  
  - Hash-based partitioning of source tables  
  - Join done partition-wise using in-memory hash tables  
  - External sort used on final results 

- **Clustering-Based Secondary Index**:  
  - Two-level structure with Index + Block Pointer (BP) files  
  - Batch-wise sorted merges for index creation  
  - Indexed columns mapped to (pageNo, recordNo) pairs  
  - Supports SEARCH, DELETE and INSERT operations.

- Robust error handling for invalid data, syntax and semantics.
## Compilation Instructions

We use ```make``` to compile all the files and creste the server executable.
```cd``` into the SimpleRA directory
```
cd SimpleRA
```
```cd``` into the soure directory (called ```src```)
```
cd src
```
To compile
```
make clean
make
```

## To run

Post compilation, an executable named ```server``` will be created in the ```src``` directory
```
./server
```


## Assignment Commands

### CROSS
**Syntax:** `relation_name <- CROSS relation_name relation_name`

**Description:** Creates a cross product (Cartesian product) of two relations, assigning the result to a new relation.

### DISTINCT
**Syntax:** `relation_name <- DISTINCT relation_name`

**Description:** Removes duplicate rows from a relation, assigning the result to a new relation.

### JOIN
**Syntax:** `relation_name <- JOIN relation_name , relation_name ON column_name , column_name`

**Description:** Performs an inner join between two relations based on a specified condition, assigning the result to a new relation.

### PROJECT
**Syntax:** `relation_name <- PROJECT projection_list FROM relation_name`

**Description:** Selects specified columns from a relation, assigning the result to a new relation. The projection_list is a comma-separated list of column names.

### SELECT
**Syntax:** `relation_name <- SELECT condition FROM relation_name`

**Description:** Filters rows from a relation based on a specified condition, assigning the result to a new relation. Conditions can compare columns or compare a column to an integer literal.

### SORT
**Syntax:** `relation_name <- SORT relation_name BY column_name IN sorting_order`

**Description:** Sorts a relation by the specified column in ascending (ASC) or descending (DESC) order, assigning the result to a new relation.

## Non-Assignment Commands

### CLEAR
**Syntax:** `CLEAR relation_name`

**Description:** Removes a relation from the database.

### INDEX
**Syntax:** `INDEX ON column_name FROM relation_name USING indexing_strategy`

**Description:** Creates an index on a specified column using the given indexing strategy (HASH, BTREE, or NOTHING).

### LIST
**Syntax:** `LIST TABLES`

**Description:** Displays a list of all tables currently in the database.

### LOAD
**Syntax:** `LOAD relation_name`

**Description:** Loads a relation from storage into the database.

### PRINT
**Syntax:** `PRINT relation_name`

**Description:** Displays the contents of a relation.

### QUIT
**Syntax:** `QUIT`

**Description:** Exits the database system.

### RENAME
**Syntax:** `RENAME column_name TO column_name FROM relation_name`

**Description:** Renames a column in the specified relation.

### SOURCE
**Syntax:** `SOURCE file_name`

**Description:** Executes commands from a specified file.

## Operators

### Binary Operators (binop)
- `>` - Greater than
- `<` - Less than
- `==` - Equal to
- `!=` - Not equal to
- `<=` - Less than or equal to
- `>=` - Greater than or equal to
- `=>` - Greater than or equal to (alternative syntax)
- `=<` - Less than or equal to (alternative syntax)

## Notes

- Assignment commands create new relations and assign results to them
- Non-assignment commands perform operations without creating new relations
- Column names and relation names are case-sensitive
- Integer literals can be used in selection conditions
- Multiple columns in projection lists are separated by commas
## **Contributors**

- [Ishan Gupta](https://github.com/Ishan-1)
- [Aditya Garg](https://github.com/Adityagargiiith) 
- [Harsh Gupta](https://github.com/harsh-gupta10)
    

