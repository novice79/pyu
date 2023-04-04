/* This is a simple HTTP(S) web server much like Python's SimpleHTTPServer */

#include <pyu/pyu.h>


using namespace std;

int main(int argc, char **argv) 
{
    auto l = pyu::create_logger("log", "aaa");
    l->trace("hello world: %1%\n", "david");
    l->debug("你好，世界😀\n");
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
    auto txt = "http://127.0.0.1:8888";
    pyu::QrToPng("aaa.png", txt).writeToPng();
	std::cout << "write " << txt << " to aaa.png file" << std::endl;

    auto f = pyu::create_fm( (pyu::exe_path(argv[0]) / "magic.mgc").string() );
    f->file_info( pyu::exe_path(argv[0]) );
    f->file_info( pyu::exe_path(argv[0]) / "magic.mgc" );
    if(argc < 2) return 0;
    if( 2 == argc)
    {
        // uncompress test
        printf("argv[1]=%s\n", argv[1]);
        cout << f->file_type(argv[1]) << endl;
        if( 0 == f->uncompress(argv[1], ".") )
        {
            l->debug("uncompress [%1%] succeed\n", argv[1] );
        }
        else
        {
            l->warn("uncompress [%1%] failed\n", argv[1] );
        }
    }
    else 
    {
        // compress files to 7z test
        std::vector<std::string> vs;
        for(int i = 2; i < argc; ++i)
        {
            vs.push_back(argv[i]);
        }
        if( 0 == f->compress(argv[1], vs) )
        {
            l->debug("compress [%1%] succeed\n", argv[1] );
        }
        else
        {
            l->warn("compress [%1%] failed\n", argv[1] );
        }
    }
    

    return 0;
}
