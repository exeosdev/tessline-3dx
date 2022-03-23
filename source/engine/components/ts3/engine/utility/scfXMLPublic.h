
#pragma once

#ifndef __TS3_ENGINE_SCF_XML_PUBLIC_H__
#define __TS3_ENGINE_SCF_XML_PUBLIC_H__

#include "scfXMLCommon.h"
#include <ts3/system/fileCommon.h>

namespace ts3
{

	class SCFXMLReader
	{
	public:
		using RxDocument = SCFXMLRootNode::RxDocument;

		SCFXMLReader( system::FileManagerHandle pSysFileManager )
		: _sysFileManager( pSysFileManager )
		{}

		SCFXMLRootNode readFile( const std::string & pFilename );

	private:
		static SCFXMLRootNode _parseXMLContent( std::string pXMLContent );

		static void _readFolderSubNodes( SCFXMLFolderNode & pFolderNode );

		static void _readPropertySubNodes( SCFXMLPropertyNode & pPropertyNode );

	private:
		system::FileManagerHandle _sysFileManager;
	};

	class SCFXMLResourceDefinitionConverter
	{
	public:
		using ConversionCallback = std::function<bool( const SCFXMLResourceNode &, DynamicByteArray & )>;

		SCFXMLResourceDefinitionConverter() = default;

		SCFXMLResourceDefinitionConverter( SCFXMLResourceDefinitionConverter && ) = default;
		SCFXMLResourceDefinitionConverter & operator=( SCFXMLResourceDefinitionConverter && ) = default;

		SCFXMLResourceDefinitionConverter( const SCFXMLResourceDefinitionConverter & ) = default;
		SCFXMLResourceDefinitionConverter & operator=( const SCFXMLResourceDefinitionConverter & ) = default;

		/// @brief
		void registerCallback( SCFXMLResourceTypeID pResTypeID, ConversionCallback pImportCallback );

		/// @brief
		bool executeConversion( const SCFXMLResourceNode & pXMLResourceNode, DynamicByteArray & pOutputBuffer );

	private:
		using ImportCallbackMap = std::unordered_map<SCFXMLResourceTypeID, ConversionCallback>;
		ImportCallbackMap _importCallbackMap;
	};

}

#endif // __TS3_ENGINE_SCF_XML_PUBLIC_H__
