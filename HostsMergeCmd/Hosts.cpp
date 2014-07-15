#include "stdafx.h"

#include "Hosts.h"

#include <assert.h>

namespace
{
	__checkReturn std::string reverse( __in std::string const & original )
	{
		return std::string(original.rbegin(), original.rend());
	}

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

		std::string sNameToRemove;
		sNameToRemove.reserve(f_nametoremove.size() + 1 );

		sNameToRemove.append(".");
		sNameToRemove.append(f_nametoremove);

		char const * const pEnd = f_existingname.c_str() + ( f_existingname.size() - sNameToRemove.size() );

		return AreEqualCaseInsensitive( pEnd, sNameToRemove.c_str() );
	}

	DomainNames OptimizeUniqueDomainNames( DomainNames const & f_original )
	{
		std::cout << "-- Finding Unique Domain Names: Start" << std::endl;

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

		std::cout << "-- Finding Unique Domain Names: End" << std::endl;

		return uniqueCleanedNames;
	}

	__checkReturn IPAddressMap::iterator RemoveItemAndMoveToNext( __in IPAddressMap & f_cache, __in IPAddressMap::iterator const & f_item )
	{
#if defined (UNORDERED_MAP_AVAILABLE)
		return f_cache.erase( f_item );
#else
		IPAddressMap::iterator next(f_item);
		++next;

		f_cache.erase( f_item );

		return next;
#endif
	}

	__checkReturn bool IsSubdomainOfBlockedEntry( __in std::string const & searchFor, __in DomainNames const & setDomainNames, __in DomainNames::const_iterator const & itEndDomain )
	{
		std::string const reverseSearchTerm( reverse( searchFor ) );

		DomainNames::const_iterator itLower(setDomainNames.lower_bound( reverseSearchTerm ));

		while( itLower != itEndDomain && ( *itLower == reverseSearchTerm || EndsWith( reverse( *itLower ), searchFor ) ) )
		{
			if( EndsWith( reverse( *itLower ), searchFor ) )
			{
				std::cout << "  ++ Removing " << searchFor << " as it matches " << reverse( *itLower ) << std::endl;
				return true;
			}

			++itLower;
		}

		return false;
	}

	template< typename TFunctor >
	__checkReturn DomainNames ExtractUniqueDomainNamesInternal(
		__in IPAddressMap const & f_cache,
		__in TFunctor process )
	{
		std::cout << "-- Extracting Unique Domain Names: Start" << std::endl;

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

			(void)setDomainNames.insert( process( itEntry->second ) ).second;
		}

		std::cout << "-- Extracting Unique Domain Names: End" << std::endl;

		return setDomainNames;
	}

	__checkReturn std::string noop( __in std::string s) 
	{
		return s;
	}

	__checkReturn DomainNames ExtractUniqueDomainNamesReverse(
	__in IPAddressMap const & f_cache )
	{
		return ExtractUniqueDomainNamesInternal( f_cache, reverse );
	}

}

__checkReturn DomainNames ExtractUniqueDomainNames(
	__in IPAddressMap const & f_cache )
{
	return ExtractUniqueDomainNamesInternal( f_cache, noop );
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

	DomainNames const setDomainNames( OptimizeUniqueDomainNames( ExtractUniqueDomainNamesReverse( f_cache ) ) );

	std::cout << "-- Removing subdomains: Start" << std::endl;
	DomainNames::const_iterator const itEndDomain(setDomainNames.end());


	IPAddressMap::iterator const itCacheEnd( f_cache.end() );

	IPAddressMap::iterator itCacheItem( f_cache.begin() );
	while( itCacheEnd != itCacheItem )
	{
		if( IsBlockedIp(itCacheItem->first) && IsSubdomainOfBlockedEntry( itCacheItem->second, setDomainNames, itEndDomain ) )
		{            
			++itemsRemoved;
			itCacheItem = RemoveItemAndMoveToNext( f_cache, itCacheItem );
		}
		else
		{
			++itCacheItem;
		}
	}

	for( DomainNames::const_iterator itDomain( setDomainNames.begin() );
		itEndDomain != itDomain;
		++itDomain)
	{


	}
	std::cout << "-- Removing subdomains: End" << std::endl;

	std::cout << "Optimizing Complete.  Removed " << itemsRemoved << " Entries." << std::endl;
}


//bool reverse_name_comparer::operator()( std::string const & lhs, std::string const & rhs)
//{
//	return reverse(lhs) < reverse(rhs);
//}
//
