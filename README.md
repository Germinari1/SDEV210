# Project
Starter code for the CSCI 210 DB project.


## Raw strings:
C++ supports multiline strings using raw string literals.
```
std::string query = R"(
    CREATE TABLE Users (
        ID INT PRIMARY KEY,
        Name VARCHAR(255),
        Age INT
    )
)";
```


## SQL Injection:
A type of cyberattack that targets an sql database. Essentially the attacker attempts to manipulate input fields in an 
application so that it tricks application into executing unintended SQL commands. 

### SQL Injection Example:
Let's say we have a simple web app that takes user input for a search query
```
SELECT * FROM users WHERE username=<user_input_>;
```
This a normal query, however let's say the input is " '' OR '1'='1'. Now the 
resulting SQL query is:
```
SELECT * FROM users WHERE username='' OR '1'='1';
```
As a result, this conditional will always evaluate to true, effectively bypassing any potential
authentication check that we have, and returning all rows from the 'users' table.

### How to prevent SQL Injection:
1. Use parameterized queries: User input is treated as data and not as part of the statement. This way the user input is not evaluated as SQL code.

2. Input validation and sanitization: A common way is to validate and sanitize all user input so that 
they don't contain malicious code or special characters that could be used in an 
SQL injection attack

In C++, SQL injection prevention involves using parameterized queries or escaping user input when constructing SQL queries.


# Credits:
1. [Primary key in SQL Server](https://www.atlassian.com/data/admin/how-to-define-an-auto-increment-primary-key-in-sql-server)
