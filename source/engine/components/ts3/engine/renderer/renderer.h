
#pragma once

#ifndef __TS3_ENGINE_SHADOW_RENDERER_H__
#define __TS3_ENGINE_SHADOW_RENDERER_H__

#include "commonRendererDefs.h"

#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3
{

	class Renderer
	{
	public:
		Renderer( ShaderLibraryHandle pShaderLibrary );
		virtual ~Renderer();

		virtual void createRendererResources();

	protected:
		gpuapi::GPUDevice & _gpuDevice;
		ShaderLibraryHandle _shaderLibrary;
	};


} // namespace ts3

#endif // __TS3_ENGINE_SHADOW_RENDERER_H__
