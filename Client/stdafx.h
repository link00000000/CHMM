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

#define DEV_ID 1;

#include <string>
#include <iostream>
#include <windows.h>
#include <winsock2.h>

#include "../Server/chmm.h"

#pragma comment(lib, "Ws2_32.lib")