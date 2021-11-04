
#include "DX_coreAPIProxy.h"
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/stdext/mapUtils.h>
#include <ts3/stdext/utilities.h>

namespace ts3::gpuapi
{

	DXGIGetDebugInterfaceType DXCoreAPIProxy::loadDXGIDebugLegacyLoader()
	{
		HMODULE dxgiDebugLib = ::LoadLibraryExA( "dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32 );
		if( !dxgiDebugLib )
		{
			return nullptr;
		}

		auto dxgiProcAddress = ::GetProcAddress( dxgiDebugLib, "DXGIGetDebugInterface" );
		if( !dxgiProcAddress )
		{
			return nullptr;
		}

		return reinterpret_cast<DXGIGetDebugInterfaceType>( dxgiProcAddress );
	}

	ComPtr<IDXGIDebug> DXCoreAPIProxy::queryDXGIDebugInterface( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags )
	{
		if( !pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			return nullptr;
		}

		ComPtr<IDXGIDebug> dxgiDebugInterface;

	#if( TS3_DRIVER_GPUAPI_DX12_BUILD )
		ComPtr<IDXGIDebug1> dxgiDebugInterface1;
		auto hResult = DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &dxgiDebugInterface1 ) );
		if( FAILED( hResult ) )
		{
			// TODO: warning
		}
		dxgiDebugInterface = std::move( dxgiDebugInterface1 );
	#elif( TS3_DRIVER_GPUAPI_DX11_BUILD )
		auto DXGIGetDebugInterface = loadDXGIDebugLegacyLoader();
		auto hResult = DXGIGetDebugInterface( IID_PPV_ARGS( &dxgiDebugInterface ) );
		if( FAILED( hResult ) )
		{
			// TODO: warning
		}
	#endif

		return dxgiDebugInterface;
	}

	ComPtr<IDXGIInfoQueue> DXCoreAPIProxy::queryDXGIDebugInfoQueue( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags )
	{
		if( !pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			return nullptr;
		}

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

	#if( TS3_DRIVER_GPUAPI_DX12_BUILD )
		auto hResult = DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &dxgiInfoQueue ) );
		if( FAILED( hResult ) )
		{
			// TODO: warning
		}
	#elif( TS3_DRIVER_GPUAPI_DX11_BUILD )
		auto DXGIGetDebugInterface = loadDXGIDebugLegacyLoader();
		auto hResult = DXGIGetDebugInterface( IID_PPV_ARGS( &dxgiInfoQueue ) );
		if( FAILED( hResult ) )
		{
			// TODO: warning
		}
	#endif

		return dxgiInfoQueue;
	}

	uint32 DXCoreAPIProxy::getDXGITextureFormatBPP( DXGI_FORMAT pDXGIFormat )
	{
	    switch( pDXGIFormat )
		{
			ts3CaseReturn( DXGI_FORMAT_UNKNOWN                    , 0   );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32A32_TYPELESS      , 128 );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32A32_FLOAT         , 128 );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32A32_UINT          , 128 );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32A32_SINT          , 128 );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32_TYPELESS         , 128 );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32_FLOAT            , 96  );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32_UINT             , 96  );
			ts3CaseReturn( DXGI_FORMAT_R32G32B32_SINT             , 96  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_TYPELESS      , 64  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_FLOAT         , 64  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_UNORM         , 64  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_UINT          , 64  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_SNORM         , 64  );
			ts3CaseReturn( DXGI_FORMAT_R16G16B16A16_SINT          , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32G32_TYPELESS            , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32G32_FLOAT               , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32G32_UINT                , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32G32_SINT                , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32G8X24_TYPELESS          , 64  );
			ts3CaseReturn( DXGI_FORMAT_D32_FLOAT_S8X24_UINT       , 64  );
			ts3CaseReturn( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS   , 64  );
			ts3CaseReturn( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT    , 64  );
			ts3CaseReturn( DXGI_FORMAT_R10G10B10A2_TYPELESS       , 32  );
			ts3CaseReturn( DXGI_FORMAT_R10G10B10A2_UNORM          , 32  );
			ts3CaseReturn( DXGI_FORMAT_R10G10B10A2_UINT           , 32  );
			ts3CaseReturn( DXGI_FORMAT_R11G11B10_FLOAT            , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_TYPELESS          , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_UNORM             , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB        , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_UINT              , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_SNORM             , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8B8A8_SINT              , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_TYPELESS            , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_FLOAT               , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_UNORM               , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_UINT                , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_SNORM               , 32  );
			ts3CaseReturn( DXGI_FORMAT_R16G16_SINT                , 32  );
			ts3CaseReturn( DXGI_FORMAT_R32_TYPELESS               , 32  );
			ts3CaseReturn( DXGI_FORMAT_D32_FLOAT                  , 32  );
			ts3CaseReturn( DXGI_FORMAT_R32_FLOAT                  , 32  );
			ts3CaseReturn( DXGI_FORMAT_R32_UINT                   , 32  );
			ts3CaseReturn( DXGI_FORMAT_R32_SINT                   , 32  );
			ts3CaseReturn( DXGI_FORMAT_R24G8_TYPELESS             , 32  );
			ts3CaseReturn( DXGI_FORMAT_D24_UNORM_S8_UINT          , 32  );
			ts3CaseReturn( DXGI_FORMAT_R24_UNORM_X8_TYPELESS      , 32  );
			ts3CaseReturn( DXGI_FORMAT_X24_TYPELESS_G8_UINT       , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_TYPELESS              , 16  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_UNORM                 , 16  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_UINT                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_SNORM                 , 16  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_SINT                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_TYPELESS               , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_FLOAT                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_D16_UNORM                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_UNORM                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_UINT                   , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_SNORM                  , 16  );
			ts3CaseReturn( DXGI_FORMAT_R16_SINT                   , 16  );
			ts3CaseReturn( DXGI_FORMAT_R8_TYPELESS                , 8   );
			ts3CaseReturn( DXGI_FORMAT_R8_UNORM                   , 8   );
			ts3CaseReturn( DXGI_FORMAT_R8_UINT                    , 8   );
			ts3CaseReturn( DXGI_FORMAT_R8_SNORM                   , 8   );
			ts3CaseReturn( DXGI_FORMAT_R8_SINT                    , 8   );
			ts3CaseReturn( DXGI_FORMAT_A8_UNORM                   , 8   );
			ts3CaseReturn( DXGI_FORMAT_R1_UNORM                   , 8   );
			ts3CaseReturn( DXGI_FORMAT_R9G9B9E5_SHAREDEXP         , 32  );
			ts3CaseReturn( DXGI_FORMAT_R8G8_B8G8_UNORM            , 32  );
			ts3CaseReturn( DXGI_FORMAT_G8R8_G8B8_UNORM            , 32  );
			ts3CaseReturn( DXGI_FORMAT_BC1_TYPELESS               , 4   );
			ts3CaseReturn( DXGI_FORMAT_BC1_UNORM                  , 4   );
			ts3CaseReturn( DXGI_FORMAT_BC1_UNORM_SRGB             , 4   );
			ts3CaseReturn( DXGI_FORMAT_BC2_TYPELESS               , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC2_UNORM                  , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC2_UNORM_SRGB             , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC3_TYPELESS               , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC3_UNORM                  , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC3_UNORM_SRGB             , 8   );
			ts3CaseReturn( DXGI_FORMAT_BC4_TYPELESS               , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC4_UNORM                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC4_SNORM                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC5_TYPELESS               , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC5_UNORM                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC5_SNORM                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_B5G6R5_UNORM               , 16  );
			ts3CaseReturn( DXGI_FORMAT_B5G5R5A1_UNORM             , 16  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8A8_UNORM             , 32  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8X8_UNORM             , 32  );
			ts3CaseReturn( DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM , 32  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8A8_TYPELESS          , 32  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB        , 32  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8X8_TYPELESS          , 32  );
			ts3CaseReturn( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB        , 32  );
			ts3CaseReturn( DXGI_FORMAT_BC6H_TYPELESS              , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC6H_UF16                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC6H_SF16                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC7_TYPELESS               , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC7_UNORM                  , 0   );
			ts3CaseReturn( DXGI_FORMAT_BC7_UNORM_SRGB             , 0   );
		};
		return DXGI_FORMAT_UNKNOWN;
	}

	const char * DXCoreAPIProxy::getDXShaderTargetStr( DXShaderTarget pShaderTarget )
	{
		static const char * const shaderTargetStrArray[] =
		{
			"vs_4_1",
			"gs_4_1",
			"ps_4_1",
			"cs_4_1",
			"vs_5_0",
			"hs_5_0",
			"ds_5_0",
			"gs_5_0",
			"ps_5_0",
			"cs_5_0",
		};
		return staticArrayElement( shaderTargetStrArray, pShaderTarget );
	}

	uint32 DXCoreAPIProxy::computeDXTextureMemoryByteSize( const TextureDimensions & pTextDimensions, DXGI_FORMAT pFormat )
	{
		const uint64 formatBitSize = getDXGITextureFormatBPP( pFormat );
		const uint64 totalPixelCount = pTextDimensions.width * pTextDimensions.height * pTextDimensions.depth * pTextDimensions.arraySize;

		uint64 totalBaseBitSize = totalPixelCount * formatBitSize;
		uint64 byteSizeDivisor = 8;

		if( pTextDimensions.mipLevelsNum > 1 )
		{
			totalBaseBitSize *= 4;
			byteSizeDivisor *= 3;
		}

		return static_cast<uint32>( ( double )totalBaseBitSize / ( double )byteSizeDivisor );
	}

	Bitmask<UINT> DXCoreAPIProxy::translateDXShaderCompileFlags( Bitmask<uint32> pShaderCreateFlags, bool pDebugDevice )
	{
		Bitmask<UINT> compileFlags = 0;
		if( pDebugDevice || pShaderCreateFlags.isSet( E_SHADER_CREATE_FLAG_DEBUG_BIT ) )
		{
			compileFlags.set( D3DCOMPILE_DEBUG );
		}
		if( pDebugDevice || pShaderCreateFlags.isSet( E_SHADER_CREATE_FLAG_OPTIMIZATION_DISABLE_BIT ) )
		{
			compileFlags.set( D3DCOMPILE_SKIP_OPTIMIZATION );
		}
		if( pShaderCreateFlags.isSet( E_SHADER_CREATE_FLAG_OPTIMIZATION_L0_BIT ) )
		{
			compileFlags.set( D3DCOMPILE_OPTIMIZATION_LEVEL0 );
		}
		if( !pDebugDevice || pShaderCreateFlags.isSet( E_SHADER_CREATE_FLAG_OPTIMIZATION_L1_BIT ) )
		{
			compileFlags.set( D3DCOMPILE_OPTIMIZATION_LEVEL3 );
		}
		return compileFlags;
	}

	DXGI_FORMAT DXCoreAPIProxy::translateDXBaseDataType( EBaseDataType pBaseDataType )
	{
		static const DXGI_FORMAT baseDataTypeArray[] =
		{
			DXGI_FORMAT_R8_SINT,
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R16_SINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
		};
		auto baseDataTypeIndex = ecGetBaseDataTypeIndex( pBaseDataType );
		return staticArrayElement( baseDataTypeArray, baseDataTypeIndex );
	}

	DXGI_FORMAT DXCoreAPIProxy::translateDXTextureFormat( ETextureFormat pTextureFormat )
	{
	    switch( pTextureFormat )
		{
			ts3CaseReturn( ETextureFormat::R32_FLOAT           , DXGI_FORMAT_R32_FLOAT            );
			ts3CaseReturn( ETextureFormat::R32_SINT            , DXGI_FORMAT_R32_SINT             );
			ts3CaseReturn( ETextureFormat::R32_UINT            , DXGI_FORMAT_R32_UINT             );
			ts3CaseReturn( ETextureFormat::R32G32_FLOAT        , DXGI_FORMAT_R32G32_FLOAT         );
			ts3CaseReturn( ETextureFormat::R32G32_SINT         , DXGI_FORMAT_R32G32_SINT          );
			ts3CaseReturn( ETextureFormat::R32G32_UINT         , DXGI_FORMAT_R32G32_UINT          );
			ts3CaseReturn( ETextureFormat::R32G32B32_FLOAT     , DXGI_FORMAT_R32G32B32_FLOAT      );
			ts3CaseReturn( ETextureFormat::R32G32B32_SINT      , DXGI_FORMAT_R32G32B32_SINT       );
			ts3CaseReturn( ETextureFormat::R32G32B32_UINT      , DXGI_FORMAT_R32G32B32_UINT       );
			ts3CaseReturn( ETextureFormat::R32G32B32A32_FLOAT  , DXGI_FORMAT_R32G32B32A32_FLOAT   );
			ts3CaseReturn( ETextureFormat::R32G32B32A32_SINT   , DXGI_FORMAT_R32G32B32A32_SINT    );
			ts3CaseReturn( ETextureFormat::R32G32B32A32_UINT   , DXGI_FORMAT_R32G32B32A32_UINT    );
			ts3CaseReturn( ETextureFormat::R16_FLOAT           , DXGI_FORMAT_R16_FLOAT            );
			ts3CaseReturn( ETextureFormat::R16_SINT            , DXGI_FORMAT_R16_SINT             );
			ts3CaseReturn( ETextureFormat::R16_UINT            , DXGI_FORMAT_R16_UINT             );
			ts3CaseReturn( ETextureFormat::R16G16_FLOAT        , DXGI_FORMAT_R16G16_FLOAT         );
			ts3CaseReturn( ETextureFormat::R16G16_SINT         , DXGI_FORMAT_R16G16_SINT          );
			ts3CaseReturn( ETextureFormat::R16G16_UINT         , DXGI_FORMAT_R16G16_UINT          );
			ts3CaseReturn( ETextureFormat::R16G16B16A16_FLOAT  , DXGI_FORMAT_R16G16B16A16_FLOAT   );
			ts3CaseReturn( ETextureFormat::R16G16B16A16_SINT   , DXGI_FORMAT_R16G16B16A16_SINT    );
			ts3CaseReturn( ETextureFormat::R16G16B16A16_UINT   , DXGI_FORMAT_R16G16B16A16_UINT    );
			ts3CaseReturn( ETextureFormat::R8_SINT             , DXGI_FORMAT_R8_SINT              );
			ts3CaseReturn( ETextureFormat::R8_UINT             , DXGI_FORMAT_R8_UINT              );
			ts3CaseReturn( ETextureFormat::R8_SNORM            , DXGI_FORMAT_R8_SNORM             );
			ts3CaseReturn( ETextureFormat::R8_UNORM            , DXGI_FORMAT_R8_UNORM             );
			ts3CaseReturn( ETextureFormat::R8G8_SINT           , DXGI_FORMAT_R8G8_SINT            );
			ts3CaseReturn( ETextureFormat::R8G8_UINT           , DXGI_FORMAT_R8G8_UINT            );
			ts3CaseReturn( ETextureFormat::R8G8_SNORM          , DXGI_FORMAT_R8G8_SNORM           );
			ts3CaseReturn( ETextureFormat::R8G8_UNORM          , DXGI_FORMAT_R8G8_UNORM           );
			ts3CaseReturn( ETextureFormat::B8G8R8X8_UNORM      , DXGI_FORMAT_B8G8R8X8_UNORM       );
			ts3CaseReturn( ETextureFormat::B8G8R8X8_SRGB       , DXGI_FORMAT_B8G8R8X8_UNORM_SRGB  );
			ts3CaseReturn( ETextureFormat::B8G8R8A8_UNORM      , DXGI_FORMAT_B8G8R8A8_UNORM       );
			ts3CaseReturn( ETextureFormat::B8G8R8A8_SRGB       , DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  );
			ts3CaseReturn( ETextureFormat::R8G8B8A8_SINT       , DXGI_FORMAT_R8G8B8A8_SINT        );
			ts3CaseReturn( ETextureFormat::R8G8B8A8_UINT       , DXGI_FORMAT_R8G8B8A8_UINT        );
			ts3CaseReturn( ETextureFormat::R8G8B8A8_SNORM      , DXGI_FORMAT_R8G8B8A8_SNORM       );
			ts3CaseReturn( ETextureFormat::R8G8B8A8_UNORM      , DXGI_FORMAT_R8G8B8A8_UNORM       );
			ts3CaseReturn( ETextureFormat::R8G8B8A8_SRGB       , DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  );

			ts3CaseReturn( ETextureFormat::R5G5B5A1            , DXGI_FORMAT_B5G5R5A1_UNORM     );
			ts3CaseReturn( ETextureFormat::R5G6B5              , DXGI_FORMAT_B5G6R5_UNORM       );
			ts3CaseReturn( ETextureFormat::R9G9B9E5            , DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
			ts3CaseReturn( ETextureFormat::R10G10B10A2_UINT    , DXGI_FORMAT_R10G10B10A2_UINT   );
			ts3CaseReturn( ETextureFormat::R10G10B10A2_UNORM   , DXGI_FORMAT_R10G10B10A2_UNORM  );
			ts3CaseReturn( ETextureFormat::R11G11B10_FLOAT32   , DXGI_FORMAT_R11G11B10_FLOAT    );
			ts3CaseReturn( ETextureFormat::D16_UNORM           , DXGI_FORMAT_D16_UNORM          );
			ts3CaseReturn( ETextureFormat::D24_UNORM_S8_UINT   , DXGI_FORMAT_D24_UNORM_S8_UINT  );
			ts3CaseReturn( ETextureFormat::D24_UNORM_X8        , DXGI_FORMAT_D24_UNORM_S8_UINT  );
			ts3CaseReturn( ETextureFormat::D32_FLOAT           , DXGI_FORMAT_D32_FLOAT          );

			ts3CaseReturn( ETextureFormat::BC1_RGBA_UNORM      , DXGI_FORMAT_BC1_UNORM      );
			ts3CaseReturn( ETextureFormat::BC1_RGBA_UNORM_SRGB , DXGI_FORMAT_BC1_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC2_UNORM           , DXGI_FORMAT_BC2_UNORM      );
			ts3CaseReturn( ETextureFormat::BC2_UNORM_SRGB      , DXGI_FORMAT_BC2_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC3_UNORM           , DXGI_FORMAT_BC3_UNORM      );
			ts3CaseReturn( ETextureFormat::BC3_UNORM_SRGB      , DXGI_FORMAT_BC3_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC4_SNORM           , DXGI_FORMAT_BC4_SNORM      );
			ts3CaseReturn( ETextureFormat::BC4_UNORM           , DXGI_FORMAT_BC4_UNORM      );
			ts3CaseReturn( ETextureFormat::BC5_SNORM           , DXGI_FORMAT_BC5_SNORM      );
			ts3CaseReturn( ETextureFormat::BC5_UNORM           , DXGI_FORMAT_BC5_UNORM      );
			ts3CaseReturn( ETextureFormat::BC6H_SFLOAT         , DXGI_FORMAT_BC6H_SF16      );
			ts3CaseReturn( ETextureFormat::BC6H_UFLOAT         , DXGI_FORMAT_BC6H_UF16      );
			ts3CaseReturn( ETextureFormat::BC7_UNORM           , DXGI_FORMAT_BC7_UNORM      );
			ts3CaseReturn( ETextureFormat::BC7_UNORM_SRGB      , DXGI_FORMAT_BC7_UNORM_SRGB );
		};
		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT DXCoreAPIProxy::translateDXVertexAttribFormat( EVertexAttribFormat pVertexAttribFormat )
	{
	    switch( pVertexAttribFormat )
		{
			ts3CaseReturn( EVertexAttribFormat::FLOAT16          , DXGI_FORMAT_R16_FLOAT          );
			ts3CaseReturn( EVertexAttribFormat::FLOAT32          , DXGI_FORMAT_R32_FLOAT          );
			ts3CaseReturn( EVertexAttribFormat::SBYTE            , DXGI_FORMAT_R8_SINT            );
			ts3CaseReturn( EVertexAttribFormat::SINT16           , DXGI_FORMAT_R16_SINT           );
			ts3CaseReturn( EVertexAttribFormat::SINT32           , DXGI_FORMAT_R32_SINT           );
			ts3CaseReturn( EVertexAttribFormat::UBYTE            , DXGI_FORMAT_R8_UINT            );
			ts3CaseReturn( EVertexAttribFormat::UINT16           , DXGI_FORMAT_R16_UINT           );
			ts3CaseReturn( EVertexAttribFormat::UINT32           , DXGI_FORMAT_R32_UINT           );
			ts3CaseReturn( EVertexAttribFormat::SBYTE_NORM       , DXGI_FORMAT_R8_SNORM           );
			ts3CaseReturn( EVertexAttribFormat::SINT16_NORM      , DXGI_FORMAT_R16_SNORM          );
			ts3CaseReturn( EVertexAttribFormat::UBYTE_NORM       , DXGI_FORMAT_R8_UNORM           );
			ts3CaseReturn( EVertexAttribFormat::UINT16_NORM      , DXGI_FORMAT_R16_UNORM          );
			ts3CaseReturn( EVertexAttribFormat::VEC2_FLOAT16     , DXGI_FORMAT_R16G16_FLOAT       );
			ts3CaseReturn( EVertexAttribFormat::VEC2_FLOAT32     , DXGI_FORMAT_R32G32_FLOAT       );
			ts3CaseReturn( EVertexAttribFormat::VEC2_SBYTE       , DXGI_FORMAT_R8G8_SINT          );
			ts3CaseReturn( EVertexAttribFormat::VEC2_SINT16      , DXGI_FORMAT_R16G16_SINT        );
			ts3CaseReturn( EVertexAttribFormat::VEC2_SINT32      , DXGI_FORMAT_R32G32_SINT        );
			ts3CaseReturn( EVertexAttribFormat::VEC2_UBYTE       , DXGI_FORMAT_R8G8_UINT          );
			ts3CaseReturn( EVertexAttribFormat::VEC2_UINT16      , DXGI_FORMAT_R16G16_UINT        );
			ts3CaseReturn( EVertexAttribFormat::VEC2_UINT32      , DXGI_FORMAT_R32G32_UINT        );
			ts3CaseReturn( EVertexAttribFormat::VEC2_SBYTE_NORM  , DXGI_FORMAT_R8G8_SNORM         );
			ts3CaseReturn( EVertexAttribFormat::VEC2_SINT16_NORM , DXGI_FORMAT_R16G16_SNORM       );
			ts3CaseReturn( EVertexAttribFormat::VEC2_UBYTE_NORM  , DXGI_FORMAT_R8G8_UNORM         );
			ts3CaseReturn( EVertexAttribFormat::VEC2_UINT16_NORM , DXGI_FORMAT_R16G16_UNORM       );
			ts3CaseReturn( EVertexAttribFormat::VEC3_FLOAT32     , DXGI_FORMAT_R32G32B32_FLOAT    );
			ts3CaseReturn( EVertexAttribFormat::VEC3_SINT32      , DXGI_FORMAT_R32G32B32_SINT     );
			ts3CaseReturn( EVertexAttribFormat::VEC3_UINT32      , DXGI_FORMAT_R32G32B32_UINT     );
			ts3CaseReturn( EVertexAttribFormat::VEC4_FLOAT16     , DXGI_FORMAT_R16G16B16A16_FLOAT );
			ts3CaseReturn( EVertexAttribFormat::VEC4_FLOAT32     , DXGI_FORMAT_R32G32B32A32_FLOAT );
			ts3CaseReturn( EVertexAttribFormat::VEC4_SBYTE       , DXGI_FORMAT_R8G8B8A8_SINT      );
			ts3CaseReturn( EVertexAttribFormat::VEC4_SINT16      , DXGI_FORMAT_R16G16B16A16_SINT  );
			ts3CaseReturn( EVertexAttribFormat::VEC4_SINT32      , DXGI_FORMAT_R32G32B32A32_SINT  );
			ts3CaseReturn( EVertexAttribFormat::VEC4_UBYTE       , DXGI_FORMAT_R8G8B8A8_UINT      );
			ts3CaseReturn( EVertexAttribFormat::VEC4_UINT16      , DXGI_FORMAT_R16G16B16A16_UINT  );
			ts3CaseReturn( EVertexAttribFormat::VEC4_UINT32      , DXGI_FORMAT_R32G32B32A32_UINT  );
			ts3CaseReturn( EVertexAttribFormat::VEC4_SBYTE_NORM  , DXGI_FORMAT_R8G8B8A8_SNORM     );
			ts3CaseReturn( EVertexAttribFormat::VEC4_SINT16_NORM , DXGI_FORMAT_R16G16B16A16_SNORM );
			ts3CaseReturn( EVertexAttribFormat::VEC4_UBYTE_NORM  , DXGI_FORMAT_R8G8B8A8_UNORM     );
			ts3CaseReturn( EVertexAttribFormat::VEC4_UINT16_NORM , DXGI_FORMAT_R16G16B16A16_UNORM );
		};
	    return DXGI_FORMAT_UNKNOWN;
	}

}
