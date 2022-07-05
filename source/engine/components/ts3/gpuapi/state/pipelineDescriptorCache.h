
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__
#define __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__

#include "commonGPUStateDefs.h"
#include <unordered_map>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( BlendStateDescriptor );
	ts3DeclareClassHandle( DepthStencilStateDescriptor );
	ts3DeclareClassHandle( GraphicsShaderBindingDescriptor );
	ts3DeclareClassHandle( IAVertexFormatDescriptor );
	ts3DeclareClassHandle( IAVertexStreamDescriptor );
	ts3DeclareClassHandle( RasterizerStateDescriptor );

	struct BlendConfig;
	struct DepthStencilConfig;
	struct GraphicsShaderSet;
	struct IAVertexInputAttributeArray;
	struct IAVertexStreamBufferBinding;
	struct RasterizerConfig;

	template <typename TpInputDesc>
	inline pipeline_descriptor_input_hash_t computePipelineDescriptorInputDescHash( const TpInputDesc & pInputDesc )
	{
		return ts3::HashGen::compute<ts3::HashAlgo::FNV1A>( pInputDesc );
	}

	/// @brief Dummy trait type for the descriptor cache. List all the things required for the traits to provide.
	class TS3_CLASS_MAYBE_UNUSED GraphicsPipelineDescriptorFactory
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
		static GraphicsShaderBindingDescriptorHandle createGraphicsShaderBindingDescriptor( GPUDevice &, const GraphicsShaderSet &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static IAVertexFormatDescriptorHandle createIAVertexFormatDescriptor( GPUDevice &, const IAVertexInputAttributeArray &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static IAVertexStreamDescriptorHandle createIAVertexStreamDescriptor( GPUDevice &, const IAVertexStreamBufferBinding &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static RasterizerStateDescriptorHandle createRasterizerStateDescriptor( GPUDevice &, const RasterizerConfig &, TpArgs && ... )
		{}
	};

	// The cache.
	template <typename TpFactory>
	class TS3_CLASS_MAYBE_UNUSED GraphicsPipelineDescriptorCache : public GPUDeviceChildObject
	{
	public:
		using GPUDeviceType = typename TpFactory::GPUDeviceType;

		explicit GraphicsPipelineDescriptorCache( GPUDeviceType & pGPUDevice )
		: GPUDeviceChildObject( pGPUDevice )
		{}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createBlendStateDescriptor( const BlendConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::BlendState, pConfig, _cachedBlendStateDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createDepthStencilStateDescriptor( const DepthStencilConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::DepthStencilState, pConfig, _cachedDepthStencilStateDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createGraphicsShaderBindingDescriptor( const GraphicsShaderSet & pShaderSet, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::ShaderBinding, pShaderSet, _cachedGraphicsShaderBindingDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createIAVertexFormatDescriptor( const IAVertexInputAttributeArray & pAttributeArray, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::IAVertexFormat, pAttributeArray, _cachedIAVertexFormatDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createIAVertexStreamDescriptor( const IAVertexStreamBufferBinding & pBufferBinding, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::IAVertexStream, pBufferBinding, _cachedIAVertexStreamFormatDescriptors, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_descriptor_id_t createRasterizerStateDescriptor( const RasterizerConfig & pConfig, TpArgs && ...pArgs )
		{
			return _createDescriptor( EGraphicsPipelineDescriptorType::RasterizerState, pConfig, _cachedRasterizerStateDescriptor, std::forward<TpArgs>( pArgs )... );
		}

	private:
		struct CachedDescriptorInfo
		{
			pipeline_descriptor_id_t id = CX_PIPELINE_DESCRIPTOR_ID_INVALID;
			EGraphicsPipelineDescriptorType type = EGraphicsPipelineDescriptorType::Unknown;
		};

		template <typename... TpArgs>
		BlendStateDescriptorHandle _createDescriptorProxy( const BlendConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createBlendStateDescriptor( mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		DepthStencilStateDescriptorHandle _createDescriptorProxy( const DepthStencilConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createDepthStencilStateDescriptor(  mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		GraphicsShaderBindingDescriptorHandle _createDescriptorProxy( const GraphicsShaderSet & pShaderSet, TpArgs && ...pArgs )
		{
			return TpFactory::createGraphicsShaderBindingDescriptor(  mGPUDevice, pShaderSet, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		IAVertexFormatDescriptorHandle _createDescriptorProxy( const IAVertexInputAttributeArray & pAttributeArray, TpArgs && ...pArgs )
		{
			return TpFactory::createIAVertexFormatDescriptor(  mGPUDevice, pAttributeArray, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		IAVertexStreamDescriptorHandle _createDescriptorProxy( const IAVertexStreamBufferBinding & pBufferBinding, TpArgs && ...pArgs )
		{
			return TpFactory::createIAVertexStreamDescriptor(  mGPUDevice, pBufferBinding, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		RasterizerStateDescriptorHandle _createDescriptorProxy( const RasterizerConfig & pConfig, TpArgs && ...pArgs )
		{
			return TpFactory::createRasterizerStateDescriptor(  mGPUDevice, pConfig, std::forward<TpArgs>( pArgs )... );
		}

		// Thanks to additional pass-through createDescriptor() functions, we can have this function as a universal template.
		// It only needs an input desc and a descriptor type-specific map where descriptors of that type are being stored.
		template <typename TpInputDesc, typename TpDescriptorMap, typename... TpArgs>
		pipeline_descriptor_id_t _createDescriptor( EGraphicsPipelineDescriptorType pDescriptorType,
		                                            const TpInputDesc & pInputDesc,
		                                            TpDescriptorMap & pDescriptorMap,
		                                            TpArgs && ...pArgs )
		{
			// First, compute a hash of the specified input desc structure. These hashes are stored in the separate map
			// as keys mapped to all descriptors (their IDs) created from the given desc structure. This allows us to
			// determine if there is already a descriptor matching the specified configuration.
			auto inputDescHash = computePipelineDescriptorInputDescHash( pInputDesc );

			// Get all descriptors with matching source hash. This properly handles 1:N situations, because we can
			// produce same binary data even for two different descriptor types (and it can get worse over time).
			auto descriptorRange = _descriptorInfoMap.equal_range( inputDescHash );

			if( descriptorRange.first != _descriptorInfoMap.end() )
			{
				for( auto descriptorIter = descriptorRange.first; descriptorIter != descriptorRange.second; ++descriptorIter )
				{
					// Filter out descriptors with type other than the current one.
					if( descriptorIter->second.type == pDescriptorType )
					{
						// Return the ID of an existing descriptor.
						return descriptorIter->second.id;
					}
				}
			}

			// Create the descriptor. All functions inside the CreateProxy have the same name, so
			// we just call it here and the overloading (via matching desc type) will do the rest.
			auto descriptorHandle = _createDescriptorProxy( pInputDesc, inputDescHash, std::forward<TpArgs>( pArgs )... );

			// Save the ID for return, descriptors are moved into the map.
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
		using DescriptorInfoMap = std::unordered_multimap<pipeline_descriptor_input_hash_t, CachedDescriptorInfo>;
		using CachedBlendStateDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, BlendStateDescriptorHandle >;
		using CachedDepthStencilStateDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, DepthStencilStateDescriptorHandle>;
		using CachedGraphicsShaderBindingDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, GraphicsShaderBindingDescriptorHandle>;
		using CachedIAVertexFormatDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, IAVertexFormatDescriptorHandle>;
		using CachedIAVertexStreamFormatDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, IAVertexStreamDescriptorHandle>;
		using CachedRasterizerStateDescriptorMap = std::unordered_map<pipeline_descriptor_id_t, RasterizerStateDescriptorHandle>;

		DescriptorInfoMap _descriptorInfoMap;
		CachedBlendStateDescriptorMap _cachedBlendStateDescriptors;
		CachedDepthStencilStateDescriptorMap _cachedDepthStencilStateDescriptors;
		CachedGraphicsShaderBindingDescriptorMap _cachedGraphicsShaderBindingDescriptors;
		CachedIAVertexFormatDescriptorMap _cachedIAVertexFormatDescriptors;
		CachedIAVertexStreamFormatDescriptorMap _cachedIAVertexStreamFormatDescriptors;
		CachedRasterizerStateDescriptorMap _cachedRasterizerStateDescriptor;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_DESCRIPTOR_CACHE_H__
