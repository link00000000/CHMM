// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define MAX_CONNECTIONS 4

#include <string>
#include <time.h>
#include <iostream>
#include <windows.h>
#include <winsock.h>

#include "chmm.h"

#pragma comment(lib, "Ws2_32.lib")