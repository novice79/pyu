#include "commom.h"
#include <boost/program_options/parsers.hpp>


int pyu::call_main(MainFunc m, std::string args)
{
    auto parts = boost::program_options::split_unix(args);
    std::vector<char*> cstrings ;
    for(auto& str : parts)
    {
        cstrings.push_back(const_cast<char*> (str.c_str()));
    }

    int argc = (int)cstrings.size();
    char** argv = cstrings.data();
    return m(argc, argv);
}
int call_main_1(MainFunc m, std::string args)
{
    std::istringstream ss(args);
    std::string arg;
    std::list<std::string> ls;
    std::vector<char*> v;
    while (ss >> arg)
    {
        ls.push_back(arg); 
        v.push_back(const_cast<char*>(ls.back().c_str()));
    }
    v.push_back(0);
    return m(ls.size(), &v[0]);
}
int call_main_2(MainFunc m, std::string args)
{
  int ret = -1;
  int argc = 0;
    char temp[ARGC_MAX][ARGV_LEN_MAX] = {0};
    char* argv[ARGC_MAX] = {0};

    if (str2args(args.c_str(), temp, &argc)==false) {
        return ret;
    }
    for (int i=0;i<argc;i++) {
        argv[i] = temp[i];
        printf("arg[%d]:[%s] ", i, argv[i]);
    }
    ret = m(argc, (char**)argv);
    return ret;
}
bool str2args(const char* s,char argv[][ARGV_LEN_MAX],int* argc) 
{
    bool in_token, in_container, escaped;
    bool ret;
    char container_start, c;
    int len, i;
    int index = 0;
    int arg_count = 0;

    ret = true;
    container_start = 0;
    in_token = false;
    in_container = false;
    escaped = false;

    len = strlen(s);
    for (i = 0; i < len; i++) {
        c = s[i];
        switch (c) {
            case ' ':
            case '\t':
            case '\n':

                if (!in_token)
                    continue;
                if (in_container) {
                    argv[arg_count][index++] = c;
                    continue;
                }
                if (escaped) {
                    escaped = false;
                    argv[arg_count][index++] = c;
                    continue;
                }
                /* if reached here, we're at end of token */
                in_token = false;
                argv[arg_count++][index] = '\0';
                index = 0;
                break;
                /* handle quotes */
            case '\'':
            case '\"':

                if (escaped) {
                    argv[arg_count][index++] = c;
                    escaped = false;
                    continue;
                }
                if (!in_token) {
                    in_token = true;
                    in_container = true;
                    container_start = c;
                    continue;
                }
                if (in_container) {
                    if (c == container_start) { //container end
                        in_container = false;
                        in_token = false;
                        argv[arg_count++][index] = '\0';
                        index = 0;
                        continue;
                    } else { //not the same as contain start char
                        argv[arg_count][index++] = c;
                        continue;
                    }
                }
                printf("Parse Error! Bad quotes\n");
                ret = false;
                break;
            case '\\':

                if (in_container && s[i + 1] != container_start) {
                    argv[arg_count][index++] = c;
                    continue;
                }
                if (escaped) {
                    argv[arg_count][index++] = c;
                    continue;
                }
                escaped = true;
                break;
            default: //normal char

                if (!in_token) {
                    in_token = true;
                }
                argv[arg_count][index++] = c;
                if (i == len - 1) { //reach the end
                    argv[arg_count++][index++] = '\0';
                }
                break;
        }
    }
    *argc = arg_count;
    if (in_container) {
        printf("Parse Error! Still in container\n");
        ret = false;
    }
    if (escaped) {
        printf("Parse Error! Unused escape (\\)\n");
        ret = false;
    }
    return ret;
}
