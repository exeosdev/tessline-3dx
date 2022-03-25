
#include "rxmlParser.h"

namespace ts3
{

	RxmlData RxmlParser::parseXMLContent( std::string pXMLContent )
	{
		RxmlData rxmlData;

		try
		{
			auto rxmlDocument = std::make_unique<RxmlDocument>();
			rxmlDocument->parse<0>( pXMLContent.data() );

			if( !rxmlDocument->first_node() )
			{
				return nullptr;
			}

			rxmlData.document = std::move( rxmlDocument );
			rxmlData.rootNode = rxmlData.document->first_node();
			rxmlData.textContent = std::move( pXMLContent );

			return rxmlData;
		}
		catch( const rapidxml::parse_error & pException )
		{
			( pException );
			ts3DebugInterrupt();
		}
		catch( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

}
