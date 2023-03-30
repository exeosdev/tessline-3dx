
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
		UniqueGPUObjectID uniqueID = cxdefs::GPU_OBJECT_ID_INVALID;

		/// A unique name given to an immutable state object.
		RefWrapper<const UniqueGPUObjectName> uniqueName;

		/// Data needed to initialize the state. Different for every state type.
		RefWrapper<const TInputDesc> inputDesc;
	};

	/// @brief A "sub-cache" used by the actual cache. Manages single state type.
	template <typename TState>
	class PipelineImmutableStateSubCache
	{
	public:
		PipelineImmutableStateSubCache( PipelineImmutableStateFactory & pFactory )
		: _stateFactoryAdapter( pFactory )
		{}

		~PipelineImmutableStateSubCache() = default;

		TS3_ATTR_NO_DISCARD GpaHandle<TState> getState( UniqueGPUObjectID pStateObjectID ) const noexcept
		{
			const auto existingStateRef = _cachedStates.find( pStateObjectID.asValue() );

			if( existingStateRef != _cachedStates.end() )
			{
				const auto & cachedStateData = existingStateRef->second;
				return cachedStateData.immutableStateObject;
			}

			return nullptr;
		}

		TS3_ATTR_NO_DISCARD GpaHandle<TState> getState( const UniqueGPUObjectName & pStateObjectName ) const noexcept
		{
			const auto uniqueID = generateUniqueGPUObjectID( pStateObjectName );
			return getState( uniqueID );
		}

		template <typename TCreateData, typename... TArgs>
		GpaHandle<TState> createState( PipelineImmutableStateCreateInfo<TCreateData> pCreateInfo, TArgs && ...pArgs )
		{
			const auto controlInputHash = hashCompute<EHashAlgo::FNV1A64>( pCreateInfo.inputDesc.get() );

			if( ( pCreateInfo.uniqueID == cxdefs::GPU_OBJECT_ID_AUTO ) && !pCreateInfo.uniqueName->empty() )
			{
				pCreateInfo.uniqueID = generateUniqueGPUObjectID( pCreateInfo.uniqueName );
			}

			if( cxdefs::isUniqueGPUObjectIDValid( pCreateInfo.uniqueID ) )
			{
				const auto existingStateRef = _cachedStates.find( pCreateInfo.uniqueID.asValue() );
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
					return stateHandle;
				}
			}

			auto newImmutableState = _stateFactoryAdapter.createState( pCreateInfo.inputDesc, std::forward<TArgs>( pArgs )... );

			if( !newImmutableState )
			{
				return nullptr;
			}

			if( !cxdefs::isUniqueGPUObjectIDValid( pCreateInfo.uniqueID ) )
			{
				pCreateInfo.uniqueID.idValue = reinterpret_cast<uint64>( newImmutableState.get() );
			}

			CachedStateData cachedStateData{};
			cachedStateData.controlInputHash = controlInputHash;
			cachedStateData.immutableStateObject = newImmutableState;

			_cachedStates[pCreateInfo.uniqueID] = std::move( cachedStateData );

			return newImmutableState;
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
		PipelineImmutableStateFactory & mStateFactory;

	public:
		PipelineImmutableStateCache( PipelineImmutableStateFactory & pFactory )
		: mStateFactory( pFactory )
		, ts3GpaStateSubCacheInit( Blend, mStateFactory )
		, ts3GpaStateSubCacheInit( DepthStencil, mStateFactory )
		, ts3GpaStateSubCacheInit( GraphicsShaderLinkage, mStateFactory )
		, ts3GpaStateSubCacheInit( IAInputLayout, mStateFactory )
		, ts3GpaStateSubCacheInit( IAVertexStream, mStateFactory )
		, ts3GpaStateSubCacheInit( Rasterizer, mStateFactory )
		, ts3GpaStateSubCacheInit( RenderTargetBinding, mStateFactory )
		, ts3GpaStateSubCacheInit( RenderPassConfiguration, mStateFactory )
		{}

		template <typename TState>
		TS3_ATTR_NO_DISCARD GpaHandle<TState> getState( UniqueGPUObjectID pStateObjectID ) const noexcept
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.getState( pStateObjectID );
		}

		template <typename TState>
		TS3_ATTR_NO_DISCARD GpaHandle<TState> getState( const UniqueGPUObjectName & pStateObjectName ) const noexcept
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.getState( pStateObjectName );
		}

		template <typename TState, typename TInputDesc, typename... TArgs>
		GpaHandle<TState> createState( UniqueGPUObjectID pUniqueID, const TInputDesc & pInputDesc, TArgs && ...pArgs )
		{
			PipelineImmutableStateCreateInfo<TInputDesc> createInfo{};
			createInfo.uniqueID = pUniqueID;
			createInfo.inputDesc = std::ref( pInputDesc );
			auto & subCache = _subCacheProxy<TState>();
			return subCache.createState( std::move( createInfo ), std::forward<TArgs>( pArgs )... );
		}

		template <typename TState, typename TInputDesc, typename... TArgs>
		GpaHandle<TState> createState( const UniqueGPUObjectName & pUniqueName, const TInputDesc & pInputDesc, TArgs && ...pArgs )
		{
			PipelineImmutableStateCreateInfo<TInputDesc> createInfo{};
			createInfo.uniqueName = std::ref( pUniqueName );
			createInfo.inputDesc = std::ref( pInputDesc );
			auto & subCache = _subCacheProxy<TState>();
			return subCache.createState( std::move( createInfo ), std::forward<TArgs>( pArgs )... );
		}

		template <typename TState>
		void resetSubCache()
		{
			auto & subCache = _subCacheProxy<TState>();
			return subCache.reset();
		}

		void reset( Bitmask<EPipelineImmutableStateTypeFlags> pResetMask = E_PIPELINE_IMMUTABLE_STATE_TYPE_MASK_ALL )
		{
			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_BLEND_BIT ) )
				_stateSubCacheBlend.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_DEPTH_STENCIL_BIT ) )
				_stateSubCacheDepthStencil.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_GRAPHICS_SHADER_LINKAGE_BIT ) )
				_stateSubCacheGraphicsShaderLinkage.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_IA_INPUT_LAYOUT_BIT ) )
				_stateSubCacheIAInputLayout.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_IA_VERTEX_STREAM_BIT ) )
				_stateSubCacheIAVertexStream.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RASTERIZER_BIT ) )
				_stateSubCacheRasterizer.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RENDER_TARGET_BINDING_BIT ) )
				_stateSubCacheRenderTargetBinding.reset();

			if( pResetMask.isSet( E_PIPELINE_IMMUTABLE_STATE_TYPE_FLAG_RENDER_PASS_BIT ) )
				_stateSubCacheRenderPassConfiguration.reset();
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
		ts3GpaStateSubCacheDeclare( RenderTargetBinding );
		ts3GpaStateSubCacheDeclare( RenderPassConfiguration );
	};

	ts3GpaStateSubCacheDefineProxy( Blend );
	ts3GpaStateSubCacheDefineProxy( DepthStencil );
	ts3GpaStateSubCacheDefineProxy( GraphicsShaderLinkage );
	ts3GpaStateSubCacheDefineProxy( IAInputLayout );
	ts3GpaStateSubCacheDefineProxy( IAVertexStream );
	ts3GpaStateSubCacheDefineProxy( Rasterizer );
	ts3GpaStateSubCacheDefineProxy( RenderTargetBinding );
	ts3GpaStateSubCacheDefineProxy( RenderPassConfiguration );

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_IMMUTABLE_STATE_CACHE_H__
