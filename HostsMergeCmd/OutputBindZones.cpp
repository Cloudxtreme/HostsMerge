#include "stdafx.h"

#include "OutputBindZones.h"

void SaveBindZones(
        __in IPAddressMap const & f_cache,
        __in_z char const * const f_szFileName )
{
    std::cout << "Saving bind zones file to " << f_szFileName << "..." << std::endl;

    std::ofstream foutf( f_szFileName );
    if( foutf.bad() )
    {
        // some error, abort
        return;
    }
    
    DomainNames const setDomainNames( ExtractUniqueDomainNames( f_cache ) );
    DomainNames::const_iterator const itEndDomain( setDomainNames.end() );

    // Banned stuff: Domain names
    for( DomainNames::const_iterator itDomain( setDomainNames.begin() );
        itEndDomain != itDomain;
        ++itDomain )
    {
        std::string domain(*itDomain);
        
        // contains bad chars
        if( std::string::npos != domain.find('\"'))
        {
            continue;
        }
        
        // Too Short
        if(domain.size() < 3)
        {
            continue;
        }
        
        std::stringstream ss;

		foutf << "zone \"" << domain << "\"{ type master; notify no; file \"/etc/bind/null.zone.file\"; };" << std::endl;
    }
    foutf << std::endl;
}

