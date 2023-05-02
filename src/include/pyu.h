#pragma once
#include <chrono>
#include "log.h"
#include "sol/sol.hpp"
#include "db.h"
#include "QrToPng.h"
#include "p7z.h"
#include "file.hpp"

namespace pyu
{
    inline std::shared_ptr<Log> create_logger(fs::path log_path = ".", std::string name = "py")
    {
        return std::make_shared<Log>(std::move(log_path), std::move(name));
    }
    inline std::shared_ptr<DB> create_db(fs::path db_path = "py.db")
    {
        return std::make_shared<DB>(std::move(db_path));
    }
    inline std::shared_ptr<FileMgr> create_fm(fs::path magic_path)
    {
        return std::make_shared<FileMgr>(std::move(magic_path));
    }
    fs::path exe_path(fs::path argv0);
    
    template<typename R, typename... ARGS>
    auto debounce(std::function<R(ARGS...)>f, int period)
    {
        using namespace std::chrono;
        static auto start = high_resolution_clock::now();
        static std::mutex start_mutex;
        auto fn = [f=std::move(f), period](ARGS... args){
            bool b(false);
            auto now = high_resolution_clock::now();
            std::lock_guard<std::mutex> guard(start_mutex);
            if (duration_cast<milliseconds>(now - start).count() > period){
                f(args...);
                start = high_resolution_clock::now();
                b = true;
            }
            return b;
        };
        return fn;
    }
}
