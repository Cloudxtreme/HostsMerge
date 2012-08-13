#include "stdafx.h"

#include "Hosts.h"

namespace
{
    __checkReturn bool AreEqualCaseInsensitive(
                            __in_z char const * const first,
                            __in_z char const * const second )
    {
        return 0 == _strcmpi( first, second );
    }
    
    __checkReturn bool EndsWith(
        __in std::string const & f_existingname,
        __in std::string const & f_nametoremove )
    {
        if( f_existingname.size() <= f_nametoremove.size() )
        {
            return false;
        }

        std::string sNameToRemove( "." );
        sNameToRemove += f_nametoremove;

        char const * const pEnd = f_existingname.c_str() + ( f_existingname.size() - sNameToRemove.size() );

        return AreEqualCaseInsensitive( pEnd, sNameToRemove.c_str() );
    }

    DomainNames OptimizeUniqueDomainNames( DomainNames const & f_original )
    {
        DomainNames uniqueCleanedNames;

        DomainNames::const_iterator const itEndDomain(f_original.end());

        for( DomainNames::const_iterator itDomain( f_original.begin() );
            itEndDomain != itDomain;
            ++itDomain)
        {
            std::string domain(*itDomain);

            // Starts with www: remove www.  Same for web.
            if( domain.find( "www.") == 0 )
            {
                domain.erase(0, 4);
            }
            else if( domain.find( "web.") == 0 )
            {
                domain.erase(0, 4);
            }

            (void)uniqueCleanedNames.insert( domain );
        }


        return uniqueCleanedNames;
    }
    
    __checkReturn IPAddressMap::iterator RemoveItemAndMoveToNext( __in IPAddressMap & f_cache, __in IPAddressMap::iterator const & f_item )
    {
#if defined(_MSC_VER)
        return f_cache.erase( f_item );
#else
        IPAddressMap::iterator next(f_item);
        ++next;

        f_cache.erase( f_item );

        return next;
#endif
    }
}

__checkReturn DomainNames ExtractUniqueDomainNames(
    __in IPAddressMap const & f_cache )
{
    DomainNames setDomainNames;
    IPAddressMap::const_iterator const itEnd( f_cache.end() );

    for( IPAddressMap::const_iterator itEntry( f_cache.begin() );
        itEnd != itEntry;
        ++itEntry )
    {
        if( !IsBlockedIp( itEntry->first ) )
        {
            continue;
        }

        (void)setDomainNames.insert( itEntry->second ).second;
    }

    return setDomainNames;
}

void RemoveMatchingEntries(
    __inout IPAddressMap & f_cache,
    __in std::string const & f_name )
{
    IPAddressMap::iterator const itEnd( f_cache.end() );

    IPAddressMap::iterator itItem( f_cache.begin() );

    while( itEnd != itItem )
    {
        if( AreEqualCaseInsensitive( itItem->second.c_str(), f_name.c_str() ) )
        {
            itItem = RemoveItemAndMoveToNext( f_cache, itItem );
        }
        else if( EndsWith( itItem->second, f_name ) )
        {
            itItem = RemoveItemAndMoveToNext( f_cache, itItem );
        }
        else
        {
            ++itItem;
        }
    }
}

void OptimizeHosts( __inout IPAddressMap & f_cache)
{
    std::cout << "Optimizing Entries...." << std::endl;

    size_t itemsRemoved(0);

    DomainNames const setDomainNames( OptimizeUniqueDomainNames( ExtractUniqueDomainNames( f_cache ) ) );
    DomainNames::const_iterator const itEndDomain(setDomainNames.end());

    for( DomainNames::const_iterator itDomain( setDomainNames.begin() );
        itEndDomain != itDomain;
        ++itDomain)
    {
        IPAddressMap::iterator const itEnd( f_cache.end() );

        IPAddressMap::iterator itItem( f_cache.begin() );
        while( itEnd != itItem )
        {
            if( IsBlockedIp(itItem->first) && EndsWith( itItem->second, *itDomain ) )
            {
                //std::cout << "Removing " << itItem->second << " as it matches " << *itDomain << std::endl;
                ++itemsRemoved;
                itItem = RemoveItemAndMoveToNext( f_cache, itItem );
            }
            else
            {
                ++itItem;
            }
        }
    }

    std::cout << "Optimizing Complete.  Removed " << itemsRemoved << " Entries." << std::endl;
}

std::string reverse(std::string const & original )
{
	return std::string(original.rbegin(), original.rend());
}

bool reverse_name_comparer::operator()( std::string const & lhs, std::string const & rhs)
{
	return reverse(lhs) < reverse(rhs);
}

