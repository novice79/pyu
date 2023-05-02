#pragma once

#include "magic.h"
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
namespace json = boost::json;
namespace fs = boost::filesystem;


class FileMgr
{
    const std::string magic_path_;
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
    :magic_path_(magic.string())
    {}
    std::string file_type(const std::string &path)
    {
        thread_local static magic_t s_t_handle = NULL;
        thread_local static bool magic_loaded{false};
        if (!s_t_handle)
        {
            // MAGIC_MIME: A shorthand for MAGIC_MIME_TYPE | MAGIC_MIME_ENCODING.
            s_t_handle = ::magic_open(MAGIC_NONE | MAGIC_MIME);
            if (s_t_handle == NULL)
            {
                printf("initialize magic library failed\n");
                return "application/octet-stream";
            }
            if( ::magic_load( s_t_handle, magic_path_.c_str() ) != 0 )
            {
                printf("cannot load magic database -%s\n", magic_error(s_t_handle));
                magic_close(s_t_handle);
            }
            else
            {
                magic_loaded = true;
                printf("load magic database succeed!\n");
            }
        }
        if( !magic_loaded )
        {
            printf("Can not load magic file, determine file type failed\n");
            return "application/octet-stream";
        }
        return ::magic_file(s_t_handle, path.c_str());
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