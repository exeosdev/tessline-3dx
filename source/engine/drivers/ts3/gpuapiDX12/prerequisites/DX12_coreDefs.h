
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_CORE_DEFS_H__
#define __TS3DRIVER_GPUAPI_DX12_CORE_DEFS_H__

namespace ts3::gpuapi
{

	constexpr SIZE_T cvD3D12CPUDescriptorPtrInvalid = ts3::Limits<SIZE_T>::maxValue;

	constexpr D3D12_CPU_DESCRIPTOR_HANDLE cvD3D12CPUDescriptorEmpty{ cvD3D12CPUDescriptorPtrInvalid };

	constexpr auto cvD3D12CommandListTypeInvalid = static_cast<D3D12_COMMAND_LIST_TYPE>( ts3::Limits<uint32>::maxValue );

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX12_CORE_DEFS_H__
