#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CR
#undef NUMFONTS
#define kill(a,b) abort()
#define sleep(a) Sleep(a)

typedef int fd_t;
typedef DWORD pid_t;
extern void win_init();
