// HostsMergeCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "PackedIPAddress.h"
#include "LoadHostsFile.h"
#include "OutputHostsFile.h"
#include "OutputWpad.h"


// URLS to download hosts files?
// http://www.jgilmore.com/AdBin/hosts.txt
// http://www.mvps.org/winhelp2002/hosts.txt
// http://hosts-file.malwareteks.com/hosts.txt

namespace
{
    static char const * const g_szHostsFiles[] =
    {
        "http://www.jgilmore.com/AdBin/hosts.txt",
        "http://www.mvps.org/winhelp2002/hosts.txt",
        "http://someonewhocares.org/hosts/hosts",
		"http://pgl.yoyo.org/adservers/serverlist.php?showintro=0;hostformat=hosts",

		"http://hosts-file.net/download/hosts.txt",
		"http://www.malwaredomainlist.com/hostslist/hosts.txt",

		//"http://support.it-mate.co.uk/downloads/hosts.txt",

        //"http://hosts-file.malwareteks.com/hosts.txt",
        // "http://everythingisnt.com/hosts",

        

		// not a hosts file - just domain per line
		// http://mirror1.malwaredomains.com/files/immortal_domains.txt
		
    };

    static char const * const * const g_szHostsFilesEnd = g_szHostsFiles + ( sizeof( g_szHostsFiles ) / sizeof( g_szHostsFiles[ 0 ] ) );

    typedef std::unordered_set< std::string > FilesToLoad;

    // Work out what to do with includes

    

    void LoadFile(
        __inout IPAddressMap & f_cache,
        __in char const * const f_szFileName )
    {
        IPAddressMap::size_type const
            nOriginalSize( f_cache.size() );

        size_t const
            nItemsInFile( LoadCache( f_cache, f_szFileName ) );

        std::cout << "   -> Loaded " << nItemsInFile << " entries." << std::endl;

        IPAddressMap::size_type const
            nNewSize( f_cache.size() );

        std::cout << "   -> Added " << ( nNewSize - nOriginalSize ) << " entries to list." << std::endl;
    }

    void LoadHostsFiles(
        __in FilesToLoad const & f_setFiles,
        __inout IPAddressMap & f_cache )
    {
        std::cout << "Loading hosts files:" << std::endl;

        FilesToLoad::const_iterator const
            itEndFile( f_setFiles.end() );

        for( FilesToLoad::const_iterator itFile( f_setFiles.begin() );
            itEndFile != itFile;
            ++itFile )
        {
            std::cout << " * Loading " << *itFile << "..." << std::endl;

            LoadFile( f_cache, itFile->c_str() );
        }
    }

    void LoadExclusions(
        __inout IPAddressMap & f_cache,
        __in char const * const f_szFileName )
    {
        IPAddressMap::size_type const
            nOriginalSize( f_cache.size() );

        IPAddressMap
            cache;
        size_t const
            nItemsInFile( LoadCache( cache, f_szFileName ) );

        std::cout << "   -> Loaded " << nItemsInFile << " exclusion entries." << std::endl;

        IPAddressMap::const_iterator
            itRemoveEnd( cache.end() );
        for( IPAddressMap::const_iterator itEntryToRemove( cache.begin() );
            itRemoveEnd != itEntryToRemove;
            ++itEntryToRemove )
        {
            RemoveMatchingEntries( f_cache, itEntryToRemove->second );
        }

        IPAddressMap::size_type const
            nNewSize( f_cache.size() );

        std::cout << "   -> Removed " << ( nOriginalSize - nNewSize ) << " entries in list." << std::endl;
    }

    void LoadWebHosts(
        __in FilesToLoad const & f_setURLs,
        __inout IPAddressMap & f_cache )
    {
        char const
            szCacheFile[] = "hosts.download";

        FilesToLoad::const_iterator const
            itURLEnd( f_setURLs.end() );

        for( FilesToLoad::const_iterator itURL( f_setURLs.begin() );
            itURLEnd != itURL;
            ++itURL )
        {
            std::cout << " * Loading " << *itURL << "..." << std::endl;

            std::ostringstream
                ss;
            ss << "wget --output-document=" << szCacheFile << " " << *itURL;
            if( 0 == system( ss.str().c_str() ) )
            {
                LoadFile( f_cache, szCacheFile );
            }
        }
    }
    
    void AddFile(
        __in FilesToLoad & f_setFiles,
        __in_z char const * const f_szFileName )
    {
        (void)f_setFiles.insert( f_szFileName );
    }

    void AddStandardFiles(
        __in FilesToLoad & f_setFiles )
    {
        
        // Add the std files
#if defined(_MSC_VER)
        AddFile( f_setFiles, "c:\\windows\\system32\\drivers\\etc\\hosts" );
#else
        AddFile( f_setFiles, "/etc/hosts" );
#endif
        AddFile( f_setFiles, "hosts" );
    }

    void AddStandardURLs(
        __in FilesToLoad & f_setURLs )
    {
        // Add the URLS
        for( char const * const * pItem = g_szHostsFiles;
            g_szHostsFilesEnd != pItem;
            ++pItem )   
        {
            (void)f_setURLs.insert( *pItem );
        }
    }
}


//lint -e952 -e818
int main(int argc, char* argv[])
{
    char const
        szHostsCacheFile[] = "hosts.cache";

    std::cout << "HostsMergeCmd" << std::endl
        << std::endl
        << "Usage:" << std::endl
        << "HostsMergeCmd" << std::endl
        << "HostsMergeCmd [hostfile] [...]" << std::endl
        << std::endl
        << "e.g.:" << std::endl
        << "HostsMergeCmd c:\\windows\\system32\\drivers\\etc\\hosts \"x:\\hosts\\hosts\" http://www.mvps.org/winhelp2002/hosts.txt" << std::endl
        << std::endl
        << "** Saves updates to " << szHostsCacheFile << std::endl
        << "** uses wget to download anything from the internet" << std::endl
        << std::endl;


    IPAddressMap
        allHosts;

    FilesToLoad setFiles;
    FilesToLoad setURLs;

    AddFile( setFiles, "hosts.bad");
    AddFile( setFiles, szHostsCacheFile );

    char const * proxyUrl = 0;

    for( int nFile( 1 ); nFile < argc; ++nFile )
    {
        if( ( argv[ nFile ] == strstr( argv[ nFile ], "http://" ) ) ||
            ( argv[ nFile ] == strstr( argv[ nFile ], "https://" ) ) ||
            ( argv[ nFile ] == strstr( argv[ nFile ], "ftp://" ) ) )
        {
            // Add the url from the command line
            (void)setURLs.insert( argv[ nFile ] );
        }
        else if( argv[ nFile ] == strstr( argv[ nFile ], "mtr://urls" ) )
        {
            AddStandardURLs( setURLs );
        }
        else if( argv[ nFile ] == strstr( argv[ nFile ], "mtr://files" ) )
        {
            AddStandardFiles( setFiles );
        }
        else if( argv[ nFile ] == strstr( argv[ nFile ], "mtr://all" ) )
        {
            AddStandardFiles( setFiles );
            AddStandardURLs( setURLs );
        }
        else if( argv[ nFile ] == strstr( argv[ nFile ], "-" ) )
        {
            if( proxyUrl)
            {
                std::cout << "Proxy URL Specified multiple times." << std::endl;
                return EXIT_FAILURE;
            }
            
            proxyUrl = (argv[ nFile ]) + 1;
        }
        else
        {
            // Add the file from the command line
            AddFile( setFiles, argv[ nFile ] );
        }
    }

    LoadHostsFiles( setFiles, allHosts );

    LoadWebHosts( setURLs, allHosts );

    LoadExclusions( allHosts, "hosts.exclude");

    std::cout << "Total Entries : " << allHosts.size() << std::endl;

    OptimizeHosts( allHosts );

    std::cout << "Total (optimized) Entries : " << allHosts.size() << std::endl;

    SaveCache( allHosts, szHostsCacheFile );

    std::cout << "Completed." << std::endl
        << "Verify the contents of " << szHostsCacheFile << " before overwriting your etc/hosts file." << std::endl;

    SaveWpadDat( allHosts, "wpad.dat", proxyUrl );

    return EXIT_SUCCESS;
}

