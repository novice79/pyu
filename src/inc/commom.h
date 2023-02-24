#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sstream>

typedef int MainFunc(int argc, char *argv[]);
#define ARGV_LEN_MAX 256
#define ARGC_MAX 32
bool str2args(const char* s,char argv[][ARGV_LEN_MAX],int* argc);
namespace pyu
{
    int call_main(MainFunc m, std::string args);
}