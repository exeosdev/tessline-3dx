
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__
#define __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__

#include "graphicsPipelineImmutableState.h"
#include <unordered_map>

#define ts3GpaStateSubCacheDeclare( pStateName ) \
	using pStateName##StateSubCache = PipelineImmutableStateSubCache<pStateName##ImmutableState>; \
	mutable pStateName##StateSubCache _stateSubCache##pStateName;

#define ts3GpaStateSubCacheInit( pStateName, pFactory ) \
	_stateSubCache##pStateName( pFactory )

#define ts3GpaStateSubCacheDefineProxy( pStateName ) \
	template <> \
	inline PipelineImmutableStateSubCache<pStateName##ImmutableState> & PipelineImmutableStateCache::_subCacheProxy<pStateName##ImmutableState>() const \
	{ \
		return _stateSubCache##pStateName; \
	}

namespace ts3::gpuapi
{

	/// @brief An extra wrapper for data required to create an immutable state object.
	/// Used to pack the data together and avoid changing function signatures when a change is made.
	template <typename TInputDesc>
	struct PipelineImmutableStateCreateInfo
	{
		/// A unique name given to an immutable state object.
		UniqueGPUObjectName uniqueName;

		/// Data needed to initialize the state. Different for every state type.
		std::reference_wrapper<const TInputDesc> inputDesc;

		PipelineImmutableStateCreateInfo()
		{}

		PipelineImmutableStateCreateInfo( UniqueGPUObjectName pUniqueName, const TInputDesc & pInputDesc )
		: uniqueName( std::move( pUniqueName ) )
		, inputDesc( std::ref( pInputDesc ) )
		{}
	};

	/// @brief A "sub-cache" used by the actual cache. Manages single state type.
	template <typename TState>
	class PipelineImmutableStateSubCache
	{
	public:
		PipelineImmutableStateSubCache( PipelineImmutableStateFactory & pFactory );
		~PipelineImmutableStateSubCache() = default;

		TS3_ATTR_NO_DISCARD TState * getState( const UniqueGPUObjectID & pStateObjectID ) const
		{
			const auto existingStateRef = _cachedStates.find( pStateObjectID.asValue() );

			if( existingStateRef != _cachedStates.end() )
			{
				const auto & cachedStateData = existingStateRef->second;
				return cachedStateData.immutableStateObject.get();
			}

			return nullptr;
		}

		TS3_ATTR_NO_DISCARD TState * getState( const UniqueGPUObjectName & pStateObjectName ) const
		{
			const auto uniqueID = generateUniqueGPUObjectID( pStateObjectName );
			return getState( uniqueID );
		}

		template <typename TCreateData, typename... TArgs>
		TState * createState( const PipelineImmutableStateCreateInfo<TCreateData> & pCreateInfo, TArgs && ...pArgs )
		{
			UniqueGPUObjectID stateUniqueID{};

			const auto controlInputHash = hashCompute<EHashAlgo::FNV1A64>( pCreateInfo.inputDesc.get() );

			if( !pCreateInfo.uniqueName.empty() )
			{
				stateUniqueID = generateUniqueGPUObjectID( pCreateInfo.uniqueName );

				const auto existingStateRef = _cachedStates.find( stateUniqueID.asValue() );
				if( existingStateRef != _cachedStates.end() )
				{
					const auto & cachedStateData = existingStateRef->second;

					if( cachedStateData.controlInputHash != controlInputHash )
					{
						// The same ID has been generated for another immutable state of the same type. Fix it.
						ts3Throw( 0 );
					}

					const auto & stateHandle = cachedStateData.immutableStateObject;
					// Return the pointer to the existing state.
					return stateHandle.get();
				}
			}

			auto newImmutableState = _stateFactoryAdapter.createState( pCreateInfo.inputDesc );

			if( !newImmutableState )
			{
				return nullptr;
			}

			if( pCreateInfo.uniqueName.empty() )
			{
				const auto uniqueName = std::to_string( reinterpret_cast<ptrdiff_t>( newImmutableState.get() ) );
				stateUniqueID = generateUniqueGPUObjectID( uniqueName );
			}

			CachedStateData cachedStateData{};
			cachedStateData.controlInputHash = controlInputHash;
			cachedStateData.immutableStateObject = newImmutableState;

			_cachedStates[stateUniqueID] = std::move( cachedStateData );

			return newImmutableState.get();
		}

		void reset()
		{
			_cachedStates.clear();
		}

	private:
		/// The data stored internally for every cached state object.
		struct CachedStateData
		{
			using ControlInputHash = HashObject<EHashAlgo::FNV1A64>;
			/// Control hash which is a hash of the inputDesc (passed inside the createInfo struct).
			ControlInputHash controlInputHash;
			/// The actual immutable state.
			GpaHandle<TState> immutableStateObject;
		};

		using cache_map_key_t = UniqueGPUObjectID::ValueType;
		using CachedStateMap = std::unordered_map<cache_map_key_t, CachedStateData>;

	private:
		/// Adapter for the state factory. Allows the cache to simply call createState() for every state type.
		PipelineImmutableStateFactoryAdapter _stateFactoryAdapter;
		/// The actual map which serves as a storage for cached objects.
		CachedStateMap _cachedStates;
	};

	/// @brief
	class PipelineImmutableStateCache
	{
	public:
		PipelineImmutableStateCache( PipelineImmutableStateFactory & pFactory )
		: ts3GpaStateSubCacheInit( Blend, pFactory )
		, ts3GpaStateSubCacheInit( DepthStencil, pFactory )
		, ts3GpaStateSubCacheInit( GraphicsShaderLinkage, pFactory )
		, ts3GpaStateSubCacheInit( IAInputLayout, pFactory )
		, ts3GpaStateSubCacheInit( IAVertexStream, pFactory )
		, ts3GpaStateSubCacheInit( Rasterizer, pFactory )
		, ts3GpaStateSubCacheInit( RenderTargetLayout, pFactory )
		, ts3GpaStateSubCacheInit( RenderTargetBinding, pFactory )
		, ts3GpaStateSubCacheInit( RenderPass, pFactory )
		{}

		template <typename TState>
		TS3_ATTR_NO_DISCARD TState * getState( const UniqueGPUObjectID & pStateObjectID ) const
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.getState( pStateObjectID );
		}

		template <typename TState>
		TS3_ATTR_NO_DISCARD TState * getState( const UniqueGPUObjectName & pStateObjectName ) const
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.getState( pStateObjectName );
		}

		template <typename TState, typename TCreateData, typename... TArgs>
		TState * createState( const PipelineImmutableStateCreateInfo<TCreateData> & pCreateInfo, TArgs && ...pArgs )
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.createState( pCreateInfo, std::forward<TArgs>( pArgs )... );
		}

		template <typename TState>
		void resetSubCache()
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.reset();
		}

	private:
		template <typename TState>
		TS3_ATTR_NO_DISCARD PipelineImmutableStateSubCache<TState> & _subCacheProxy() const;

	private:
		ts3GpaStateSubCacheDeclare( Blend );
		ts3GpaStateSubCacheDeclare( DepthStencil );
		ts3GpaStateSubCacheDeclare( GraphicsShaderLinkage );
		ts3GpaStateSubCacheDeclare( IAInputLayout );
		ts3GpaStateSubCacheDeclare( IAVertexStream );
		ts3GpaStateSubCacheDeclare( Rasterizer );
		ts3GpaStateSubCacheDeclare( RenderTargetLayout );
		ts3GpaStateSubCacheDeclare( RenderTargetBinding );
		ts3GpaStateSubCacheDeclare( RenderPass );
	};

	ts3GpaStateSubCacheDefineProxy( Blend );
	ts3GpaStateSubCacheDefineProxy( DepthStencil );
	ts3GpaStateSubCacheDefineProxy( GraphicsShaderLinkage );
	ts3GpaStateSubCacheDefineProxy( IAInputLayout );
	ts3GpaStateSubCacheDefineProxy( IAVertexStream );
	ts3GpaStateSubCacheDefineProxy( Rasterizer );
	ts3GpaStateSubCacheDefineProxy( RenderTargetLayout );
	ts3GpaStateSubCacheDefineProxy( RenderTargetBinding );
	ts3GpaStateSubCacheDefineProxy( RenderPass );

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__
