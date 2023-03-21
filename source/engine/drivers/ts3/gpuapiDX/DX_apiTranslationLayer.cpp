
#include "DX_apiTranslationLayer.h"
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/stdext/stlHelperAlgo.h>
#include <ts3/stdext/utilities.h>

namespace ts3::gpuapi
{

	DXGIGetDebugInterfaceType atl::loadDXGIDebugLegacyLoader()
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

	ComPtr<IDXGIDebug> atl::queryDXGIDebugInterface( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags )
	{
		if( !pDriverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
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

	ComPtr<IDXGIInfoQueue> atl::queryDXGIDebugInfoQueue( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags )
	{
		if( !pDriverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
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

	uint32 atl::getDXGITextureFormatBPP( DXGI_FORMAT pDXGIFormat )
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

	const char * atl::getDXShaderTargetStr( DXShaderTarget pShaderTarget )
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

	uint32 atl::computeDXTextureMemoryByteSize( const TextureDimensions & pTextDimensions, DXGI_FORMAT pFormat )
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

	Bitmask<UINT> atl::translateDXShaderCompileFlags( Bitmask<uint32> pShaderCreateFlags, bool pDebugDevice )
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

	DXGI_FORMAT atl::translateDXBaseDataType( EBaseDataType pBaseDataType )
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
		auto baseDataTypeIndex = cxdefs::getBaseDataTypeIndex( pBaseDataType );
		return staticArrayElement( baseDataTypeArray, baseDataTypeIndex );
	}

	DXGI_FORMAT atl::translateDXTextureFormat( ETextureFormat pTextureFormat )
	{
	    switch( pTextureFormat )
		{
            ts3CaseReturn( ETextureFormat::UNKNOWN             , DXGI_FORMAT_UNKNOWN              );
			ts3CaseReturn( ETextureFormat::R32F                , DXGI_FORMAT_R32_FLOAT            );
			ts3CaseReturn( ETextureFormat::R32I                , DXGI_FORMAT_R32_SINT             );
			ts3CaseReturn( ETextureFormat::R32U                , DXGI_FORMAT_R32_UINT             );
			ts3CaseReturn( ETextureFormat::RG32F               , DXGI_FORMAT_R32G32_FLOAT         );
			ts3CaseReturn( ETextureFormat::RG32I               , DXGI_FORMAT_R32G32_SINT          );
			ts3CaseReturn( ETextureFormat::RG32U               , DXGI_FORMAT_R32G32_UINT          );
			ts3CaseReturn( ETextureFormat::RGB32F              , DXGI_FORMAT_R32G32B32_FLOAT      );
			ts3CaseReturn( ETextureFormat::RGB32I              , DXGI_FORMAT_R32G32B32_SINT       );
			ts3CaseReturn( ETextureFormat::RGB32U              , DXGI_FORMAT_R32G32B32_UINT       );
			ts3CaseReturn( ETextureFormat::RGBA32F             , DXGI_FORMAT_R32G32B32A32_FLOAT   );
			ts3CaseReturn( ETextureFormat::RGBA32I             , DXGI_FORMAT_R32G32B32A32_SINT    );
			ts3CaseReturn( ETextureFormat::RGBA32U             , DXGI_FORMAT_R32G32B32A32_UINT    );
			ts3CaseReturn( ETextureFormat::R16F                , DXGI_FORMAT_R16_FLOAT            );
			ts3CaseReturn( ETextureFormat::R16I                , DXGI_FORMAT_R16_SINT             );
			ts3CaseReturn( ETextureFormat::R16U                , DXGI_FORMAT_R16_UINT             );
			ts3CaseReturn( ETextureFormat::RG16F               , DXGI_FORMAT_R16G16_FLOAT         );
			ts3CaseReturn( ETextureFormat::RG16I               , DXGI_FORMAT_R16G16_SINT          );
			ts3CaseReturn( ETextureFormat::RG16U               , DXGI_FORMAT_R16G16_UINT          );
			ts3CaseReturn( ETextureFormat::RGBA16F             , DXGI_FORMAT_R16G16B16A16_FLOAT   );
			ts3CaseReturn( ETextureFormat::RGBA16I             , DXGI_FORMAT_R16G16B16A16_SINT    );
			ts3CaseReturn( ETextureFormat::RGBA16U             , DXGI_FORMAT_R16G16B16A16_UINT    );
			ts3CaseReturn( ETextureFormat::R8I                 , DXGI_FORMAT_R8_SINT              );
			ts3CaseReturn( ETextureFormat::R8U                 , DXGI_FORMAT_R8_UINT              );
			ts3CaseReturn( ETextureFormat::R8IN                , DXGI_FORMAT_R8_SNORM             );
			ts3CaseReturn( ETextureFormat::R8UN                , DXGI_FORMAT_R8_UNORM             );
			ts3CaseReturn( ETextureFormat::RG8I                , DXGI_FORMAT_R8G8_SINT            );
			ts3CaseReturn( ETextureFormat::RG8U                , DXGI_FORMAT_R8G8_UINT            );
			ts3CaseReturn( ETextureFormat::RG8IN               , DXGI_FORMAT_R8G8_SNORM           );
			ts3CaseReturn( ETextureFormat::RG8UN               , DXGI_FORMAT_R8G8_UNORM           );
            ts3CaseReturn( ETextureFormat::BGRX8UN             , DXGI_FORMAT_B8G8R8X8_UNORM       );
            ts3CaseReturn( ETextureFormat::BGRX8SRGB           , DXGI_FORMAT_B8G8R8X8_UNORM_SRGB  );
            ts3CaseReturn( ETextureFormat::BGRA8UN             , DXGI_FORMAT_B8G8R8A8_UNORM       );
            ts3CaseReturn( ETextureFormat::BGRA8SRGB           , DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  );
            ts3CaseReturn( ETextureFormat::RGBA8I              , DXGI_FORMAT_R8G8B8A8_SINT        );
            ts3CaseReturn( ETextureFormat::RGBA8U              , DXGI_FORMAT_R8G8B8A8_UINT        );
            ts3CaseReturn( ETextureFormat::RGBA8IN             , DXGI_FORMAT_R8G8B8A8_SNORM       );
            ts3CaseReturn( ETextureFormat::RGBA8UN             , DXGI_FORMAT_R8G8B8A8_UNORM       );
            ts3CaseReturn( ETextureFormat::RGBA8SRGB           , DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  );

			ts3CaseReturn( ETextureFormat::R5G5B5A1            , DXGI_FORMAT_B5G5R5A1_UNORM     );
			ts3CaseReturn( ETextureFormat::R5G6B5              , DXGI_FORMAT_B5G6R5_UNORM       );
			ts3CaseReturn( ETextureFormat::R9G9B9E5            , DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
			ts3CaseReturn( ETextureFormat::RGB10A2U            , DXGI_FORMAT_R10G10B10A2_UINT   );
			ts3CaseReturn( ETextureFormat::RGB10A2UN           , DXGI_FORMAT_R10G10B10A2_UNORM  );
			ts3CaseReturn( ETextureFormat::R11G11B10F          , DXGI_FORMAT_R11G11B10_FLOAT    );
			ts3CaseReturn( ETextureFormat::D16UN               , DXGI_FORMAT_D16_UNORM          );
			ts3CaseReturn( ETextureFormat::D24UNS8U            , DXGI_FORMAT_D24_UNORM_S8_UINT  );
			ts3CaseReturn( ETextureFormat::D24UNX8             , DXGI_FORMAT_D24_UNORM_S8_UINT  );
			ts3CaseReturn( ETextureFormat::D32F                , DXGI_FORMAT_D32_FLOAT          );

			ts3CaseReturn( ETextureFormat::BC1                 , DXGI_FORMAT_BC1_UNORM      );
			ts3CaseReturn( ETextureFormat::BC1SRGB             , DXGI_FORMAT_BC1_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC2                 , DXGI_FORMAT_BC2_UNORM      );
			ts3CaseReturn( ETextureFormat::BC2SRGB             , DXGI_FORMAT_BC2_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC3                 , DXGI_FORMAT_BC3_UNORM      );
			ts3CaseReturn( ETextureFormat::BC3SRGB             , DXGI_FORMAT_BC3_UNORM_SRGB );
			ts3CaseReturn( ETextureFormat::BC4IN               , DXGI_FORMAT_BC4_SNORM      );
			ts3CaseReturn( ETextureFormat::BC4UN               , DXGI_FORMAT_BC4_UNORM      );
			ts3CaseReturn( ETextureFormat::BC5IN               , DXGI_FORMAT_BC5_SNORM      );
			ts3CaseReturn( ETextureFormat::BC5UN               , DXGI_FORMAT_BC5_UNORM      );
			ts3CaseReturn( ETextureFormat::BC6HSF              , DXGI_FORMAT_BC6H_SF16      );
			ts3CaseReturn( ETextureFormat::BC6HUF              , DXGI_FORMAT_BC6H_UF16      );
			ts3CaseReturn( ETextureFormat::BC7                 , DXGI_FORMAT_BC7_UNORM      );
			ts3CaseReturn( ETextureFormat::BC7SRGB             , DXGI_FORMAT_BC7_UNORM_SRGB );
		};
		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT atl::translateDXVertexAttribFormat( EVertexAttribFormat pVertexAttribFormat )
	{
	    switch( pVertexAttribFormat )
		{
			ts3CaseReturn( EVertexAttribFormat::F16      , DXGI_FORMAT_R16_FLOAT          );
			ts3CaseReturn( EVertexAttribFormat::F32      , DXGI_FORMAT_R32_FLOAT          );
			ts3CaseReturn( EVertexAttribFormat::I8       , DXGI_FORMAT_R8_SINT            );
			ts3CaseReturn( EVertexAttribFormat::I16      , DXGI_FORMAT_R16_SINT           );
			ts3CaseReturn( EVertexAttribFormat::I32      , DXGI_FORMAT_R32_SINT           );
			ts3CaseReturn( EVertexAttribFormat::U8       , DXGI_FORMAT_R8_UINT            );
			ts3CaseReturn( EVertexAttribFormat::U16      , DXGI_FORMAT_R16_UINT           );
			ts3CaseReturn( EVertexAttribFormat::U32      , DXGI_FORMAT_R32_UINT           );
			ts3CaseReturn( EVertexAttribFormat::I8N      , DXGI_FORMAT_R8_SNORM           );
			ts3CaseReturn( EVertexAttribFormat::I16N     , DXGI_FORMAT_R16_SNORM          );
			ts3CaseReturn( EVertexAttribFormat::U8N      , DXGI_FORMAT_R8_UNORM           );
			ts3CaseReturn( EVertexAttribFormat::U16N     , DXGI_FORMAT_R16_UNORM          );
			ts3CaseReturn( EVertexAttribFormat::Vec2F16  , DXGI_FORMAT_R16G16_FLOAT       );
			ts3CaseReturn( EVertexAttribFormat::Vec2F32  , DXGI_FORMAT_R32G32_FLOAT       );
			ts3CaseReturn( EVertexAttribFormat::Vec2I8   , DXGI_FORMAT_R8G8_SINT          );
			ts3CaseReturn( EVertexAttribFormat::Vec2I16  , DXGI_FORMAT_R16G16_SINT        );
			ts3CaseReturn( EVertexAttribFormat::Vec2I32  , DXGI_FORMAT_R32G32_SINT        );
			ts3CaseReturn( EVertexAttribFormat::Vec2U8   , DXGI_FORMAT_R8G8_UINT          );
			ts3CaseReturn( EVertexAttribFormat::Vec2U16  , DXGI_FORMAT_R16G16_UINT        );
			ts3CaseReturn( EVertexAttribFormat::Vec2U32  , DXGI_FORMAT_R32G32_UINT        );
			ts3CaseReturn( EVertexAttribFormat::Vec2I8N  , DXGI_FORMAT_R8G8_SNORM         );
			ts3CaseReturn( EVertexAttribFormat::Vec2I16N , DXGI_FORMAT_R16G16_SNORM       );
			ts3CaseReturn( EVertexAttribFormat::Vec2U8N  , DXGI_FORMAT_R8G8_UNORM         );
			ts3CaseReturn( EVertexAttribFormat::Vec2U16N , DXGI_FORMAT_R16G16_UNORM       );
			ts3CaseReturn( EVertexAttribFormat::Vec3F32  , DXGI_FORMAT_R32G32B32_FLOAT    );
			ts3CaseReturn( EVertexAttribFormat::Vec3I32  , DXGI_FORMAT_R32G32B32_SINT     );
			ts3CaseReturn( EVertexAttribFormat::Vec3U32  , DXGI_FORMAT_R32G32B32_UINT     );
			ts3CaseReturn( EVertexAttribFormat::Vec4F16  , DXGI_FORMAT_R16G16B16A16_FLOAT );
			ts3CaseReturn( EVertexAttribFormat::Vec4F32  , DXGI_FORMAT_R32G32B32A32_FLOAT );
			ts3CaseReturn( EVertexAttribFormat::Vec4I8   , DXGI_FORMAT_R8G8B8A8_SINT      );
			ts3CaseReturn( EVertexAttribFormat::Vec4I16  , DXGI_FORMAT_R16G16B16A16_SINT  );
			ts3CaseReturn( EVertexAttribFormat::Vec4I32  , DXGI_FORMAT_R32G32B32A32_SINT  );
			ts3CaseReturn( EVertexAttribFormat::Vec4U8   , DXGI_FORMAT_R8G8B8A8_UINT      );
			ts3CaseReturn( EVertexAttribFormat::Vec4U16  , DXGI_FORMAT_R16G16B16A16_UINT  );
			ts3CaseReturn( EVertexAttribFormat::Vec4U32  , DXGI_FORMAT_R32G32B32A32_UINT  );
			ts3CaseReturn( EVertexAttribFormat::Vec4I8N  , DXGI_FORMAT_R8G8B8A8_SNORM     );
			ts3CaseReturn( EVertexAttribFormat::Vec4I16N , DXGI_FORMAT_R16G16B16A16_SNORM );
			ts3CaseReturn( EVertexAttribFormat::Vec4U8N  , DXGI_FORMAT_R8G8B8A8_UNORM     );
			ts3CaseReturn( EVertexAttribFormat::Vec4U16N , DXGI_FORMAT_R16G16B16A16_UNORM );
		};
	    return DXGI_FORMAT_UNKNOWN;
	}

} // namespace ts3::gpuapi
