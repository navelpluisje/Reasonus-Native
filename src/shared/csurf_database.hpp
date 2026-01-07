// Database.h
#ifndef CSURF_DATABASE_H
#define CSURF_DATABASE_H

#include <sqlite3.h>
#include <string>

class Database
{
public:
    Database(const std::string &dbName);
    ~Database();
    bool execute(const std::string &sql);

    std::vector<std::tuple<int, std::string, std::string>> FetchPlugins();

private:
    sqlite3 *db;
    void populateDatabase();
};

#endif // CSURF_DATABASE_H