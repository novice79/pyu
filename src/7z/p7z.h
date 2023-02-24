#pragma once

namespace pyu
{
int cmd_to_7z(const char* cmd);
int api_7z_extract(const char* in);
int api_7z_extract(const char* in, const char* out);
}