#include "stdafx.h"

#include "OutputWpad.h"

void SaveWpadDat(
        __in IPAddressMap const & f_cache,
        __in_z char const * const f_szFileName,
        __in_z_opt char const * const proxyUrl )
{
    std::cout << "Saving wpad.dat file to " << f_szFileName << "..." << std::endl;

    std::ofstream foutf( f_szFileName );
    if( foutf.bad() )
    {
        // some error, abort
        return;
    }
    
    std::string proxy = "DIRECT";
    if( proxyUrl && *proxyUrl)
    {
        proxy = "PROXY ";
        proxy += proxyUrl;
    }

    DomainNames const setDomainNames( ExtractUniqueDomainNames( f_cache ) );
    DomainNames::const_iterator const itEndDomain( setDomainNames.end() );

    foutf << "//////////////////////////////////////////////////////////////////////////////" << std::endl
          << "//" << std::endl
          << "// Hosts file Based proxy auto configuration script" << std::endl
          << "//" << std::endl
          << "//////////////////////////////////////////////////////////////////////////////" << std::endl
          << std::endl;

    foutf << "function IsHostOrDomain(host, candidate)" << std::endl
          << "{" << std::endl
          << "  return shExpMatch(host, \"*.\" + candidate) || dnsDomainIs(host, candidate);" << std::endl
          << "}" << std::endl
          << std::endl;
    
    
    foutf << "function FindProxyForURL(url, host)" << std::endl
          << "{" << std::endl
          << std::endl  
          << "  // WARNING: all shExpMatch rules following MUST be lowercase!" << std::endl
          << "  url = url.toLowerCase();" << std::endl
          << "  host = host.toLowerCase();" << std::endl
          << std::endl;

    // Local network
    foutf << "  if( isInNet(host, \"10.0.0.0\", \"255.0.0.0\")" << std::endl
          << "   || isInNet(host, \"127.0.0.1\", \"255.255.255.255\")" << std::endl
		  << "   || isInNet(host, \"172.16.0.0\", \"255.240.0.0\")" << std::endl
		  << "   || isInNet(host, \"192.168.0.0\", \"255.255.0.0\"))"
          << "  {" << std::endl
          << "    return \"DIRECT\";" << std::endl
          << "  }" << std::endl
          << std::endl;

    // Banned stuff: Hard coded networks
    foutf << "  if(isInNet(host, \"205.180.85.0\", \"255.255.255.0\")) return \"PROXY 0.0.0.0:3421\";" << std::endl // one whole class C full of ad servers (fastclick)
          << "  if(isInNet(host, \"67.215.65.130\", \"255.255.255.255\")) return \"PROXY 0.0.0.0:3421\";" << std::endl; // hit-adult.opendns.com

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

        foutf << "  if(IsHostOrDomain(host, \"" << domain << "\")) return \"PROXY 0.0.0.0:3421\";" << std::endl;
    }
    foutf << std::endl;
    
    // FTP and HTTP should be proxied    
    foutf << "  if((url.substring(0, 5) == \"http:\")" << std::endl
          << "   ||(url.substring(0, 4) == \"ftp:\")" << std::endl
          << "  )" << std::endl
          << "  {" << std::endl
          << "    return \"" << proxy << "\";" << std::endl
          << "  }" << std::endl
          << std::endl;

    // Everything else direct
    foutf << "  return \"DIRECT\";" << std::endl
          << std::endl
          << "}" << std::endl
          << std::endl;
}

