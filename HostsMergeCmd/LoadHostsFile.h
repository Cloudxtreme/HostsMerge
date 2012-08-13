#pragma once

#include "Hosts.h"


__checkReturn size_t LoadCache(
                               __inout IPAddressMap & f_cache,
                               __in_z char const * const f_szFileName );


