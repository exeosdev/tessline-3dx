
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__
#define __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__

#include "graphicsPipelineState.h"
#include "graphicsShaderLinkage.h"
#include "inputAssemblerCommon.h"
#include <unordered_map>

namespace ts3::GpuAPI
{

	class GraphicsPipelineDescriptor;

	ts3GpaDeclareClassHandle( BlendStateDescriptor );
	ts3GpaDeclareClassHandle( DepthStencilStateDescriptor );
	ts3GpaDeclareClassHandle( RasterizerStateDescriptor );
	ts3GpaDeclareClassHandle( GraphicsShaderLinkageDescriptor );
	ts3GpaDeclareClassHandle( IAVertexInputLayoutDescriptor );
	ts3GpaDeclareClassHandle( IAVertexStreamDescriptor );

	struct BlendStateDescriptorCreateInfo;
	struct DepthStencilStateDescriptorCreateInfo;
	struct RasterizerStateDescriptorCreateInfo;
	struct GraphicsShaderLinkageDescriptorCreateInfo;
	struct IAVertexInputLayoutDescriptorCreateInfo;
	struct IAVertexStreamDescriptorCreateInfo;

	enum class EGraphicsPipelineCachedDescriptorType : uint32
	{
		BlendState = static_cast<uint32>( EGraphicsPipelineDescriptorType::BlendState ),
		DepthStencilState = static_cast<uint32>( EGraphicsPipelineDescriptorType::DepthStencilState ),
		IAVertexInputLayout = static_cast<uint32>( EGraphicsPipelineDescriptorType::IAVertexInputLayout ),
		RasterizerState = static_cast<uint32>( EGraphicsPipelineDescriptorType::RasterizerState ),
		ShaderLinkage = static_cast<uint32>( EGraphicsPipelineDescriptorType::ShaderLinkage ),
		Unknown,
	};

	/// @brief Dummy trait type for the descriptor cache. List all the things required for the traits to provide.
	class TS3_ATTR_MAYBE_UNUSED GraphicsPipelineDescriptorFactory2
	{
	public:
		using GPUDeviceType = GPUDevice;

	public:
		static BlendStateDescriptor * createDescriptor( GPUDeviceType &, const BlendStateDescriptorCreateInfo & );

		static DepthStencilStateDescriptor * createDescriptor( GPUDeviceType &, const DepthStencilStateDescriptorCreateInfo & );

		static RasterizerStateDescriptor * createDescriptor( GPUDeviceType &, const RasterizerStateDescriptorCreateInfo & );

		static GraphicsShaderLinkageDescriptor * createDescriptor( GPUDeviceType &, const GraphicsShaderLinkageDescriptorCreateInfo & );

		static IAVertexInputLayoutDescriptor * createDescriptor( GPUDeviceType &, const IAVertexInputLayoutDescriptorCreateInfo & );

		static IAVertexStreamDescriptor * createDescriptor( GPUDeviceType &, const IAVertexStreamDescriptorCreateInfo & );
	};

	template <typename TDescriptorFactory>
	class TS3_ATTR_MAYBE_UNUSED GraphicsPipelineDescriptorCache2
	{
	public:
		using DescriptorFactory = TDescriptorFactory;
		using DescriptorRef = std::pair<GraphicsPipelineDescriptor *, PipelineDescriptorID>;
		using GPUDeviceType = typename DescriptorFactory::GPUDeviceType;

		explicit GraphicsPipelineDescriptorCache2( GPUDeviceType & pGPUDevice );
		virtual ~GraphicsPipelineDescriptorCache2();

		DescriptorRef createBlendStateDescriptor( const BlendStateDescriptorCreateInfo & pCreateInfo )
		{
		}

		DescriptorRef createDepthStencilStateDescriptor( const DepthStencilStateDescriptorCreateInfo & pCreateInfo )
		{
		}

		DescriptorRef createRasterizerStateDescriptor( const RasterizerStateDescriptorCreateInfo & pCreateInfo )
		{
		}

		DescriptorRef createGraphicsShaderLinkageDescriptor( const GraphicsShaderLinkageDescriptorCreateInfo & pCreateInfo )
		{
		}

		DescriptorRef createIAVertexInputLayoutDescriptor( const IAVertexInputLayoutDescriptorCreateInfo & pCreateInfo )
		{
		}

		DescriptorRef createIAVertexStreamDescriptor( const IAVertexStreamDescriptorCreateInfo & pCreateInfo )
		{
		}

	private:
		struct CachedDescriptorInfo
		{
			EGraphicsPipelineCachedDescriptorType type = EGraphicsPipelineCachedDescriptorType::Unknown;
			PipelineDescriptorID id = CxDefs::PIPELINE_DESCRIPTOR_ID_INVALID;
		};

		// Thanks to additional pass-through createDescriptor() functions, we can have this function as a universal template.
		// It only needs an input desc and a descriptor type-specific map where pdesc of that type are being stored.
		template <typename TCreateInfo, typename TpDescriptorMap, typename... TpArgs>
		PipelineDescriptorID _createDescriptor( EGraphicsPipelineCachedDescriptorType pDescriptorType,
		                                        const TCreateInfo & pCreateInfo,
		                                        TpDescriptorMap & pDescriptorMap,
		                                        TpArgs && ...pArgs )
		{
			// First, compute a hash of the specified input desc structure. These hashes are stored in the separate map
			// as keys mapped to all pdesc (their IDs) created from the given desc structure. This allows us to
			// determine if there is already a descriptor matching the specified configuration.
			auto inputDescHash = StateMgmt::computePipelineDescriptorHash( pCreateInfo );

			// Get all pdesc with matching source hash. This properly handles potential collisions.
			auto descriptorRange = _descriptorInfoMap.equal_range( inputDescHash );

			if( descriptorRange.first != _descriptorInfoMap.end() )
			{
				for( auto descriptorIter = descriptorRange.first; descriptorIter != descriptorRange.second; ++descriptorIter )
				{
					// Filter out pdesc with type other than the current one.
					if( descriptorIter->second.type == pDescriptorType )
					{
						// Return the ID of an existing descriptor.
						return descriptorIter->second.id;
					}
				}
			}

			// Create the descriptor. All functions inside the CreateProxy have the same name, so
			// we just call it here and the overloading (via matching desc type) will do the rest.
			auto descriptorHandle = createDescriptor( pInputDesc, inputDescHash, std::forward<TpArgs>( pArgs )... );

			// Save the ID for return, pdesc are moved into the map.
			const auto descriptorID = descriptorHandle->mDescriptorID;

			CachedDescriptorInfo descriptorInfo;
			descriptorInfo.id = descriptorID;
			descriptorInfo.type = pDescriptorType;

			// Associate new descriptor's ID with the hash of desc structure. When the same desc config
			// is requested later on, this descriptor will be returned instead of creating a new one.
			_descriptorInfoMap.insert( {inputDescHash, std::move(descriptorInfo)} );

			// Save the descriptor itself.
			pDescriptorMap[descriptorID] = descriptorHandle;

			// Return the ID.
			return descriptorHandle;
		}

	private:
		using DescriptorInfoMap = std::unordered_multimap<PipelineDescriptorHash, CachedDescriptorInfo>;
		using CachedBlendStateDescriptorMap = std::unordered_map<PipelineDescriptorID, BlendStateDescriptorHandle >;
		using CachedDepthStencilStateDescriptorMap = std::unordered_map<PipelineDescriptorID, DepthStencilStateDescriptorHandle>;
		using CachedGraphicsShaderLinkageDescriptorMap = std::unordered_map<PipelineDescriptorID, GraphicsShaderLinkageDescriptorHandle>;
		using CachedIAVertexInputLayoutDescriptorMap = std::unordered_map<PipelineDescriptorID, IAVertexInputLayoutDescriptorHandle>;
		using CachedIAVertexStreamFormatDescriptorMap = std::unordered_map<PipelineDescriptorID, IAVertexStreamDescriptorHandle>;
		using CachedRasterizerStateDescriptorMap = std::unordered_map<PipelineDescriptorID, RasterizerStateDescriptorHandle>;

		DescriptorInfoMap _descriptorInfoMap;
		CachedBlendStateDescriptorMap _cachedBlendStateDescriptors;
		CachedDepthStencilStateDescriptorMap _cachedDepthStencilStateDescriptors;
		CachedGraphicsShaderLinkageDescriptorMap _cachedGraphicsShaderLinkageDescriptors;
		CachedIAVertexInputLayoutDescriptorMap _cachedIAVertexInputLayoutDescriptors;
		CachedIAVertexStreamFormatDescriptorMap _cachedIAVertexStreamFormatDescriptors;
		CachedRasterizerStateDescriptorMap _cachedRasterizerStateDescriptor;
	};

	template <typename TpInputDesc>
	inline PipelineDescriptorHash computePipelineDescriptorHash( const TpInputDesc & pInputDesc )
	{
		return HashGen::compute<EHashAlgo::FNV1A>( pInputDesc );
	}

	/// @brief Dummy trait type for the descriptor cache. List all the things required for the traits to provide.
	class TS3_ATTR_MAYBE_UNUSED GraphicsPipelineDescriptorFactory
	{
	public:
		using GPUDeviceType = void;

		template <typename... TpArgs>
		static BlendStateDescriptorHandle createBlendStateDescriptor( GPUDevice &, const BlendConfig &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static DepthStencilStateDescriptorHandle createDepthStencilStateDescriptor( GPUDevice &, const DepthStencilConfig &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static GraphicsShaderLinkageDescriptorHandle createGraphicsShaderLinkageDescriptor( GPUDevice &, const GraphicsShaderSet &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static IAVertexInputLayoutDescriptorHandle createIAVertexInputLayoutDescriptor( GPUDevice &, const IAVertexInputFormat &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static IAVertexStreamDescriptorHandle createIAVertexStreamDescriptor( GPUDevice &, const IAVertexStreamConfiguration &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static RasterizerStateDescriptorHandle createRasterizerStateDescriptor( GPUDevice &, const RasterizerConfig &, TpArgs && ... )
		{}
	};

	class IPipelineDescriptorCache
	{
	public:
	};

	// The cache.
	template <typename TpFactory>
	class TS3_ATTR_MAYBE_UNUSED GraphicsPipelineDescriptorCache : public GPUDeviceChildObject
	{
	public:
		using GPUDeviceType = typename TpFactory::GPUDeviceType;

		explicit GraphicsPipelineDescriptorCache( GPUDeviceType & pGPUDevice )
		: GPUDeviceChildObject( pGPUDevice )
		{}

		template <typename... TpArgs>
		PipelineDescriptorID createBlendStateDescriptor( const BlendConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::BlendState, pConfig, _cachedBlendStateDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		PipelineDescriptorID createDepthStencilStateDescriptor( const DepthStencilConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::DepthStencilState, pConfig, _cachedDepthStencilStateDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		PipelineDescriptorID createGraphicsShaderLinkageDescriptor( const GraphicsShaderSet & pShaderSet, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::ShaderLinkage, pShaderSet, _cachedGraphicsShaderLinkageDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		PipelineDescriptorID createIAVertexInputLayoutDescriptor( const IAVertexInputFormat & pInputFormat, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::IAVertexInputLayout, pInputFormat, _cachedIAVertexInputLayoutDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		PipelineDescriptorID createIAVertexStreamDescriptor( const IAVertexStreamConfiguration & pStreamConfiguration, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::IAVertexStream, pStreamConfiguration, _cachedIAVertexStreamFormatDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		PipelineDescriptorID createRasterizerStateDescriptor( const RasterizerConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::RasterizerState, pConfig, _cachedRasterizerStateDescriptor, std::forward<TpArgs>( pArgs )... );
		}

	private:
		struct CachedDescriptorInfo
		{
			PipelineDescriptorID id = PIPELINE_DESCRIPTOR_ID_INVALID;
			EGraphicsPipelineDescriptorType type = EGraphicsPipelineDescriptorType::Unknown;
		};

		template <typename... TpArgs>
		BlendStateDescriptorHandle createDescriptor( const BlendConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createBlendStateDescriptor( mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		DepthStencilStateDescriptorHandle createDescriptor( const DepthStencilConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createDepthStencilStateDescriptor(  mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		GraphicsShaderLinkageDescriptorHandle createDescriptor( const GraphicsShaderSet & pShaderSet, TpArgs && ...pArgs )
		{
			return TpFactory::createGraphicsShaderLinkageDescriptor(  mGPUDevice, pShaderSet, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		IAVertexInputLayoutDescriptorHandle createDescriptor( const IAVertexInputFormat & pInputFormat, TpArgs && ...pArgs )
		{
			return TpFactory::createIAVertexInputLayoutDescriptor(  mGPUDevice, pInputFormat, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		IAVertexStreamDescriptorHandle createDescriptor( const IAVertexStreamConfiguration & pStreamConfiguration, TpArgs && ...pArgs )
		{
			return TpFactory::createIAVertexStreamDescriptor(  mGPUDevice, pStreamConfiguration, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		RasterizerStateDescriptorHandle createDescriptor( const RasterizerConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createRasterizerStateDescriptor(  mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		// Thanks to additional pass-through createDescriptor() functions, we can have this function as a universal template.
		// It only needs an input desc and a descriptor type-specific map where pdesc of that type are being stored.
		template <typename TpInputDesc, typename TpDescriptorMap, typename... TpArgs>
		PipelineDescriptorID _createDescriptor( EGraphicsPipelineDescriptorType pDescriptorType,
		                                            const TpInputDesc & pInputDesc,
		                                            TpDescriptorMap & pDescriptorMap,
		                                            TpArgs && ...pArgs )
		{
			// First, compute a hash of the specified input desc structure. These hashes are stored in the separate map
			// as keys mapped to all pdesc (their IDs) created from the given desc structure. This allows us to
			// determine if there is already a descriptor matching the specified configuration.
			auto inputDescHash = computePipelineDescriptorHash( pInputDesc );

			// Get all pdesc with matching source hash. This properly handles 1:N situations, because we can
			// produce same binary data even for two different descriptor types (and it can get worse over time).
			auto descriptorRange = _descriptorInfoMap.equal_range( inputDescHash );

			if( descriptorRange.first != _descriptorInfoMap.end() )
			{
				for( auto descriptorIter = descriptorRange.first; descriptorIter != descriptorRange.second; ++descriptorIter )
				{
					// Filter out pdesc with type other than the current one.
					if( descriptorIter->second.type == pDescriptorType )
					{
						// Return the ID of an existing descriptor.
						return descriptorIter->second.id;
					}
				}
			}

			// Create the descriptor. All functions inside the CreateProxy have the same name, so
			// we just call it here and the overloading (via matching desc type) will do the rest.
			auto descriptorHandle = createDescriptor( pInputDesc, inputDescHash, std::forward<TpArgs>( pArgs )... );

			// Save the ID for return, pdesc are moved into the map.
			const auto descriptorID = descriptorHandle->mDescriptorID;

			CachedDescriptorInfo descriptorInfo;
			descriptorInfo.id = descriptorID;
			descriptorInfo.type = pDescriptorType;

			// Associate new descriptor's ID with the hash of desc structure. When the same desc config
			// is requested later on, this descriptor will be returned instead of creating a new one.
			_descriptorInfoMap.insert( {inputDescHash, std::move(descriptorInfo)} );

			// Save the descriptor itself.
			pDescriptorMap[descriptorID] = descriptorHandle;

			// Return the ID.
			return descriptorHandle;
		}

	private:
		using DescriptorInfoMap = std::unordered_multimap<PipelineDescriptorHash, CachedDescriptorInfo>;
		using CachedBlendStateDescriptorMap = std::unordered_map<PipelineDescriptorID, BlendStateDescriptorHandle >;
		using CachedDepthStencilStateDescriptorMap = std::unordered_map<PipelineDescriptorID, DepthStencilStateDescriptorHandle>;
		using CachedGraphicsShaderLinkageDescriptorMap = std::unordered_map<PipelineDescriptorID, GraphicsShaderLinkageDescriptorHandle>;
		using CachedIAVertexInputLayoutDescriptorMap = std::unordered_map<PipelineDescriptorID, IAVertexInputLayoutDescriptorHandle>;
		using CachedIAVertexStreamFormatDescriptorMap = std::unordered_map<PipelineDescriptorID, IAVertexStreamDescriptorHandle>;
		using CachedRasterizerStateDescriptorMap = std::unordered_map<PipelineDescriptorID, RasterizerStateDescriptorHandle>;

		DescriptorInfoMap _descriptorInfoMap;
		CachedBlendStateDescriptorMap _cachedBlendStateDescriptors;
		CachedDepthStencilStateDescriptorMap _cachedDepthStencilStateDescriptors;
		CachedGraphicsShaderLinkageDescriptorMap _cachedGraphicsShaderLinkageDescriptors;
		CachedIAVertexInputLayoutDescriptorMap _cachedIAVertexInputLayoutDescriptors;
		CachedIAVertexStreamFormatDescriptorMap _cachedIAVertexStreamFormatDescriptors;
		CachedRasterizerStateDescriptorMap _cachedRasterizerStateDescriptor;
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__
