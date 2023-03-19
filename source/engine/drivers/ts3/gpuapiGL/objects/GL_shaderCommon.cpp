
#include "GL_shaderCommon.h"
#include <ts3/stdext/stringUtils.h>
#include <regex>

namespace ts3::gpuapi
{
	namespace rcutil
	{

		bool processGLShaderSourceExplicitLayoutQualifiers(
				const Version & pSupportedAPIVersion,
				std::string & pShaderSource,
				GLShaderDataLayoutMap & pOutputLayoutMap )
		{
			static const std::string cvReStrBaseLayoutBinding{ "layout\\s*\\([\\s\\,\\=a-zA-Z0-9]*binding\\s*\\=\\s*([0-9]+)[\\s\\,\\=a-zA-Z0-9]*\\)" };
			static const std::string cvReStrBaseLayoutLocation{ "layout\\s*\\([\\s\\,\\=a-zA-Z0-9]+location\\s*\\=\\s*([0-9]+)[\\s\\,\\=a-zA-Z0-9]*\\)" };
			static const std::string cvReStrBaseInputAttribute{ "in\\s+[a-zA-Z0-9]+\\s*([a-zA-Z0-9]+)" };
			static const std::string cvReStrBaseOutputVariable{ "out\\s+[a-zA-Z0-9]+\\s*([a-zA-Z0-9]+)" };
			static const std::string cvReStrBaseUniformBlock{ "uniform\\s+([a-zA-Z0-9]+)" };
			static const std::string cvReStrBaseUniformSampler{ "uniform\\s+sampler[a-zA-Z0-9]+\\s+([a-zA-Z0-9]+)" };

			static const std::string cvReStrFullInputAttribute{ cvReStrBaseLayoutLocation + "\\s*" + cvReStrBaseInputAttribute + "\\s*;" };
			static const std::string cvReStrFullOutputVariable{ cvReStrBaseLayoutLocation + "\\s*" + cvReStrBaseOutputVariable + "\\s*;" };
			static const std::string cvReStrFullUniformBlock{ cvReStrBaseLayoutBinding + "\\s*" + cvReStrBaseUniformBlock };
			static const std::string cvReStrFullUniformSampler{ cvReStrBaseLayoutBinding + "\\s*" + cvReStrBaseUniformSampler };

			static const std::regex cvRegexBaseLayoutBinding{ cvReStrBaseLayoutBinding };
			static const std::regex cvRegexBaseLayoutLocation{ cvReStrBaseLayoutLocation };

			static const std::regex cvRegexFullInputAttribute{ cvReStrFullInputAttribute };
			static const std::regex cvRegexFullOutputVariable{ cvReStrFullOutputVariable };
			static const std::regex cvRegexFullUniformBlock{ cvReStrFullUniformBlock };
			static const std::regex cvRegexFullUniformSampler{ cvReStrFullUniformSampler };

			static const std::regex cvRegexUtilLayout{ "layout" };
			static const std::regex cvRegexUtilVersion{ "#version\\s+[3-4][0-6]0" };
			static const std::regex cvRegexUtilLeadingSpacesIn{ "^\\s*in" };
			static const std::regex cvRegexUtilLeadingSpacesOut{ "^\\s*out" };
			static const std::regex cvRegexUtilLeadingSpacesUniform{ "^\\s*uniform" };

			using StrLines = std::vector<std::string>;
			auto sourceLines = strUtils::splitStringEx<StrLines>( pShaderSource, '\n', []( auto & pStrLines, auto * pStr, auto pLength ) {
				pStrLines.push_back( std::string( pStr, pLength ) );
			} );

			uint32 layouts = 0;

			for( auto & sourceLine : sourceLines )
			{
				std::smatch regexMatch;
				if( std::regex_search( sourceLine, regexMatch, cvRegexUtilLayout ) )
				{
					if( std::regex_search( sourceLine, regexMatch, cvRegexFullInputAttribute ) )
					{
						const auto & debugView = regexMatch[0].str();
						const auto & attributeLocation = regexMatch[1].str();
						const auto & attributeName = regexMatch[2].str();
						pOutputLayoutMap.attributeLocations[attributeName] = fromString<uint32>( attributeLocation ).first;
						sourceLine = std::regex_replace( sourceLine, cvRegexBaseLayoutLocation, "" );
						sourceLine = std::regex_replace( sourceLine, cvRegexUtilLeadingSpacesIn, "in" );
						continue;
					}
					if( std::regex_search( sourceLine, regexMatch, cvRegexFullOutputVariable ) )
					{
						const auto & debugView = regexMatch[0].str();
						const auto & outputVarLocation = regexMatch[1].str();
						const auto & outputVarName = regexMatch[2].str();
						pOutputLayoutMap.fragDataLocations[outputVarName] = fromString<uint32>( outputVarLocation ).first;
						sourceLine = std::regex_replace( sourceLine, cvRegexBaseLayoutLocation, "" );
						sourceLine = std::regex_replace( sourceLine, cvRegexUtilLeadingSpacesOut, "out" );
						continue;
					}
					if( std::regex_search( sourceLine, regexMatch, cvRegexFullUniformSampler ) )
					{
						const auto & debugView = regexMatch[0].str();
						const auto & samplerBinding = regexMatch[1].str();
						const auto & samplerName = regexMatch[2].str();
						pOutputLayoutMap.samplerBindings[samplerName] = fromString<uint32>( samplerBinding ).first;
						sourceLine = std::regex_replace( sourceLine, cvRegexBaseLayoutBinding, "" );
						sourceLine = std::regex_replace( sourceLine, cvRegexUtilLeadingSpacesUniform, "uniform" );
						continue;
					}
					if( std::regex_search( sourceLine, regexMatch, cvRegexFullUniformBlock ) )
					{
						const auto & debugView = regexMatch[0].str();
						const auto & uniformBlockBinding = regexMatch[1].str();
						const auto & uniformBlockName = regexMatch[2].str();
						pOutputLayoutMap.uniformBlockBindings[uniformBlockName] = fromString<uint32>( uniformBlockBinding ).first;
						sourceLine = std::regex_replace( sourceLine, cvRegexBaseLayoutBinding, "" );
						sourceLine = std::regex_replace( sourceLine, cvRegexUtilLeadingSpacesUniform, "uniform" );
						continue;
					}

					++layouts;
				}
			}

			pShaderSource.clear();
			for( auto & sourceLine : sourceLines )
			{
				pShaderSource.append( sourceLine );
				pShaderSource.append( 1, '\n' );
			}

			std::string versionStr{ "#version " };
			versionStr.append( toString<char>( pSupportedAPIVersion.major ) );
			versionStr.append( toString<char>( pSupportedAPIVersion.minor ) );
			versionStr.append( 1, '0' );
			pShaderSource = std::regex_replace( pShaderSource, cvRegexUtilVersion, versionStr );

			return !pOutputLayoutMap.empty();
		}

	}
}
