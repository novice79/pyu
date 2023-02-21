#pragma once
#include <string>
#include "sqlite3.h"

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>
namespace json = boost::json;
class DB
{
private:
    sqlite3 *db_;
    bool is_opened_;
public:
    DB(std::string path = "py.db");
    ~DB();
    std::string exec_sql(const std::string& sql);
    std::string get_pass();
private:
    void init();
    int usr_count();
};
