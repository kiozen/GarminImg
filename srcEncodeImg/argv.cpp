#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

char* get_argv(const int index, char** argv)
{
    char* result = NULL;
    int len;

#ifdef WIN32
    int numargs;
    wchar_t** argw = CommandLineToArgvW(GetCommandLineW(), &numargs);

    // determine the buffer length first (including the trailing null)
    len = WideCharToMultiByte(CP_UTF8, 0, argw[index], -1, NULL, 0, NULL, NULL);
    result = (char*)calloc(len, 1);
    WideCharToMultiByte(CP_UTF8, 0, argw[index], -1, result, len, NULL, NULL);

    GlobalFree(argw);
#else
    len = strlen(argv[index]) + 1;
    result = (char*)calloc(len, 1);
    strcpy(result, argv[index]);
#endif

    return result;
}
