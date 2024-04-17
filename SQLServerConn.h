#ifndef SQLServerConn_H
#define SQLServerConn_H

#include <iostream>
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <locale>
#include <codecvt>

class SQLServerConn {
private:
    SQLHENV henv; // Environment handle
    SQLHDBC hdbc; // Connection handle

public:
    SQLServerConn() : henv(nullptr), hdbc(nullptr) {}

    ~SQLServerConn() {
        disconnect();
    }

    void connect(const std::string& connectionString) {
        // Allocate environment handle
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv))
            throw std::runtime_error("Failed to allocate environment handle.");

        SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        // Allocate connection handle
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc))
            throw std::runtime_error("Failed to allocate connection handle.");

        // Convert std::string to std::wstring, then c_str() can convert connectionStringW into a SQLWCHAR*
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring connectionStringW = converter.from_bytes(connectionString);

        // Connect to SQL Server
        SQLRETURN retcode = SQLDriverConnectW(hdbc, NULL, (SQLWCHAR*)connectionStringW.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
            throw std::runtime_error("Failed to connect to SQL Server.");
    }

    void disconnect() {
        if (hdbc != nullptr) {
            SQLDisconnect(hdbc);
            SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            hdbc = nullptr;
        }
        if (henv != nullptr) {
            SQLFreeHandle(SQL_HANDLE_ENV, henv);
            henv = nullptr;
        }
    }

    SQLHDBC getHDBC() {
        return hdbc;
    }
};

#endif