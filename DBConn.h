#ifndef DBConn_H
#define DBConn_H

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <locale>
#include <codecvt>
#include <string>

class DBConn {
public:
    SQLHSTMT hStmt; // Handle to the statement.
    SQLHDBC hDbc;   // Copy of the database connection handle for operations.

    // Constructor takes a database connection handle and allocates a statement handle.
    DBConn(SQLHDBC hDbc) : hDbc(hDbc), hStmt(NULL) {
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    }

    /*
    + Executes an sql query
    NOTE: SQLExecDirectW expects a 'SQLWCHAR*' string for the SQL query,
        which uses Unicode (wide character) encoding. It's more common to use
        this over SQLExecDirectA as it supports more characters. As a result, we'd convert a
        regular string into a 'wide string'. Then c_str() is able to turn that into a 
        SQLWCHAR* for us.

    - Return false when SQL_ERROR so when something failed. We do this over !SQL_SUCCESS because this 
      allows us to return true, even if no rows were affected. So the query still ran, it just didn't 
      affect anything. This helps in the edge case where we're trying to delete a supplier that has 
      no products. We're able to delete products by supplier_id without having to worry about throwing 
      an error due to executeSQL returning false becasue it affected no rows.

    - Memory access violation happened at fetchRow when we were creating and then updating the same 
    customer. Something was going wrong with fetchRow and how it detected if a customer was there. I think i
    'fixed' it by minimizing when I pass by reference because during my last 3 tests nothing bad happened. But I still don't know the solution to that mystery and it's actually frustrating.
    */
    bool executeSQL(const std::string& sqlQuery) {
        // Convert string into wstring
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring sqlQueryW = converter.from_bytes(sqlQuery);

        // Execute SQL Statement, and then return the success flag
        SQLRETURN retcode = SQLExecDirectW(hStmt, (SQLWCHAR*)sqlQueryW.c_str(), SQL_NTS);

        if (SQL_ERROR == retcode) {
            logSQLError();
            return false;
        }

        return true;
    };

    // Logs out SQL errors to console
    void logSQLError() {
        SQLSMALLINT recordNumber = 1;
        SQLWCHAR sqlState[6];
        SQLINTEGER nativeError;
        SQLWCHAR* messageText = nullptr; // Pointer to error message buffer
        SQLSMALLINT textLength;
        SQLRETURN diagRecRet;

        // Loop to retrieve and process error messages
        while (SQL_SUCCESS == (diagRecRet = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, recordNumber++, sqlState, &nativeError, NULL, 0, &textLength))) {
            // Allocate memory for the error message buffer
            messageText = new SQLWCHAR[textLength + 1];

            // Retrieve the error message
            SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, recordNumber - 1, sqlState, &nativeError, messageText, textLength + 1, &textLength);

            // Output the error message
            std::wcerr << "SQL Error " << nativeError << ": " << std::wstring(messageText) << std::endl;

            // Free the memory for the error message buffer
            delete[] messageText;
            messageText = nullptr;
        }

        // Check if an error occurred during the last iteration
        if (diagRecRet != SQL_NO_DATA) {
            // Error occurred while fetching error message
            std::wcerr << "Error occurred while fetching error message" << std::endl;
            // Free the memory for the error message buffer if not already done
            if (messageText != nullptr) {
                delete[] messageText;
            }
        }
    }
  
    // Gets the primary key value of the last inserted row
    int getLastInsertedID() {
        // Execute the SQL statement to retrieve the last inserted ID
        if (!executeSQL("SELECT @@IDENTITY AS LastID")) {
            throw std::runtime_error("Failed to retrieve last inserted ID");
        }

        // Bind the result set column to a C variable
        SQLINTEGER lastID = 0;
        SQLRETURN retcode = SQLBindCol(hStmt, 1, SQL_C_SLONG, &lastID, sizeof(SQLINTEGER), NULL);
        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
            // Handle error
            throw std::runtime_error("Failed to bind result set column");
        }

        // Fetch the result
        retcode = SQLFetch(hStmt);
        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
            // Handle error
            throw std::runtime_error("Failed to fetch result set");
        }

        // Clean up and return the ID we got as an integer
        SQLFreeStmt(hStmt, SQL_CLOSE);
        return static_cast<int>(lastID);
    }

    // Attempts to create a new database
    void createDatabase(const std::string& dbName) {    
        // Construct query string
        std::string query = "CREATE DATABASE " + dbName;
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to create database '" + dbName + "'!");
        }
    }

    // Deletes a database
    void dropDatabase(const std::string& dbName) {
        // Create query to drop the database
        std::string query = "DROP DATABASE " + dbName;
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to delete database '" + dbName + "'!");
        }
    }

    // Select/switch the active database 'context
    void useDatabase(const std::string& dbName) {
        // Create query to use/select a database 
        const std::string query = "USE " + dbName;
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to use/select database '" + dbName + "'!");
        }
    }

    // Checks if a database exists with a given name
    bool dbExists(const std::string& dbName) {
        // Prepare the SQL query to check if the database exists
        std::string query = "IF EXISTS(SELECT * FROM master.sys.databases WHERE name='" + dbName + "') BEGIN SELECT 1 END ELSE BEGIN SELECT 0 END;";

        // Execute the query
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to execute query to check database existence.");
        }

        // Fetch the result
        SQLLEN count = 0;
        SQLRETURN retcode = SQLFetch(hStmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            SQLGetData(hStmt, 1, SQL_C_SLONG, &count, sizeof(count), NULL);
        } else {
            throw std::runtime_error("Failed to fetch result of query to check database existence.");
        }

        // Free the statement handle
        SQLFreeStmt(hStmt, SQL_CLOSE);

        // If count is 1, the database exists; otherwise, it doesn't
        return count == 1;
    }

    // Checks if a table exists with a given name
    bool tableExists(const std::string& tableName) {
        // Prepare the SQL query to check if the table exists
        std::string query = "IF EXISTS(SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = '" + tableName + "') BEGIN SELECT 1 END ELSE BEGIN SELECT 0 END;";

        // Execute the query
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to execute query to check table existence.");
        }

        // Fetch the result
        SQLLEN count = 0;
        SQLRETURN retcode = SQLFetch(hStmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            SQLGetData(hStmt, 1, SQL_C_SLONG, &count, sizeof(count), NULL);
        } else {
            throw std::runtime_error("Failed to fetch result of query to check table existence.");
        }

        // Free the statement handle
        SQLFreeStmt(hStmt, SQL_CLOSE);

        // If count is 1, the table exists; otherwise, it doesn't
        return count == 1;
    }


    /*
    - Returns true if a row exists in a given table named <tableName> with 
       corresponding column
    
    NOTE: If value has single quotes, ensure that they are escaped using escapeSQL before 
    passing it into isValidRow.
    */
    template<typename T>
    bool isValidRow(std::string tableName, std::string colName, T value) {
        bool isValid = true;

        // Create the query string
        std::string query = "SELECT " + colName + " FROM " + tableName + " WHERE " + colName + " = " + std::to_string(value);

        // Execute the query
        if (!executeSQL(query)) {
            throw std::runtime_error("Failed to check IF " + colName + "(" + std::to_string(value) + ") was valid!");
        }

        // Try to fetch the row data, if row wasn't found, set isValid to false to indicate no
        SQLRETURN retcode = fetchRow();
        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
            isValid = false;
        }

        // Close cursor since we fetched a row using fetchRow. Then return boolean.
        closeCursor();
        return isValid;
    }


    /*
    - Used to bind variables to column values when we fetch our data from our database.

    1. colNum: Number of the column we want to bind.
    2. targetType: Specifies C data-type to which the column should be bound So 
       'SQL_C_CHAR' is for character data types, and 'SQL_C_SLONG' is for signed long
       integer data types.
    3. targetValue: Pointer to the variable/buffer that will receive the data fetched from column
    4. bufferLength: Length of the variable/buffer pointed to be 'targetValue'. 
    5. indicator: Pointer to a buffer where the function writes the length or indicator 
       value. For fixed-length data types, such as our string-related attributes where 
       we know their maximum length and they won't be null, NULL is used as the default parameter.
    */
    SQLRETURN bindColumn(int colNum, SQLSMALLINT targetType, SQLPOINTER targetValue, SQLLEN bufferLength, SQLLEN* indicator = NULL) {
        return SQLBindCol(hStmt, colNum, targetType, targetValue, bufferLength, indicator);
    }

    // Fetches data for a row
    SQLRETURN fetchRow() {
        return SQLFetch(hStmt);
    }

    // Closes SQL Cursor and releases resources; We'll do these after every fetchRow()
    SQLRETURN closeCursor() {
        return SQLFreeStmt(hStmt, SQL_CLOSE);
    }

    /*
    - Handles escaping single quotes in an SQL query.
    */
    std::string escapeSQL(std::string query) {
        size_t pos = 0; // Keep track of the index position in the string
        pos = query.find("'", pos); // try to find first index position of a single quote

        // Iterate while we continue finding index positions from query.find()
        while ((pos != std::string::npos)) {
            query.replace(pos, 1, "''"); // replace the single quote we found with two single quotes
            pos += 2; // Increase index by two to skip over the two single quotes we've just inserted
            pos = query.find("'", pos); // Try to find index for next single quote
        }
        return query;
    }

    // Destructor frees the statement handle.
    ~DBConn() {
        if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
};

#endif