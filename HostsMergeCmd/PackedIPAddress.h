#pragma once

typedef unsigned long PackedIP;

__checkReturn PackedIP PackIP(
                          __in char const ip1,
                          __in char const ip2,
                          __in char const ip3,
                          __in char const ip4 );

__checkReturn PackedIP BlockedIP();

__checkReturn bool IsBlockedIp( __in PackedIP const f_ip );

__checkReturn std::string UnpackedIP( __in PackedIP const f_ip );


