#include "stdafx.h"

#include "LoadHostsFile.h"

namespace
{
#if !defined(_MSC_VER)

    char *strtok_s(
        char *strToken,
        const char *strDelimit,
        char **context
        )
    {
        return strtok( strToken, strDelimit );
    }
#endif
    FILE *OpenFile( char const * const filename)
    {
#if defined(_MSC_VER)
        FILE *finf = 0;
        errno_t const err( fopen_s( &finf, filename, "rt" ) );
        if( 0 != err )
        {
            // some error, abort
            return 0;
        }

        return finf;
#else
        return fopen( filename, "r");
#endif
    }

    bool ReadIP( __in_z char * line, int & ip1, int & ip2, int &ip3, int & ip4)
    {
        char ch;
#if defined(_MSC_VER)
        int const pos( sscanf_s(
            line,
            //127  .  0  .  0  .  1
            "%d %c %d %c %d %c %d",
            &ip1,
            &ch, sizeof(char),
            &ip2,
            &ch, sizeof(char),
            &ip3,
            &ch, sizeof(char),
            &ip4 ) );
#else
        int const pos( sscanf(
            line,
            //127  .  0  .  0  .  1
            "%d %c %d %c %d %c %d",
            &ip1,
            &ch,
            &ip2,
            &ch,
            &ip3,
            &ch,
            &ip4 ) );
#endif
        return 0 != pos;        
    }

    __checkReturn bool IsComment( __in_z char const * const f_szLine )
    {
        return ( ';' == *f_szLine ) ||
            ( '#' == *f_szLine ) ||
            ( '\n' == *f_szLine ) ||
            ( '\r' == *f_szLine );
    }

    __checkReturn char MakeLowerCase( __in char const f_ch )
    {
        return static_cast< char >( tolower( f_ch ) & 0xff );
    }


    __checkReturn char IsInvalidCharacter( __in char const f_ch )
    {
		return ( f_ch < ' ' ) || ( f_ch == '#' );
    }

    __checkReturn std::string CleanHostName( __in_z char const * const f_szHostName )
    {
        std::string const sHostName( f_szHostName );

        std::string sConvertedHostName;

        std::string::const_iterator const itOriginalBegin( sHostName.begin() );
        std::string::const_iterator const itOriginalEnd( sHostName.end() );
                
        // convert to lowercase
        (void)std::transform(
            itOriginalBegin,
            itOriginalEnd,
            std::back_inserter(sConvertedHostName),
            MakeLowerCase );

        std::string::iterator const itBegin( sConvertedHostName.begin() );
        std::string::iterator const itEnd( sConvertedHostName.end() );

        (void)sConvertedHostName.erase(
            std::remove_if(
            itBegin,
            itEnd,
            IsInvalidCharacter ),
            itEnd );

		while(!sConvertedHostName.empty() && *sConvertedHostName.rbegin() == '.' )
		{
			sConvertedHostName.resize(sConvertedHostName.size() - 1);
		}

        return sConvertedHostName;
    }
}

__checkReturn size_t LoadCache(
                               __inout IPAddressMap & f_cache,
                               __in_z char const * const f_szFileName )
{
    FILE *finf = OpenFile( f_szFileName );
    if( !finf )
    {
        // some error, abort
        return 0;
    }

    size_t  nItems( 0 );
    size_t const knLineLength( 8192 );
    char szLine[ knLineLength + 1 ];
    while( fgets( szLine, knLineLength, finf ) )
    {
        if( IsComment( szLine ) )
        {
            continue;
        }
        
        int ip1;
        int ip2;
        int ip3;
        int ip4;
        if( !ReadIP( szLine, ip1, ip2, ip3, ip4 ))
        {
            continue;
        }
        
        char const szDelimiters[] = "\t ";
        char *next_token = 0;
        char const *pTok = strtok_s( szLine, szDelimiters, &next_token );
        if( pTok )
        {
            PackedIP const recordIP( PackIP( static_cast<char >(ip1 & 0xff), static_cast<char >(ip2 & 0xff), static_cast<char >(ip3 & 0xff), static_cast<char >(ip4 & 0xff) ) );

            bool const bIsBlockedIp( IsBlockedIp( recordIP ) );

            PackedIP const ip( bIsBlockedIp
                ? BlockedIP()
                : recordIP );

            for( pTok = strtok_s( 0, szDelimiters, &next_token );
                pTok;
                pTok = strtok_s( 0, szDelimiters, &next_token ) )
            {
                if( IsComment( pTok ) )
                {
                    break;
                }

                std::string const sHostName( CleanHostName( pTok ) );

                // skip blanks
                if( sHostName.empty() )
                {
                    continue;
                }

                // skip localhost
                if( sHostName == "localhost" )
                {
                    continue;
                }

                bool bFound( false );
                if( !bIsBlockedIp )
                {
                    // Perform search
                    IPAddressMap::const_iterator const itLower( f_cache.lower_bound( ip ) );
                    IPAddressMap::const_iterator const itUpper( f_cache.upper_bound( ip ) );

                    for( IPAddressMap::const_iterator itItem( itLower );
                        itUpper != itItem;
                        ++itItem )
                    {
                        if( itItem->second == sHostName )
                        {
                            bFound = true;
                            break;
                        }
                    }
                }
				else
				{
					// Blocked IP with no .
					if( std::string::npos == sHostName.find("."))
					{
						continue;
					}
				}


                if( !bFound )
                {
                    (void)f_cache.insert(
                        IPAddressMap::value_type( ip, sHostName ) );
                    ++nItems;
                }
            }
        }
    }

    fclose( finf );

    return nItems;
}

