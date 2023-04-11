#include "db.h"

using namespace std;

DB::DB(fs::path path)
{
    fs::create_directories(path.parent_path());
    int rc = sqlite3_open(path.string().c_str(), &db_);
    if (rc)
    {
        printf("Opened sqlite database failed: %s\n", sqlite3_errmsg(db_)); 
        is_opened_ = false;
    }
    else
    {
        printf("Opened sqlite database successfully\n"); 
        is_opened_ = true;
        init();
    }
}
DB::~DB()
{
    if(is_opened_)
    {
        sqlite3_close(db_);
    }
}
std::string DB::exec_sql(const std::string& sql)
{
    json::object j;
    j["result"] = json::array();
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        j["ret"] = -1;
        j["msg"] = sqlite3_errmsg(db_);
        return json::serialize( j );
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        json::object row;
        int num_cols = sqlite3_column_count(stmt);
        for (int i = 0; i < num_cols; ++i)
        {           
            switch (sqlite3_column_type(stmt, i))
            {
            case (SQLITE3_TEXT):
                row[sqlite3_column_name(stmt, i)] = (char*)sqlite3_column_text(stmt, i);
                break;
            case (SQLITE_INTEGER):
                row[sqlite3_column_name(stmt, i)] = sqlite3_column_int(stmt, i);
                break;
            case (SQLITE_FLOAT):
                row[sqlite3_column_name(stmt, i)] = sqlite3_column_double(stmt, i);
                break;
            default:
                break;
            }       
        }
        if(!row.empty())
        {
            j["result"].as_array().push_back(row);
        }
    }
    if (rc != SQLITE_DONE) {
        j["ret"] = -1;      
        j["msg"] = sqlite3_errmsg(db_);       
    }
    else 
    {
        j["ret"] = 0;
    }
    sqlite3_finalize(stmt);
    if ( !boost::ifind_first(sql, "select ") )   
    {
        j.erase("result");
    }
    // if(j["result"].empty()) j.erase("result");
    return json::serialize( j );
}
int DB::usr_count()
{
    string sql = R"(
        select count(*) from user;
    )";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return 0;
    }
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}
void DB::init()
{
    string sql = R"(
        create table if not exists user (
            _id integer primary key autoincrement not null,
            pass text
        );
    )";
    exec_sql(sql);
    int count = usr_count();
    printf("usr_count=%d\n", count);
    if(count == 0)
    {
        sql = R"(
            insert into user (pass) values ("admin");
        )";
        exec_sql(sql);
        printf("begin insert default pass -----------------------------------\n");
    }
}
std::string DB::get_pass(){
    string sql = R"(
        select pass from user where _id=1;
    )";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("get pass failed? This will never happen");
        return "";
    }
    sqlite3_step(stmt);
    string pass = (char*)sqlite3_column_text(stmt, 0);
    sqlite3_finalize(stmt);
    return pass;
}
