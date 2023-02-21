/* This is a simple HTTP(S) web server much like Python's SimpleHTTPServer */

#include <pyu/pyu.h>

int main(int argc, char **argv) 
{
    auto l = pyu::create_logger("log", "aaa");
    l->trace("hello world: %1%\n", "david");
    l->debug("hello world\n");
    l->info("hello world\n");
    l->warn("hello world\n");
    l->error("hello world\n");
    l->fatal("hello world\n");

    auto db = pyu::create_db();
    l->debug("db pass=%1%\n", db->get_pass() );
    l->warn( db->exec_sql("select * from user;") );
    sol::state lua;
	// open some common libraries
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('lua bark bark bark!')");
    try
    {
        throw "hello world";
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception catched " << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr << "exception catched " << '\n';
    }
	std::cout << std::endl;
    return 0;
}
