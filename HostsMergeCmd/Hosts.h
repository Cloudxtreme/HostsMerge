#pragma once

#include "PackedIPAddress.h"

//class reverse_name_comparer
//{
//public:
//	bool operator()( std::string const & lhs, std::string const & rhs);
//};

typedef std::unordered_multimap< PackedIP, std::string > IPAddressMap;

typedef std::set< std::string > DomainNames;

__checkReturn DomainNames ExtractUniqueDomainNames(
    __in IPAddressMap const & f_cache );

void RemoveMatchingEntries(
    __inout IPAddressMap & f_cache,
    __in std::string const & f_name );
    
void OptimizeHosts( __inout IPAddressMap & f_cache);

