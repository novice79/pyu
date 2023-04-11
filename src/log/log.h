
#pragma once

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace fs = boost::filesystem;

class Log 
{
public:
    Log(fs::path log_path = ".", std::string name = "py");
    template<typename... Arguments>
    void trace(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::trace) << boost::str((boost::format(fmt) % ... % args));
    }
    template<typename... Arguments>
    void debug(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::debug) << boost::str((boost::format(fmt) % ... % args));
    }
    template<typename... Arguments>
    void info(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::info) << boost::str((boost::format(fmt) % ... % args));
    }
    template<typename... Arguments>
    void warn(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::warning) << boost::str((boost::format(fmt) % ... % args));
    }
    template<typename... Arguments>
    void error(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::error) << boost::str((boost::format(fmt) % ... % args));
    }
    template<typename... Arguments>
    void fatal(const std::string& fmt, const Arguments&... args)
    {
        BOOST_LOG_SEV(this->lg_, boost::log::trivial::fatal) << boost::str((boost::format(fmt) % ... % args));
    }
private:
    thread_local static boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg_;

}; // class end
