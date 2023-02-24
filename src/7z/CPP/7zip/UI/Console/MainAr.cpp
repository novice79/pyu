// MainAr.cpp

#include "StdAfx.h"

#include "../../../Common/MyException.h"
#include "../../../Common/StdOutStream.h"

#include "../../../Windows/ErrorMsg.h"
#include "../../../Windows/NtCheck.h"

#include "../Common/ArchiveCommandLine.h"
#include "../Common/ExitCode.h"

#include "ConsoleClose.h"

using namespace NWindows;

CStdOutStream *g_StdStream = NULL;
CStdOutStream *g_ErrStream = NULL;

extern int Main2( int numArgs, char *args[] );

static const char *kException_CmdLine_Error_Message = "Command Line Error:";
static const char *kExceptionErrorMessage = "ERROR:";
static const char *kUserBreakMessage  = "Break signaled";
static const char *kMemoryExceptionMessage = "ERROR: Can't allocate required memory!";
static const char *kUnknownExceptionMessage = "Unknown Error";
static const char *kInternalExceptionMessage = "\n\nInternal Error #";

static void FlushStreams()
{
  if (g_StdStream)
    g_StdStream->Flush();
}

static void PrintError(const char *message)
{
  FlushStreams();
  if (g_ErrStream)
    *g_ErrStream << "\n\n" << message << endl;
}
#define ARGV_LEN_MAX 256
#define ARGC_MAX 32
static bool str2args(const char* s,char argv[][ARGV_LEN_MAX],int* argc) {
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
                PrintError("Parse Error! Bad quotes\n");
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
        PrintError("Parse Error! Still in container\n");
        ret = false;
    }
    if (escaped) {
        PrintError("Parse Error! Unused escape (\\)\n");
        ret = false;
    }
    return ret;
}

#define NT_CHECK_FAIL_ACTION *g_StdStream << "Unsupported Windows version"; return NExitCode::kFatalError;

int main_7z( int numArgs, char *args[])
// int MY_CDECL main( int numArgs, char *args[])
{
  g_ErrStream = &g_StdErr;
  g_StdStream = &g_StdOut;

  NT_CHECK

  NConsoleClose::CCtrlHandlerSetter ctrlHandlerSetter;
  int res = 0;
  
  try
  {
    res = Main2( numArgs, args );
  }
  catch(const CNewException &)
  {
    PrintError(kMemoryExceptionMessage);
    return (NExitCode::kMemoryError);
  }
  catch(const NConsoleClose::CCtrlBreakException &)
  {
    PrintError(kUserBreakMessage);
    return (NExitCode::kUserBreak);
  }
  catch(const CArcCmdLineException &e)
  {
    PrintError(kException_CmdLine_Error_Message);
    if (g_ErrStream)
      *g_ErrStream << e << endl;
    return (NExitCode::kUserError);
  }
  catch(const CSystemException &systemError)
  {
    if (systemError.ErrorCode == E_OUTOFMEMORY)
    {
      PrintError(kMemoryExceptionMessage);
      return (NExitCode::kMemoryError);
    }
    if (systemError.ErrorCode == E_ABORT)
    {
      PrintError(kUserBreakMessage);
      return (NExitCode::kUserBreak);
    }
    if (g_ErrStream)
    {
      PrintError("System ERROR:");
      *g_ErrStream << NError::MyFormatMessage(systemError.ErrorCode) << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(NExitCode::EEnum &exitCode)
  {
    FlushStreams();
    if (g_ErrStream)
      *g_ErrStream << kInternalExceptionMessage << exitCode << endl;
    return (exitCode);
  }
  catch(const UString &s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const AString &s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const char *s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const wchar_t *s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(int t)
  {
    if (g_ErrStream)
    {
      FlushStreams();
      *g_ErrStream << kInternalExceptionMessage << t << endl;
      return (NExitCode::kFatalError);
    }
  }
  catch(...)
  {
    PrintError(kUnknownExceptionMessage);
    return (NExitCode::kFatalError);
  }

  return res;
}
namespace pyu
{
int cmd_to_7z(const char* cmd)
{
  int ret = -1;
  int argc = 0;
    char temp[ARGC_MAX][ARGV_LEN_MAX] = {0};
    char* argv[ARGC_MAX] = {0};

    if (str2args(cmd, temp, &argc)==false) {
        return 7;
    }
    for (int i=0;i<argc;i++) {
        argv[i] = temp[i];
        printf("arg[%d]:[%s] ", i, argv[i]);
    }
    ret = main_7z(argc, (char**)argv);
    return ret;
}
int api_7z_extract(const char* in)
{
  char buff[512] = {0};
  sprintf(buff, "dummy x -y %s", in );
  return cmd_to_7z( buff );
}
int api_7z_extract(const char* in, const char* out)
{
  char buff[512] = {0};
  sprintf(buff, "dummy x -y %s -o%s", in, out );
  return cmd_to_7z( buff );
}
}
// int main(int argc, char *argv[])
// {
//     return main_7z( argc, argv );
// }