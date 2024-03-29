#pragma once
#include <string>
#include "sqlite3.h"

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace json = boost::json;
class DB
{
private:
    sqlite3 *db_;
    bool is_opened_;
public:
    DB(fs::path path = "py.db");
    ~DB();
    json::object exec_sql(const std::string& sql);
    std::string get_pass();
    int count(std::string select);
private:
    void init();
    int usr_count();
};
