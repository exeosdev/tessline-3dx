
#ifndef __TS3_CORE_GDS_TYPE_SUPPORT_STD_CORE_H__
#define __TS3_CORE_GDS_TYPE_SUPPORT_STD_CORE_H__

#include "gdsTypeSupportCommon.h"

namespace std
{

	template <typename _Ty1, typename _Ty2>
	struct pair;

	template <typename _Ch, typename _Tr, typename _Alloc>
	class basic_string;

}

namespace ts3
{

	namespace gds
	{


		/***********************************************************************************************************/
		/******************************************** std::basic_string ********************************************/
		/***********************************************************************************************************/

		template <typename _Ch, typename _Tr, typename _Alloc, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::basic_string<_Ch, _Tr, _Alloc> & pString,
                                        const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;

			// The first member is the length of the specified string.
			byteSize += evalByteSize( emptySizeType() );

			// Then we have the characters (potentially cast).
			byteSize += pString.length() * evalByteSize( _Ch{}, pCastTag );

			return byteSize;
		}

		template <typename _Ch, typename _Tr, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const std::basic_string<_Ch, _Tr, _Alloc> & pString,
                                     const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			const auto strLength = pString.length();

			gds_size_t byteSize = serialize( pOutputBuffer, asSizeType( strLength ) );

			for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
			{
				byteSize += serialize( pOutputBuffer + byteSize, pString[charIndex], pCastTag );
			}

			return byteSize;
		}

		template <typename _Ch, typename _Tr, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       std::basic_string<_Ch, _Tr, _Alloc> & pString,
                                       const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			size_t strLength = 0;
			gds_size_t byteSize = deserialize( pInputData, asSizeType( strLength ) );

			pString.resize( strLength );

			for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
			{
				byteSize += deserialize( pInputData + byteSize, pString[charIndex], pCastTag );
			}

			return byteSize;
		}

		/***********************************************************************************************************/
		/************************************************ std::pair ************************************************/
		/***********************************************************************************************************/

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t evalByteSize( const std::pair<_Ty1, _Ty2> & pPair )
		{
			gds_size_t byteSize = 0;
			byteSize += evalByteSize( pPair.first );
			byteSize += evalByteSize( pPair.second );
			return byteSize;
		}

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t serialize( byte * pOutputBuffer, const std::pair<_Ty1, _Ty2> & pPair )
		{
			gds_size_t byteSize = 0;
			byteSize += serialize( pOutputBuffer, pPair.first );
			byteSize += serialize( pOutputBuffer, pPair.second );
			return byteSize;
		}

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t deserialize( const byte * pInputData, std::pair<_Ty1, _Ty2> & pPair )
		{
			gds_size_t byteSize = 0;
			byteSize += deserialize( pInputData, pPair.first );
			byteSize += deserialize( pInputData, pPair.second );
			return byteSize;
		}

	}

}

#endif // __TS3_CORE_GDS_TYPE_SUPPORT_STD_CORE_H__
