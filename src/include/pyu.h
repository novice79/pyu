#pragma once

#include "log.h"
#include "sol/sol.hpp"
#include "db.h"
namespace pyu
{
    inline std::shared_ptr<Log> create_logger(std::string log_path = ".", std::string name = "py")
    {
        return std::make_shared<Log>(std::move(log_path), std::move(name));
    }
    inline std::shared_ptr<DB> create_db(std::string db_path = "py.db")
    {
        return std::make_shared<DB>(std::move(db_path));
    }
}
