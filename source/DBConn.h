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
    */
    bool executeSQL(const std::string& sqlQuery) {
        // Convert string into wstring
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring sqlQueryW = converter.from_bytes(sqlQuery);

        // Execute SQL Statement, and then return the success flag
        SQLRETURN retcode = SQLExecDirectW(hStmt, (SQLWCHAR*)sqlQueryW.c_str(), SQL_NTS);

        // If we failed, log the sql error
        if (!SQL_SUCCEEDED(retcode)) {
            SQLSMALLINT recordNumber = 1;
            SQLWCHAR sqlState[6];
            SQLINTEGER nativeError;
            SQLWCHAR messageText[1024]; // Increased buffer size
            SQLSMALLINT textLength;
            SQLRETURN diagRecRet;
            while (SQL_SUCCESS == (diagRecRet = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, recordNumber++, sqlState, &nativeError, messageText, sizeof(messageText), &textLength))) {
                std::wcerr << "SQL Error " << nativeError << ": " << std::wstring(messageText) << std::endl;
            }
        }
        return SQL_SUCCEEDED(retcode);
    };
    
    // Gets the primary key value of the last inserted row
    int getLastInsertedID() {
        // Execute the SQL statement to retrieve the last inserted ID
        if (!executeSQL("SELECT @@IDENTITY AS LastID")) {
            throw std::runtime_error("Failed to retrieve last inserted ID");
        }

        // Bind the result set column to a C variable
        SQLINTEGER lastID;
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

    // Destructor frees the statement handle.
    ~DBConn() {
        if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
};

#endif