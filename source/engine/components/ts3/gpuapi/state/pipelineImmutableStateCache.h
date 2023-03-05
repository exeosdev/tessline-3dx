
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__
#define __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__

#include "graphicsPipelineImmutableState.h"
#include "graphicsShaderState.h"
#include <unordered_map>

namespace ts3::gpuapi
{

	template <typename TDescriptor>
	struct PipelineImmutableStateCreateInfo
	{
		UniqueGPUObjectName uniqueName;

		std::reference_wrapper<const TDescriptor> descriptor;

		PipelineImmutableStateCreateInfo()
		{}

		PipelineImmutableStateCreateInfo( UniqueGPUObjectName pUniqueName, const TDescriptor & pDescriptor )
		: uniqueName( std::move( pUniqueName ) )
		, descriptor( std::ref( pDescriptor ) )
		{}
	};

	/// @brief
	class PipelineImmutableStateCache
	{
	public:
		PipelineImmutableStateCache( PipelineImmutableStateFactory & pFactory );
		~PipelineImmutableStateCache();

		BlendImmutableState * getBlendState( const UniqueGPUObjectID & pDescriptorID ) const;
		BlendImmutableState * getBlendState( const UniqueGPUObjectName & pDescriptorName ) const;

		DepthStencilImmutableState * getDepthStencilState( const UniqueGPUObjectID & pDescriptorID ) const;
		DepthStencilImmutableState * getDepthStencilState( const UniqueGPUObjectName & pDescriptorName ) const;

		GraphicsShaderLinkageImmutableState * getGraphicsShaderLinkageState( const UniqueGPUObjectID & pDescriptorID ) const;
		GraphicsShaderLinkageImmutableState * getGraphicsShaderLinkageState( const UniqueGPUObjectName & pDescriptorName ) const;

		RasterizerImmutableState * getRasterizerState( const UniqueGPUObjectID & pDescriptorID ) const;
		RasterizerImmutableState * getRasterizerState( const UniqueGPUObjectName & pDescriptorName ) const;

		VertexInputLayoutImmutableState * getVertexInputLayoutState( const UniqueGPUObjectID & pDescriptorID ) const;
		VertexInputLayoutImmutableState * getVertexInputLayoutState( const UniqueGPUObjectName & pDescriptorName ) const;

		BlendImmutableState * createBlendState( const PipelineImmutableStateCreateInfo<BlendDescriptor> & pCreateInfo );

		DepthStencilImmutableState * createDepthStencilState( const PipelineImmutableStateCreateInfo<DepthStencilDescriptor> & pCreateInfo );

		GraphicsShaderLinkageImmutableState * createShaderLinkageState( const PipelineImmutableStateCreateInfo<GraphicsShaderSet> & pCreateInfo );

		RasterizerImmutableState * createRasterizerState( const PipelineImmutableStateCreateInfo<RasterizerDescriptor> & pCreateInfo );

		VertexInputLayoutImmutableState * createVertexInputLayoutState( const PipelineImmutableStateCreateInfo<VertexInputLayoutDescriptor> & pCreateInfo );

	private:
		BlendImmutableStateHandle _createProxy( const BlendDescriptor & pDescriptor )
		{
			return _stateFactory->createBlendState( pDescriptor );
		}

		DepthStencilImmutableStateHandle _createProxy( const DepthStencilDescriptor & pDescriptor )
		{
			return _stateFactory->createDepthStencilState( pDescriptor );
		}

		GraphicsShaderLinkageImmutableStateHandle _createProxy( const GraphicsShaderSet & pShaderSet )
		{
			return _stateFactory->createGraphicsShaderLinkageState( pShaderSet );
		}

		RasterizerImmutableStateHandle _createProxy( const RasterizerDescriptor & pDescriptor )
		{
			return _stateFactory->createRasterizerState( pDescriptor );
		}

		VertexInputLayoutImmutableStateHandle _createProxy( const VertexInputLayoutDescriptor & pDescriptor )
		{
			return _stateFactory->createVertexInputLayoutState( pDescriptor );
		}

		template <typename TState, typename TDescriptor, typename TDescriptorMap, typename... TArgs>
		TState * _createState( const PipelineImmutableStateCreateInfo<TDescriptor> & pCreateInfo, TDescriptorMap & pDescriptorMap, TArgs && ...pArgs )
		{
			const auto uniqueID = generateUniqueGPUObjectID( pCreateInfo.uniqueName );
			const auto controlInputHash = hashCompute<EHashAlgo::FNV1A64>( pCreateInfo.descriptor.get() );

			const auto existingStateRef = pDescriptorMap.find( uniqueID.asValue() );

			if( existingStateRef != pDescriptorMap.end() )
			{
				const auto & cachedStateData = existingStateRef.second;

				if( cachedStateData.controlInputHash != controlInputHash )
				{
					// The same ID has been generated for another immutable state of the same type. Fix it.
					ts3Throw( 0 );
				}

				const auto & stateHandle = cachedStateData.immutableStateObject;
				// Return the pointer to the existing state.
				return stateHandle.get();
			}

			const auto newImmutableState = _createProxy( pCreateInfo.descriptor );

			if( !newImmutableState )
			{
				return nullptr;
			}

			CachedStateData<TState> cachedStateData{};
			cachedStateData.controlInputHash = controlInputHash;
			cachedStateData.immutableStateObject = newImmutableState;

			pDescriptorMap[uniqueID] = std::move( cachedStateData );

			return newImmutableState.get();
		}

	private:
		template <typename TState>
		struct CachedStateData
		{
			using ControlInputHash = HashObject<EHashAlgo::FNV1A64>;
			ControlInputHash controlInputHash;
			GpaHandle<TState> immutableStateObject;
		};

		using cache_map_key_t = UniqueGPUObjectID::ValueType;

		using CachedBlendStateData = CachedStateData<BlendImmutableState>;
		using CachedDepthStencilStateData = CachedStateData<DepthStencilImmutableState>;
		using CachedGraphicsShaderLinkageStateData = CachedStateData<GraphicsShaderLinkageImmutableState>;
		using CachedRasterizerStateData = CachedStateData<RasterizerImmutableState>;
		using CachedVertexInputLayoutStateData = CachedStateData<VertexInputLayoutImmutableState>;

		using CachedBlendStateMap = std::unordered_map<cache_map_key_t, CachedBlendStateData>;
		using CachedDepthStencilStateMap = std::unordered_map<cache_map_key_t, CachedDepthStencilStateData>;
		using CachedGraphicsShaderLinkageStateMap = std::unordered_map<cache_map_key_t, CachedGraphicsShaderLinkageStateData>;
		using CachedRasterizerStateMap = std::unordered_map<cache_map_key_t, CachedRasterizerStateData>;
		using CachedVertexInputLayoutStateMap = std::unordered_map<cache_map_key_t, CachedVertexInputLayoutStateData>;

	private:
		PipelineImmutableStateFactory * _stateFactory;
		CachedBlendStateMap _cachedBlendStates;
		CachedDepthStencilStateMap _cachedDepthStencilStates;
		CachedGraphicsShaderLinkageStateMap _cachedGraphicsShaderLinkageStates;
		CachedRasterizerStateMap _cachedRasterizerStates;
		CachedVertexInputLayoutStateMap _cachedVertexInputLayoutStates;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__
