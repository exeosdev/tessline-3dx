
#include "DX11_gpuBuffer.h"
#include <ts3/gpuapiDX11/DX11_coreAPIProxy.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/DX11_commandList.h>

namespace ts3
{
namespace gpuapi
{

	DX11GPUBuffer::DX11GPUBuffer( DX11GPUDevice & pDX11GPUDevice,
	                              const ResourceMemoryInfo & pResourceMemory,
	                              const GPUBufferProperties & pBufferProperties,
	                              ComPtr<ID3D11Buffer> pD3D11Buffer )
    : GPUBuffer( pDX11GPUDevice, pResourceMemory, pBufferProperties )
    , mD3D11Buffer( std::move( pD3D11Buffer ) )
	{ }

	DX11GPUBuffer::~DX11GPUBuffer() = default;

	DX11GPUBufferHandle DX11GPUBuffer::create( DX11GPUDevice & pDX11GPUDevice,
	                                           const GPUBufferCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		if( !validateBufferCreateInfo( createInfo ) )
		{
			return nullptr;
		}

		auto dx11GPUBufferDesc = translateBufferDesc( createInfo );

		D3D11_BUFFER_DESC d3d11BufferDesc;
		d3d11BufferDesc.ByteWidth = trunc_numeric_cast<UINT>( createInfo.bufferSize );
		d3d11BufferDesc.BindFlags = dx11GPUBufferDesc.bindFlags;
		d3d11BufferDesc.CPUAccessFlags = dx11GPUBufferDesc.cpuAccessFlags;
		d3d11BufferDesc.MiscFlags = dx11GPUBufferDesc.resourceMiscFlags;
		d3d11BufferDesc.Usage = dx11GPUBufferDesc.usage;
		d3d11BufferDesc.StructureByteStride = 0;

		const void * initDataPtr = nullptr;

		if( createInfo.initDataDesc )
		{
			if( createInfo.initDataDesc.size < createInfo.bufferSize )
			{
				ts3DebugInterrupt();
				ts3DebugOutput( "Warn: init data must fill the whole resource. Buffer will be created with default content." );
			}
			else
			{
				initDataPtr = createInfo.initDataDesc.pointer;
			}
		}

		ComPtr<ID3D11Buffer> d3d11Buffer;
		HRESULT hResult = S_OK;

		if( !initDataPtr )
		{
			hResult = pDX11GPUDevice.mD3D11Device1->CreateBuffer( &d3d11BufferDesc, nullptr, d3d11Buffer.GetAddressOf() );
		}
		else
		{
			D3D11_SUBRESOURCE_DATA d3d11BufferInitData;
			d3d11BufferInitData.pSysMem = createInfo.initDataDesc.pointer;
			d3d11BufferInitData.SysMemPitch = 0;
			d3d11BufferInitData.SysMemSlicePitch = 0;

			hResult = pDX11GPUDevice.mD3D11Device1->CreateBuffer( &d3d11BufferDesc, &d3d11BufferInitData, d3d11Buffer.GetAddressOf() );
		}

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		d3d11Buffer->GetDesc( &d3d11BufferDesc );

		GPUBufferProperties bufferProperties;
		bufferProperties.byteSize = d3d11BufferDesc.ByteWidth;
		bufferProperties.resourceFlags = createInfo.resourceFlags;

		ResourceMemoryInfo bufferMemoryInfo;
		bufferMemoryInfo.sourceHeapRegion.offset = 0;
		bufferMemoryInfo.sourceHeapRegion.size = d3d11BufferDesc.ByteWidth;
		bufferMemoryInfo.baseAlignment = createInfo.memoryBaseAlignment;
		bufferMemoryInfo.memoryFlags = createInfo.memoryFlags;

		auto dx11Buffer = createDynamicInterfaceObject<DX11GPUBuffer>( pDX11GPUDevice,
		                                                               bufferMemoryInfo,
		                                                               bufferProperties,
		                                                               d3d11Buffer );

		return dx11Buffer;
	}

	bool DX11GPUBuffer::mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );

		auto d3d11MapMode = DX11CoreAPIProxy::translateDX11BufferMapFlags( pMapMode, mResourceMemory.memoryFlags );
		ts3DebugAssert( d3d11MapMode != 0 );

		D3D11_MAPPED_SUBRESOURCE mappedBufferInfo;
		auto hResult = d3d11DeviceContext1->Map( mD3D11Buffer.Get(), 0, d3d11MapMode, 0, &mappedBufferInfo );
		if( FAILED( hResult ) )
		{
			return false;
		}

		ResourceMappedMemory mappedMemoryInfo;
		mappedMemoryInfo.pointer = mappedBufferInfo.pData;
		mappedMemoryInfo.mappedRegion = pRegion;
		mappedMemoryInfo.memoryMapFlags = static_cast<EGPUMemoryMapFlags>( pMapMode );

		_dx11MapInfo.mappedSubresource = mappedBufferInfo;
		_dx11MapInfo.requestedMapMode = d3d11MapMode;

		setMappedMemory( mappedMemoryInfo );

		return true;
	}

	void DX11GPUBuffer::unmap( void * pCommandObject )
	{
		auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );

		d3d11DeviceContext1->Unmap( mD3D11Buffer.Get(), 0 );

		resetMappedMemory();
	}

	void DX11GPUBuffer::flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion )
	{
		auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );

		d3d11DeviceContext1->Unmap( mD3D11Buffer.Get(), 0 );

		auto hResult = d3d11DeviceContext1->Map( mD3D11Buffer.Get(), 0, _dx11MapInfo.requestedMapMode, 0, &( _dx11MapInfo.mappedSubresource ) );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
		}
	}

	void DX11GPUBuffer::invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion )
	{
		if( pRegion.size == mBufferProperties.byteSize )
		{
			auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );
			d3d11DeviceContext1->DiscardResource( mD3D11Buffer.Get() );
		}
	}

	void DX11GPUBuffer::updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );

		Bitmask<UINT> copyFlags = 0;
		if( ( pCopyDesc.sourceBufferRegion.size == mBufferProperties.byteSize ) || pCopyDesc.flags.isSet( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT ) )
		{
			copyFlags.set( D3D11_COPY_DISCARD );
		}

		D3D11_BOX sourceBufferBox;
		sourceBufferBox.top = 0;
		sourceBufferBox.bottom = 1;
		sourceBufferBox.front = 0;
		sourceBufferBox.back = 1;
		sourceBufferBox.left = trunc_numeric_cast<UINT>( pCopyDesc.sourceBufferRegion.offset );
		sourceBufferBox.right = trunc_numeric_cast<UINT>( pCopyDesc.sourceBufferRegion.offset + pCopyDesc.sourceBufferRegion.size );

		auto * targetBuffer = mD3D11Buffer.Get();
		auto targetBufferOffset = trunc_numeric_cast<UINT>( pCopyDesc.targetBufferOffset );
		auto * sourceBuffer = pSource.queryInterface<DX11GPUBuffer>()->mD3D11Buffer.Get();

		d3d11DeviceContext1->CopySubresourceRegion1( targetBuffer, 0, targetBufferOffset, 0, 0, sourceBuffer, 0, &sourceBufferBox, copyFlags );
	}

	void DX11GPUBuffer::updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		auto * d3d11DeviceContext1 = getD3D11DeviceContext( pCommandObject );

		Bitmask<UINT> updateFlags = 0;
		if( ( pUploadDesc.bufferRegion.size == mBufferProperties.byteSize ) || pUploadDesc.flags.isSet( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT ) )
		{
			updateFlags.set( D3D11_COPY_DISCARD );
		}

        if( pUploadDesc.bufferRegion.size == mBufferProperties.byteSize )
        {
            d3d11DeviceContext1->UpdateSubresource1( mD3D11Buffer.Get(), 0, nullptr, pUploadDesc.inputDataDesc.pointer, 0, 0, updateFlags );
        }
        else
        {
            D3D11_BOX targetBufferBox;
            targetBufferBox.top = 0;
            targetBufferBox.bottom = 1;
            targetBufferBox.front = 0;
            targetBufferBox.back = 1;
            targetBufferBox.left = trunc_numeric_cast<UINT>( pUploadDesc.bufferRegion.offset );
            targetBufferBox.right = trunc_numeric_cast<UINT>( pUploadDesc.bufferRegion.offset + pUploadDesc.bufferRegion.size );

            d3d11DeviceContext1->UpdateSubresource1( mD3D11Buffer.Get(), 0, &targetBufferBox, pUploadDesc.inputDataDesc.pointer, 0, 0, updateFlags );
        }
	}

	bool DX11GPUBuffer::validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode )
	{
		return GPUBuffer::validateMapRequest( pRegion, pMapMode );
	}

	bool DX11GPUBuffer::validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo )
	{
		if( !GPUBuffer::validateBufferCreateInfo( pCreateInfo ) )
		{
			return false;
		}

		constexpr uint32 cxDX11GPUBufferMemorySupportedFlags = E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT;
		pCreateInfo.memoryFlags = pCreateInfo.memoryFlags & cxDX11GPUBufferMemorySupportedFlags;

		return true;
	}

	DX11GPUBufferDesc DX11GPUBuffer::translateBufferDesc( const GPUBufferCreateInfo & pCreateInfo )
	{
		DX11GPUBufferDesc dx11GPUBufferDesc;
		dx11GPUBufferDesc.bindFlags = DX11CoreAPIProxy::translateDX11BufferBindFlags( pCreateInfo.resourceFlags );
		dx11GPUBufferDesc.cpuAccessFlags = 0;
		dx11GPUBufferDesc.resourceMiscFlags = 0;
		dx11GPUBufferDesc.usage = D3D11_USAGE_DEFAULT;

		if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT ) )
		{
			dx11GPUBufferDesc.usage = D3D11_USAGE_IMMUTABLE;
		}
		if( pCreateInfo.resourceFlags.isSetAnyOf( E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT ) )
		{
			dx11GPUBufferDesc.usage = D3D11_USAGE_STAGING;
		}
		if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT ) && ( dx11GPUBufferDesc.usage != D3D11_USAGE_DYNAMIC ) )
		{
			dx11GPUBufferDesc.usage = D3D11_USAGE_DYNAMIC;
			dx11GPUBufferDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_WRITE );
		}

		if( pCreateInfo.memoryFlags.isSet( E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_BIT ) )
		{
			dx11GPUBufferDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_READ );
		}
		if( pCreateInfo.memoryFlags.isSet( E_GPU_MEMORY_MAP_FLAG_ACCESS_WRITE_BIT ) )
		{
			dx11GPUBufferDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_WRITE );
		}

		auto indirectBufferFlags = E_GPU_BUFFER_BIND_FLAG_INDIRECT_DISPATCH_BUFFER_BIT | E_GPU_BUFFER_BIND_FLAG_INDIRECT_DRAW_BUFFER_BIT;
		if( pCreateInfo.resourceFlags.isSetAnyOf( indirectBufferFlags ) )
		{
			dx11GPUBufferDesc.resourceMiscFlags.set( D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS );
		}

		if( pCreateInfo.resourceFlags.isSetAnyOf( E_GPU_BUFFER_BIND_FLAG_SHADER_UAV_BUFFER_BIT ) )
		{
			dx11GPUBufferDesc.resourceMiscFlags.set( D3D11_RESOURCE_MISC_BUFFER_STRUCTURED );
		}

		return dx11GPUBufferDesc;
	}

	ID3D11DeviceContext1 * DX11GPUBuffer::getD3D11DeviceContext( void * pCommandObject )
	{
		return reinterpret_cast<DX11CommandList *>( pCommandObject )->mD3D11DeviceContext1.Get();
	}

} /* namespace ts3 */
} /* namespace gpuapi */
