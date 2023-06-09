#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <cstdio>
#include <string>
#include <vector>

#include "sqlite3.h"

#define PRINT_MESSAGES

#ifdef PRINT_MESSAGES
#define _PF_DB_OPEN_ fprintf(stderr, "return code %d during openning database!\n", rc);
#define _PF_DB_CLOSE_ fprintf(stderr, "return code %d during closing database!\n", rc);
#define _PF_DB_CLOSE_ fprintf(stderr, "return code %d during closing database!\n", rc);
#define _PF_PREPARE_ fprintf(stderr, "return code %d during preparing statement in %s()!\n", rc, __func__);
#define _PF_BIND_ fprintf(stderr, "return code %d during binding param in %s()!\n", rc, __func__);
#define _PF_STEP_ fprintf(stderr, "return code %d during step operation in %s()!\n", rc, __func__);
#define _PF_STMT_CLOSE_ fprintf(stderr, "return code: %d during closing prep_stmt in %s()!\n", rc, __func__);
#define _PF_STMT_RESET_ fprintf(stderr, "return code: %d during resetting prep_stmt in %s()!\n", rc, __func__);
#define _PF_SKIPPED_OPEN_ fprintf(stderr, "%s() skipped - open database first\n", __func__);
#define _PF_SKIPPED_PREP_ fprintf(stderr, "%s() skipped - prepare query first\n", __func__);
#else
#define _PF_DB_OPEN_ 
#define _PF_DB_CLOSE_ 
#define _PF_DB_CLOSE_ 
#define _PF_PREPARE_ 
#define _PF_BIND_ 
#define _PF_STEP_ 
#define _PF_STMT_CLOSE_ 
#define _PF_STMT_RESET_ 
#define _PF_SKIPPED_OPEN_ 
#define _PF_SKIPPED_PREP_ 
#endif // PRINT_MESSAGES

/*
SQLITE_OK           0   // Successful result 
SQLITE_ERROR        1   // Generic error 
SQLITE_INTERNAL     2   // Internal logic error in SQLite 
SQLITE_PERM         3   // Access permission denied 
SQLITE_ABORT        4   // Callback routine requested an abort 
SQLITE_BUSY         5   // The database file is locked 
SQLITE_LOCKED       6   // A table in the database is locked 
SQLITE_NOMEM        7   // A malloc() failed 
SQLITE_READONLY     8   // Attempt to write a readonly database 
SQLITE_INTERRUPT    9   // Operation terminated by sqlite3_interrupt()
SQLITE_IOERR       10   // Some kind of disk I/O error occurred 
SQLITE_CORRUPT     11   // The database disk image is malformed 
SQLITE_NOTFOUND    12   // Unknown opcode in sqlite3_file_control() 
SQLITE_FULL        13   // Insertion failed because database is full 
SQLITE_CANTOPEN    14   // Unable to open the database file 
SQLITE_PROTOCOL    15   // Database lock protocol error 
SQLITE_EMPTY       16   // Internal use only 
SQLITE_SCHEMA      17   // The database schema changed 
SQLITE_TOOBIG      18   // String or BLOB exceeds size limit 
SQLITE_CONSTRAINT  19   // Abort due to constraint violation 
SQLITE_MISMATCH    20   // Data type mismatch 
SQLITE_MISUSE      21   // Library used incorrectly 
SQLITE_NOLFS       22   // Uses OS features not supported on host 
SQLITE_AUTH        23   // Authorization denied 
SQLITE_FORMAT      24   // Not used 
SQLITE_RANGE       25   // 2nd parameter to sqlite3_bind out of range 
SQLITE_NOTADB      26   // File opened that is not a database file 
SQLITE_NOTICE      27   // Notifications from sqlite3_log() 
SQLITE_WARNING     28   // Warnings from sqlite3_log() 
SQLITE_ROW         100  // sqlite3_step() has another row ready 
SQLITE_DONE        101  // sqlite3_step() has finished executing 
*/

class SQLite3{
    sqlite3* db;
    bool db_open;

    sqlite3_stmt* prep_stmt;
    bool prep_started;

public:
    SQLite3(const std::string& path);
    ~SQLite3();

protected:
    std::string read_row(sqlite3_stmt* stmt) const;

public:
    void prepare_query(const std::string& query);

    void bind_int(int pos, int param);
    void bind_double(int pos, double param);
    void bind_text(int pos, std::string param);
    void bind_null(int pos);
    void binds_clear();

    std::vector<std::string> execute_prepared_query();
    void clear_prepared_query();
};

#endif