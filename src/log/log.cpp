#include "log.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

thread_local boost::log::sources::severity_logger< boost::log::trivial::severity_level > Log::lg_;

Log::Log(fs::path log_path, std::string name)
{
    if( !fs::exists(log_path) )
    {
        fs::create_directories(log_path);
    }
    auto prefix = (boost::format("%1%/%2%_") % log_path.string() % name).str();
    logging::add_file_log
    (
        keywords::file_name = prefix + "%Y-%m-%d.log",
        // 5MiB limit to rotate
        keywords::rotation_size = 5 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::auto_flush = true,
        keywords::format = "[%TimeStamp%]:<%Severity%> %Message%",
        keywords::max_files = 10
    );
    // add console sink
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    sink->locked_backend()->add_stream(
        boost::shared_ptr<std::ostream>(&std::cout, boost::null_deleter()) );

    // flush
    sink->locked_backend()->auto_flush(true);

    // format sink
    sink->set_formatter
    (
        expr::format("[%1%]: <%2%> %3%")
            % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            % logging::trivial::severity
            % expr::smessage
        // expr::stream
        //     << "["
        //     << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
        //     << "]: <" << logging::trivial::severity
        //     << "> " << expr::smessage
    );
    // register sink
    logging::core::get()->add_sink(sink);

	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::trace
	);
	logging::add_common_attributes();
}

