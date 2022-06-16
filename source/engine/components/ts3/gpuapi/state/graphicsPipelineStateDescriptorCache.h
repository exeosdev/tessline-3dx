
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_DESCRIPTOR_CACHE_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_DESCRIPTOR_CACHE_H__

#include "pipelineStateDesc.h"
#include <map>
#include <unordered_map>

namespace ts3::gpuapi
{

	// Implementation of a simple cache for graphics pipeline state descriptors used by drivers without monolithic PSOs.
	// GpuAPI layer, in order to be as forward-compatible as possible, uses that approach itself (pipeline state is
	// monolithic and mutable only via switching the PSO). For older APIs this can introduce significant memory overhead
	// and a lot of unnecessary API calls (especially in OpenGL). This cache is designed to solve these issues.
	// Graphics pipeline has four descriptors:
	// - blend
	// - depth/stencil
	// - rasterizer
	// - vertex input format
	// The cache ensures, that for any given combination of settings, only a single instance of descriptor is created.
	// For example, if two PSOs share the same blend configuration, they will share blend descriptor representing that
	// state. This is beneficial in two ways as mentioned above:
	// 1) In DX11, we can use the same ID3D11BlendState object.
	// 2) In OpenGL we can check for matching blend configuration and skip all glBlend*() calls entirely.
	// Although OpenGL doesn't provide dedicated state objects like D3D does, it *does* have VAOs for VertexInputFormat
	// state and we also reuse those. This enables us to skip binding VAO if input format remains the same between PSOs.

	/// @brief Dummy trait type for the descriptor cache. List all the things required for the traits to provide.
	struct TS3_CLASS_MAYBE_UNUSED GraphicsPipelineStateDescriptorCacheTraits
	{
		// We need a typedefs for actual descriptors (since they are driver-specific).
		// All four types are required to be present in a trait type.

		using BlendDescriptorType = void;
		using DepthStencilDescriptorType = void;
		using RasterizerDescriptorType = void;
		using VertexInputFormatDescriptorType = void;

		// We need methods for creating the descriptors using provided desc structures (and potentially some extra data).
		// The only requirements for those are:
		// - names must be as these below
		// - they must be callable using a static call syntax (e.g. MyTraitType::createBlendDescriptor(...))

		template <typename... TpArgs>
		static BlendDescriptorType createBlendDescriptor( const BlendConfigDesc &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static DepthStencilDescriptorType createDepthStencilDescriptor( const DepthStencilConfigDesc &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static RasterizerDescriptorType createRasterizerDescriptor( const RasterizerConfigDesc &, TpArgs && ... )
		{}

		template <typename... TpArgs>
		static VertexInputFormatDescriptorType createVertexInputFormatDescriptor( const VertexInputFormatDesc &, TpArgs && ... )
		{}
	};

	// The cache.
	template <typename TpTraits>
	class GraphicsPipelineStateDescriptorCache
	{
	public:
		enum class CachedDescriptorType : uint32
		{
			Blend,
			DepthStencil,
			Rasterizer,
			VertexInputFormat
		};

		using Traits = TpTraits;

		using BlendDescriptorType = typename TpTraits::BlendDescriptorType;
		using DepthStencilDescriptorType = typename TpTraits::DepthStencilDescriptorType;
		using RasterizerDescriptorType = typename TpTraits::RasterizerDescriptorType;
		using VertexInputFormatDescriptorType = typename TpTraits::VertexInputFormatDescriptorType;

	public:
		GraphicsPipelineStateDescriptorCache() = default;

		// Public create API. Descriptors are usually created by a GPUDevice (which also owns the cache).

		template <typename... TpArgs>
		pipeline_state_descriptor_id_t createBlendDescriptor( const BlendConfigDesc & pBlendDesc, TpArgs && ...pArgs )
		{
			return _createDescriptor( CachedDescriptorType::Blend, pBlendDesc, _cachedBlendDescriptorMap, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_state_descriptor_id_t createDepthStencilDescriptor( const DepthStencilConfigDesc & pDepthStencilDesc, TpArgs && ...pArgs )
		{
			return _createDescriptor( CachedDescriptorType::DepthStencil, pDepthStencilDesc, _cachedDepthStencilDescriptorMap, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_state_descriptor_id_t createRasterizerDescriptor( const RasterizerConfigDesc & pRasterizerDesc, TpArgs && ...pArgs )
		{
			return _createDescriptor( CachedDescriptorType::Rasterizer, pRasterizerDesc, _cachedRasterizerDescriptorMap, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		pipeline_state_descriptor_id_t createVertexInputFormatDescriptor( const VertexInputFormatDesc & pVertexInputFormatDesc, TpArgs && ...pArgs )
		{
			return _createDescriptor( CachedDescriptorType::VertexInputFormat, pVertexInputFormatDesc, _cachedVertexInputFormatDescriptorMap, std::forward<TpArgs>( pArgs )... );
		}

		// Public get API. Device fetches only descriptors created earlier, during PSO parsing/creation.
		// No additional checks - throw immediately if an unknown ID is specified, this should never happen.

		const BlendDescriptorType & getBlendDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
		{
			return _cachedBlendDescriptorMap.at( pDescriptorID );
		}

		const DepthStencilDescriptorType & getDepthStencilDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
		{
			return _cachedDepthStencilDescriptorMap.at( pDescriptorID );
		}

		const RasterizerDescriptorType & getRasterizerDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
		{
			return _cachedRasterizerDescriptorMap.at( pDescriptorID );
		}

		const VertexInputFormatDescriptorType & getVertexInputFormatDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
		{
			return _cachedVertexInputFormatDescriptorMap.at( pDescriptorID );
		}

	private:
		struct CachedDescriptorInfo
		{
			CachedDescriptorType type;
			pipeline_state_descriptor_id_t id;
		};

		// Thanks to additional pass-through createDescriptor() functions, we can have this function as a universal template.
		// It only needs an input desc and a descriptor type-specific map where descriptors of that type are being stored.
		template <typename TpInputDesc, typename TpDescriptorCache, typename... TpArgs>
		pipeline_state_descriptor_id_t _createDescriptor( CachedDescriptorType pDescriptorType,
		                                                  const TpInputDesc & pInputDesc,
		                                                  TpDescriptorCache & pDescriptorCache,
		                                                  TpArgs && ...pArgs )
		{
			// First, compute a hash of the specified input desc structure. These hashes are stored in the separate map
			// as keys mapped to all descriptors (their IDs) created from the given desc structure. This allows us to
			// determine if there is already a descriptor matching the specified configuration.
			auto inputDescHash = computePipelineInputDescHash( pInputDesc );

			// Get all descriptors with matching source hash. This properly handles 1:N situations, because we can
			// produce same binary data even for two different descriptor types (and it can get worse over time).
			auto existingDescriptorRange = _descriptorIDMap.equal_range( inputDescHash );

			if( existingDescriptorRange.first != _descriptorIDMap.end() )
			{
				for( auto descriptorIter = existingDescriptorRange.first; descriptorIter != existingDescriptorRange.first; ++descriptorIter )
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
			auto descriptor = _createDescriptorProxy( pInputDesc, inputDescHash, std::forward<TpArgs>( pArgs )... );

			// Save the ID for return, descriptors are moved into the map.
			const auto descriptorID = descriptor.descriptorID;

			CachedDescriptorInfo descriptorInfo;
			descriptorInfo.type = pDescriptorType;
			descriptorInfo.id = descriptorID;

			// Associate new descriptor's ID with the hash of desc structure. When the same desc config
			// is requested later on, this descriptor will be returned instead of creating a new one.
			_descriptorIDMap.insert( typename decltype( _descriptorIDMap )::value_type( inputDescHash, std::move( descriptorInfo ) ) );

			// Save the descriptor itself.
			pDescriptorCache[descriptorID] = std::move( descriptor );

			// Return the ID.
			return descriptorID;
		}

		template <typename... TpArgs>
		static BlendDescriptorType _createDescriptorProxy( const BlendConfigDesc & pBlendDesc, TpArgs && ...pArgs )
		{
			return TpTraits::createBlendDescriptor( pBlendDesc, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		static DepthStencilDescriptorType _createDescriptorProxy( const DepthStencilConfigDesc & pDepthStencilDesc, TpArgs && ...pArgs )
		{
			return TpTraits::createDepthStencilDescriptor( pDepthStencilDesc, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		static RasterizerDescriptorType _createDescriptorProxy( const RasterizerConfigDesc & pRasterizerDesc, TpArgs && ...pArgs )
		{
			return TpTraits::createRasterizerDescriptor( pRasterizerDesc, std::forward<TpArgs>( pArgs )... );
		}

		template <typename... TpArgs>
		static VertexInputFormatDescriptorType _createDescriptorProxy( const VertexInputFormatDesc & pVertexInputFormatDesc, TpArgs && ...pArgs )
		{
			return TpTraits::createVertexInputFormatDescriptor( pVertexInputFormatDesc, std::forward<TpArgs>( pArgs )... );
		}

	private:
		using DescriptorIDMap = std::multimap<pipeline_input_desc_hash_t, CachedDescriptorInfo>;
		using CachedBlendDescriptorMap = std::unordered_map<pipeline_state_descriptor_id_t, BlendDescriptorType>;
		using CachedDepthStencilDescriptorMap = std::unordered_map<pipeline_state_descriptor_id_t, DepthStencilDescriptorType>;
		using CachedRasterizerDescriptorMap = std::unordered_map<pipeline_state_descriptor_id_t, RasterizerDescriptorType>;
		using CachedVertexInputFormatDescriptorMap = std::unordered_map<pipeline_state_descriptor_id_t, VertexInputFormatDescriptorType>;

		DescriptorIDMap _descriptorIDMap;
		CachedBlendDescriptorMap _cachedBlendDescriptorMap;
		CachedDepthStencilDescriptorMap _cachedDepthStencilDescriptorMap;
		CachedRasterizerDescriptorMap _cachedRasterizerDescriptorMap;
		CachedVertexInputFormatDescriptorMap _cachedVertexInputFormatDescriptorMap;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_STATE_DESCRIPTOR_CACHE_H__
