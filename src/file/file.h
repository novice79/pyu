#pragma once

#include "magic.h"
#include <string>
#include <vector>
#include <mutex>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
namespace json = boost::json;
namespace fs = boost::filesystem;
int main_unrar(int argc, char *argv[]);
int main_7z(int argc, char *argv[]);
typedef int MainFunc(int argc, char *argv[]);
int call_main(MainFunc m, std::string args);

class FileMgr
{
    magic_t handle_;
    std::mutex handle_mutex_;
    bool magic_loaded_;
private:
    std::string concat_s(std::string s) 
    {
        return s;
    }
    template<typename... Args>
    std::string concat_s(std::string s, Args... args) // recursive variadic function
    {
        return s + " " + concat_s(args...) ;
    }
public:
    FileMgr(fs::path magic)
    {
        handle_ = ::magic_open(MAGIC_NONE | MAGIC_MIME);
        magic_loaded_ = ::magic_load( handle_, magic.string().c_str() ) == 0;
        if( magic_loaded_ )
        {
            printf("Load magic database succeed!\n");
        }
        else 
        {
            printf("Can not load magic database -%s\n", magic_error(handle_));
			magic_close(handle_);
        }
    }
    std::string file_type(const std::string &path)
    {
        std::lock_guard<std::mutex> guard(handle_mutex_);
        if( !magic_loaded_ )
        {
            printf("Can not load magic file, determine file type failed\n");
            return "application/octet-stream";
        }
        return ::magic_file(handle_, path.c_str());
    }
    template<typename... Args>
    int compress(std::string zip, Args... args)
    {
        return compress(zip, concat_s(args...));
    }
    int compress(std::string zip, std::vector<std::string> files)
    {
        std::string f;
        for(auto i : files)
        {
            f += i + " ";
        }
        return compress(zip, f);
    }
    int compress(const std::string& zip, const std::string& files)
    {
        using boost::format;
        auto cmd = str(format("7z a %1% %2%") % zip % files );
        return call_main(main_7z, cmd);
    }
    int uncompress(const std::string &zip, const std::string &to)
    {
        using boost::format;
        auto ft = file_type(zip);
        std::string cmd;
        std::size_t found = ft.find("/zip");

        if (found != std::string::npos)
        {
            cmd = str(format("7z x -aoa -o%1% %2%") % to % zip );
            return call_main(main_7z, cmd);
        }
        found = ft.find("/x-7z");
        if (found != std::string::npos)
        {
            cmd = str(format("7z x -aoa -o%1% %2%") % to % zip );
            return call_main(main_7z, cmd);
        }
        found = ft.find("/x-rar");
        if (found != std::string::npos)
        {
            cmd = str(format("unrar x -o+ %1% %2%") % zip % to );
            return call_main(main_unrar, cmd);
        }
        return -1;
    }
    json::array file_info(fs::path p)
    {	
        json::array fi_array;
        if( !fs::exists(p) ) return fi_array;
        std::vector<fs::path> v;
        if( fs::is_directory(p) )
        {
            for (auto &&x : fs::directory_iterator(p))
                v.push_back(x.path());
        }
        else if( fs::is_regular_file(p) )
        {
            v.push_back(p);
        }
        else
        {
            return fi_array;
        }
        // leave client to sort
        // std::sort(v.begin(), v.end());
        for (auto &&x : v)
        {
            json::object fi;
            auto fn = x.filename().string();
            std::time_t t = fs::last_write_time(x.string());
            std::tm tm = *std::localtime(&t);
            std::ostringstream ss;
            ss << std::put_time(&tm, "%F %T");
            std::string time_str = ss.str();
            fi["name"] = fn;
            fi["time"] = time_str;
            fi["path"] = x.string();
            if( fs::is_directory(x) )
            {
                fi["type"] = "dir";
                fi_array.emplace_back(fi);
                continue;
            }
            auto type = file_type( x.string() );
            fi["type"] = (!type.empty() ? type : "UNKOWN");
            fi["size"] = fs::file_size(x);
            fi["ext"] = x.extension().string();
            fi_array.emplace_back(fi);
        }
        // std::cout<< "fi_array=" << json::serialize(fi_array) << std::endl;
        return fi_array;
    }
};