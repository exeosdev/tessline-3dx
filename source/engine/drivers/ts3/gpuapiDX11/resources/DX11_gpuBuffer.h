
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_GPU_BUFFER_H__
#define __TS3DRIVER_GPUAPI_DX11_GPU_BUFFER_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( DX11GPUBuffer );

	struct DX11GPUBufferDesc
	{
		Bitmask<D3D11_BIND_FLAG> bindFlags;
		Bitmask<D3D11_CPU_ACCESS_FLAG> cpuAccessFlags;
		Bitmask<D3D11_RESOURCE_MISC_FLAG> resourceMiscFlags;
		D3D11_USAGE usage;
	};

	class DX11GPUBuffer : public GPUBuffer
	{
	public:
		ComPtr<ID3D11Buffer> const mD3D11Buffer;

		DX11GPUBuffer( DX11GPUDevice & pDX11GPUDevice,
		               const ResourceMemoryInfo & pResourceMemory,
		               const GPUBufferProperties & pBufferProperties,
		               ComPtr<ID3D11Buffer> pD3D11Buffer );

		virtual ~DX11GPUBuffer();

		static DX11GPUBufferHandle create( DX11GPUDevice & pDX11GPUDevice,
		                                   const GPUBufferCreateInfo & pCreateInfo );

	private:
	    virtual bool mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode ) override;

		virtual void unmap( void * pCommandObject ) override;

		virtual void flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc ) override;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) override;

		virtual bool validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode ) override;

		static bool validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo );

		static DX11GPUBufferDesc translateBufferDesc( const GPUBufferCreateInfo & pCreateInfo );

		static ID3D11DeviceContext1 * getD3D11DeviceContext( void * pCommandObject );

	private:
		struct DX11BufferMapInfo
		{
			D3D11_MAP requestedMapMode;
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		};

		DX11BufferMapInfo _dx11MapInfo;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_BUFFER_H__
