/* This is a simple HTTP(S) web server much like Python's SimpleHTTPServer */

#include <pyu/pyu.h>
#include <thread>

using namespace std;
void test_debounce()
{
    int x = 0;
    // if use auto, can not deduction
    std::function<int(int)> f = [&x](int i){
        return x += i;
    };
    // debounce 500 milliseconds
    auto dbf = pyu::debounce(f, 500);
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        dbf(1) ? cout<< "hit " : cout<< "skip ";
    }
    cout << endl;
    cout<< "x=" << x << endl;
}
int main(int argc, char **argv) 
{
    test_debounce();
    auto exe_path{pyu::exe_path(argv[0])};
    auto l = pyu::create_logger( exe_path / "log", "aaa");
    l->trace("hello world: %1%\n", "david");
    l->debug("你好，世界😀\n");
    l->info("hello world\n");
    l->warn("hello world\n");
    l->error("hello world\n");
    l->fatal("hello world\n");

    auto db = pyu::create_db(exe_path / "py.db");
    l->debug("db pass=%1%\n", db->get_pass() );
    l->warn( json::serialize( db->exec_sql("select * from user;") ) );
    sol::state lua;
	// open some common libraries
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('lua bark bark bark!')");
    auto txt = "http://127.0.0.1:8888";
    pyu::QrToPng(exe_path / "aaa.png", txt).writeToPng();
	std::cout << "write QR code of '" << txt << "' to " << exe_path / "aaa.png" << std::endl;

    auto f = pyu::create_fm( exe_path / "magic.mgc" );
    cout<< json::serialize(f->file_info( exe_path ));
    cout<< json::serialize(f->file_info( exe_path / "magic.mgc" ) ) << endl;;
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
