
#ifndef __TS3_STDEXT_GDS_STD_CORE_H__
#define __TS3_STDEXT_GDS_STD_CORE_H__

#include "prerequisites.h"
#include <ts3/platform/gds.h>

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
		inline gds_size_t evalByteSize(
				const std::basic_string<_Ch, _Tr, _Alloc> & pString,
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
		inline gds_size_t queryMinByteSize(
				const std::basic_string<_Ch, _Tr, _Alloc> & pString,
				const TypeCastTag<_Cast> & = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;
			// The length of a string is always there, expressed as a size_type. Empty string contains just this.
			byteSize += evalByteSize( emptySizeType() );
			return byteSize;
		}

		template <typename _Ch, typename _Tr, typename _Alloc, typename _Cast = void>
		inline gds_size_t serialize(
				byte * pOutputBuffer,
				EByteOrder pByteOrder,
				const std::basic_string<_Ch, _Tr, _Alloc> & pString,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			const auto strLength = pString.length();

			gds_size_t byteSize = serialize( pOutputBuffer, pByteOrder, asSizeType( strLength ) );

			for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
			{
				byteSize += serialize( pOutputBuffer + byteSize, pByteOrder, pString[charIndex], pCastTag );
			}

			return byteSize;
		}

		template <typename _Ch, typename _Tr, typename _Alloc, typename _Cast = void>
		inline gds_size_t deserialize(
				const byte * pInputData,
				EByteOrder pByteOrder,
				std::basic_string<_Ch, _Tr, _Alloc> & pString,
				const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			size_t strLength = 0;

			gds_size_t byteSize = deserialize( pInputData, pByteOrder, asSizeType( strLength ) );

			pString.resize( strLength );

			for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
			{
				byteSize += deserialize( pInputData + byteSize, pByteOrder, pString[charIndex], pCastTag );
			}

			return byteSize;
		}

		/***********************************************************************************************************/
		/************************************************ std::pair ************************************************/
		/***********************************************************************************************************/

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t evalByteSize(
			const std::pair<_Ty1, _Ty2> & pPair,
			const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;
			byteSize += evalByteSize( pPair.first, pCastTag );
			byteSize += evalByteSize( pPair.second, pCastTag );
			return byteSize;
		}

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t queryMinByteSize(
			const std::pair<_Ty1, _Ty2> & pPair,
			const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;
			byteSize += queryMinByteSize( pPair.first, pCastTag );
			byteSize += queryMinByteSize( pPair.second, pCastTag );
			return byteSize;
		}

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t serialize(
			byte * pOutputBuffer,
			EByteOrder pByteOrder,
			const std::pair<_Ty1, _Ty2> & pPair,
			const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;
			byteSize += serialize( pOutputBuffer, pByteOrder, pPair.first, pCastTag );
			byteSize += serialize( pOutputBuffer, pByteOrder, pPair.second, pCastTag );
			return byteSize;
		}

		template <typename _Ty1, typename _Ty2, typename _Cast = void>
		inline gds_size_t deserialize(
			const byte * pInputData,
			EByteOrder pByteOrder,
			std::pair<_Ty1, _Ty2> & pPair,
			const TypeCastTag<_Cast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = 0;
			byteSize += deserialize( pInputData, pByteOrder, pPair.first, pCastTag );
			byteSize += deserialize( pInputData, pByteOrder, pPair.second, pCastTag );
			return byteSize;
		}

	}

}

#endif // __TS3_STDEXT_GDS_STD_CORE_H__
