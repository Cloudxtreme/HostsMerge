#include "stdafx.h"

#include "PackedIPAddress.h"


namespace
{
    inline __checkReturn unsigned short MakeWord(
        __in unsigned char const a,
        __in unsigned char const b )
    {
        unsigned long const bitsToShiftBy = 8;
        unsigned short const mask = 0xff;
    
        return static_cast<unsigned short>(static_cast<unsigned char>(a & mask)) |
               static_cast<unsigned short>(static_cast<unsigned char>(b & mask) << bitsToShiftBy);
    }

    inline __checkReturn unsigned long MakeLong(
        __in unsigned short const a,
        __in unsigned short const b)
    {
        unsigned long const bitsToShiftBy = 16;
        unsigned long const mask = 0xffff;
        
        return static_cast<unsigned long>(static_cast<unsigned short>(a & mask)) |
                //lint -e701
               static_cast<unsigned long>(static_cast<unsigned short>(b & mask) << bitsToShiftBy);
    }

    inline __checkReturn unsigned char LowByte( __in unsigned short const w )
    {
        unsigned short const mask = 0xff;
        
        return static_cast<unsigned char>(w & mask);
    }

    inline __checkReturn unsigned char HighByte( __in unsigned short const w )
    {
        unsigned short const bitsToShiftBy = 8;
        unsigned short const mask = 0xff;
        
        return static_cast<unsigned char>((w >> bitsToShiftBy) & mask);
    }

    inline __checkReturn unsigned short LowWord( __in unsigned long const l )
    {
        unsigned long const mask = 0xffff;
        
        return static_cast<unsigned short>(l & mask);
    }

    inline __checkReturn unsigned short HighWord( __in unsigned long const l )
    {
        unsigned long const bitsToShiftBy = 16;
        unsigned long const mask = 0xffff;
    
        return static_cast<unsigned short >((l >> bitsToShiftBy) & mask);
    }

}

__checkReturn PackedIP PackIP(
                              __in char const ip1,
                              __in char const ip2,
                              __in char const ip3,
                              __in char const ip4 )
{
    return MakeLong(
        MakeWord( ip1, ip2 ),
        MakeWord( ip3, ip4 ) );
}

__checkReturn PackedIP BlockedIP()
{
    static PackedIP const ipZero( PackIP( 0, 0, 0, 0 ) );

    return ipZero;
}

__checkReturn bool IsBlockedIp( __in PackedIP const f_ip )
{
    static PackedIP const ipLocalhost( PackIP( 127, 0, 0, 1 ) );

    return ( ipLocalhost == f_ip ) ||
        ( BlockedIP() == f_ip );
}

__checkReturn std::string UnpackedIP( __in PackedIP const f_ip )
{
    std::stringstream ss;
    
    ss << static_cast<int>(LowByte( LowWord( f_ip ) )) << "."
       << static_cast<int>(HighByte( LowWord( f_ip ) )) << "."
       << static_cast<int>(LowByte( HighWord( f_ip ) )) << "."
       << static_cast<int>(HighByte( HighWord( f_ip ) ));

    return ss.str();
}


