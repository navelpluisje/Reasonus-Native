#include "csurf_database.hpp"
#include <iostream>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"

Database::Database(const std::string &dbName)
{
    std::string dbPath = GetReaSonusFolderPath() + pathSeparator + dbName;

    if (sqlite3_open(dbPath.c_str(), &db))
    {
        ShowConsoleMsg(("Can't open database: " + std::string(sqlite3_errmsg(db))).c_str());
    }
    else
    {
        populateDatabase();
        ShowConsoleMsg("Hmmmm, have it?");
    }
}

Database::~Database()
{
    sqlite3_close(db);
}

bool Database::execute(const std::string &sql)
{
    char *errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        ShowConsoleMsg("\n");
        ShowConsoleMsg(errMsg);
        ShowConsoleMsg("\n");
        ShowConsoleMsg(sql.c_str());
        ShowConsoleMsg("\n");
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void Database::populateDatabase()
{
    execute("CREATE TABLE IF NOT EXISTS plugins (id INTEGER PRIMARY KEY, name TEXT, developer TEXT, type TEXT, pluginId TEXT, originalName TEXT,  reaperName TEXT, timestamp TEXT, instrument INTEGER);");
    execute("CREATE TABLE IF NOT EXISTS params (id INTEGER PRIMARY KEY, pluginId TEXT, name TEXT, paramIdx INTEGER,  minVal REAL, maxVal real, steps INTEGER);");
}

std::vector<std::tuple<int, std::string, std::string>> Database::FetchPlugins()
{
    std::vector<std::tuple<int, std::string, std::string>> results;
    const char *sql = "SELECT id, reaperName, pluginId FROM plugins;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return results;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        const char *pluginId = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        results.emplace_back(id, name ? name : "Unknown Name", pluginId ? pluginId : "Unknown Id");
    }

    sqlite3_finalize(stmt);
    return results;
}