#pragma once

#include "log.h"
#include "sol/sol.hpp"
#include "db.h"
#include "QrToPng.h"
#include "p7z.h"
#include "file.h"

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
    inline std::shared_ptr<FileMgr> create_fm(std::string magic_path)
    {
        return std::make_shared<FileMgr>(std::move(magic_path));
    }
    
    fs::path exe_path(fs::path argv0);
}
