#pragma once

#include "Hosts.h"

void SaveWpadDat(
        __in IPAddressMap const & f_cache,
        __in_z char const * const f_szFileName,
        __in_z_opt char const * const proxyUrl );


