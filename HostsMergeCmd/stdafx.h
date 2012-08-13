// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>

#if defined(_MSC_VER)

#include <unordered_map>
#include <unordered_set>
#include <set>

#else
#include <map>
#include <set>
#define unordered_map map
#define unordered_multimap multimap
#define unordered_set set


#define __checkReturn
#define __in
#define __inout
#define __in_z
#define __in_z_opt

#define _strcmpi strcasecmp

#endif

